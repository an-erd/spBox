// I2Cdevlib:
// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!
//
// Rotary Encoder:
// The code for the rotary encoder has been copied from http://playground.arduino.cc/Main/RotaryEncoders,
// Int0 & Int1 example using bitRead() with debounce handling and true Rotary Encoder pulse tracking, J.Carter(of Earth)
//

#include <arduino.h>
extern "C" {
#include "c_types.h"
#include "user_interface.h"
// #include "ets_sys.h"
#include "osapi.h"
}

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define	SERIAL_STATUS_OUTPUT

#define BUTTON_A		0
#define BUTTON_B		16
#define BUTTON_C		2
#define ENCODER_PIN_A	12
#define ENCODER_PIN_B	14
#define LED				0

#define THRESHOLD		7		// debounce threshold in milliseconds
#define DELAY_MS_1HZ	1000	// milliseconds delay ->  1 Hz
#define DELAY_MS_10HZ	100		// milliseconds delay -> 10 Hz

// update_temperature_pressure_step
#define SENSOR_PAUSED				0
#define SENSOR_REQ_TEMP					1
#define SENSOR_READ_TEMP_REQ_PRESSURE	2
#define SENSOR_READ_PRESSURE			3
#define SENSOR_DONE						4


#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

typedef struct
{
	int16_t		ax, ay, az;			// accel values (sensor)
	float		ax_f, ay_f, az_f;	// accel float values (calculated)

	int16_t		gx, gy, gz;			// gyro values (sensor)

	int16_t		mx, my, mz;			// magnetometer values (sensor)
	float		heading;			// calculated heading (calculated)

	float		temperature;		// temperature (sensor)
	float		pressure;			// pressure (sensor)
	float		altitude;			// altitude (sensor)

	bool		do_update_accel_gyro_mag;	
	bool		do_update_temperature_pressure;
	bool		do_update_temperature_pressure_step;
	int8_t		update_temperature_pressure_step;
	bool		changed_accel_gyro_mag;			// -> re-calculate
	bool		changed_temperatur_pressure;	// -> re-calculate
} sGlobalSensors;

typedef struct
{
	uint32_t	int0time;			// ISR threshold
	uint32_t	int1time;			// ISR threshold
	uint8_t		int0signal;
	uint8_t		int0history;
	uint8_t		int1signal;
	uint8_t		int1history;
	long		rotaryHalfSteps;	// internal counter used for rot enc position
	long		actualRotaryTicks;	// rot enc position
	bool		changed_rotEnc;		// flag -> rotary encoder position changed
} sGlobalRotEnc;

typedef struct
{
	char displaybuffer[4][21];  // 4 lines with 21 chars each
	char tempbuffer[3][15];     // temp for float to str conversion
	bool update_display;
} sGlobalDisplay;

MPU6050		accelgyro;
HMC5883L	mag;
BMP085		barometer;
Adafruit_SSD1306  display = Adafruit_SSD1306();	// TODO

sGlobalSensors	sensors;
sGlobalDisplay	display_struct;
volatile sGlobalRotEnc	rotenc;

LOCAL os_timer_t timer_update_temperature_pressure;
LOCAL os_timer_t timer_update_temperature_pressure_steps;
LOCAL os_timer_t timer_update_accel_gyro_mag;
LOCAL os_timer_t timer_update_display;

int32_t lastMicros;		// TODO

// = dtostre() function experimental ============================
char * dtostrf_sign(double number, signed char width, unsigned char prec, char *s) {
	bool negative = false;

	if (isnan(number)) {
		strcpy(s, "nan");
		return s;
	}
	if (isinf(number)) {
		strcpy(s, "inf");
		return s;
	}

	char* out = s;

	int fillme = width; // how many cells to fill for the integer part
	if (prec > 0) {
		fillme -= (prec + 1);
	}

	// Handle negative numbers
	if (number < 0.0) {
		negative = true;
		fillme--;
		number = -number;
	}
	else {
		fillme--;
	}

	// Round correctly so that print(1.999, 2) prints as "2.00"
	// I optimized out most of the divisions
	double rounding = 2.0;
	for (uint8_t i = 0; i < prec; ++i)
		rounding *= 10.0;
	rounding = 1.0 / rounding;

	number += rounding;

	// Figure out how big our number really is
	double tenpow = 1.0;
	int digitcount = 1;
	while (number >= 10.0 * tenpow) {
		tenpow *= 10.0;
		digitcount++;
	}

	number /= tenpow;
	fillme -= digitcount;

	// Pad unused cells with spaces
	while (fillme-- > 0) {
		*out++ = ' ';
	}

	// Handle negative sign
	if (negative) {
		*out++ = '-';
	}
	else {
		*out++ = ' ';
	}

	// Print the digits, and if necessary, the decimal point
	digitcount += prec;
	int8_t digit = 0;
	while (digitcount-- > 0) {
		digit = (int8_t)number;
		if (digit > 9) digit = 9; // insurance
		*out++ = (char)('0' | digit);
		if ((digitcount == prec) && (prec > 0)) {
			*out++ = '.';
		}
		number -= digit;
		number *= 10.0;
	}

	// make sure the string is terminated
	*out = 0;
	return s;
}
// ====================================================================

// rotary encoder interrupt routines
void int0() {
	if (millis() - rotenc.int0time < THRESHOLD)
		return;
	rotenc.int0history = rotenc.int0signal;
	rotenc.int0signal = digitalRead(ENCODER_PIN_A);
	if (rotenc.int0history == rotenc.int0signal)
		return;
	rotenc.int0time = millis();
	if (rotenc.int0signal == rotenc.int1signal)
		rotenc.rotaryHalfSteps--;
	else
		rotenc.rotaryHalfSteps++;
}

void int1() {
	if (millis() - rotenc.int1time < THRESHOLD)
		return;
	rotenc.int1history = rotenc.int1signal;
	rotenc.int1signal = digitalRead(ENCODER_PIN_B);
	if (rotenc.int1history == rotenc.int1signal)
		return;
	rotenc.int1time = millis();
}

void initialize_GPIO() {
	pinMode(BUTTON_A, INPUT_PULLUP);
	pinMode(BUTTON_B, INPUT_PULLUP);
	pinMode(BUTTON_C, INPUT_PULLUP);
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
}

void initialize_display() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.display();
	display_struct.update_display = true;
}

void initialize_accelgyro() {
	accelgyro.setI2CMasterModeEnabled(false);
	accelgyro.setI2CBypassEnabled(true);
	accelgyro.setSleepEnabled(false);
	accelgyro.initialize();
	sensors.do_update_accel_gyro_mag = false;
	sensors.changed_accel_gyro_mag = false;
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
#endif // SERIAL_STATUS_OUTPUT
}

void initialize_mag() {
	mag.initialize();
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
#endif // SERIAL_STATUS_OUTPUT
}

void initialize_barometer() {
	barometer.initialize();
	sensors.do_update_temperature_pressure = false;
	sensors.changed_temperatur_pressure = false;
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(barometer.testConnection() ? "BMP180 connection successful" : "BMP180 connection failed");
#endif // SERIAL_STATUS_OUTPUT
}

void initialize_rotary_encoder() {
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), int0, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), int1, CHANGE);

	rotenc.int0signal = digitalRead(ENCODER_PIN_A);
	rotenc.int0history = rotenc.int0signal;
	rotenc.int1signal = digitalRead(ENCODER_PIN_B);
	rotenc.int1history = rotenc.int1signal;
	rotenc.rotaryHalfSteps = 0;
	rotenc.actualRotaryTicks = 0;
	rotenc.changed_rotEnc = false;
}

LOCAL void ICACHE_FLASH_ATTR update_temperature_pressure_cb(void *arg) {
	sensors.do_update_temperature_pressure = true;
}

LOCAL void ICACHE_FLASH_ATTR update_temperature_pressure_step_cb(void *arg) {
	sensors.do_update_temperature_pressure_step = true;
}

LOCAL void ICACHE_FLASH_ATTR update_accel_gyro_mag_cb(void *arg) {
	sensors.do_update_accel_gyro_mag = true;
}

LOCAL void ICACHE_FLASH_ATTR update_display_cb(void *arg) {
	 display_struct.update_display = true;
}


void setup_update_temperature_pressure_timer()
{
	os_timer_disarm(&timer_update_temperature_pressure);
	os_timer_setfn(&timer_update_temperature_pressure, (os_timer_func_t *) update_temperature_pressure_cb, (void *)0);
	os_timer_arm(&timer_update_temperature_pressure, DELAY_MS_1HZ, true);	// DELAY_MS_1HZ
}

void setup_update_accel_gyro_mag_timer()
{
	os_timer_disarm(&timer_update_accel_gyro_mag);
	os_timer_setfn(&timer_update_accel_gyro_mag, (os_timer_func_t *)update_accel_gyro_mag_cb, (void *)0);
	os_timer_arm(&timer_update_accel_gyro_mag, DELAY_MS_10HZ, true);
}

void setup_update_display_timer()
{
	os_timer_disarm(&timer_update_display);
	os_timer_setfn(&timer_update_display, (os_timer_func_t *)update_display_cb, (void *)0);
	os_timer_arm(&timer_update_display, 2000, true);
}

void get_accelgyro()
{
	accelgyro.getMotion6(&sensors.ax, &sensors.ay, &sensors.az, &sensors.gx, &sensors.gy, &sensors.gz);
}

void get_mag()
{
	mag.getHeading(&sensors.mx, &sensors.my, &sensors.mz);
}

void get_temperature_pressure() 
{
	switch (sensors.update_temperature_pressure_step){
		case SENSOR_REQ_TEMP:
			barometer.setControl(BMP085_MODE_TEMPERATURE);
			os_timer_disarm(&timer_update_temperature_pressure_steps);
			os_timer_setfn(&timer_update_temperature_pressure_steps, (os_timer_func_t *)update_temperature_pressure_step_cb, (void *)0);
			os_timer_arm(&timer_update_temperature_pressure_steps, barometer.getMeasureDelayMilliseconds(), false);
			break;

		case SENSOR_READ_TEMP_REQ_PRESSURE:
			sensors.temperature = barometer.getTemperatureC();
			barometer.setControl(BMP085_MODE_PRESSURE_3);
			os_timer_disarm(&timer_update_temperature_pressure_steps);
			os_timer_setfn(&timer_update_temperature_pressure_steps, (os_timer_func_t *)update_temperature_pressure_cb, (void *)0);
			os_timer_arm(&timer_update_temperature_pressure_steps, barometer.getMeasureDelayMilliseconds(), false);
			break;

		case SENSOR_READ_PRESSURE:
			sensors.pressure = barometer.getPressure();
			os_timer_disarm(&timer_update_temperature_pressure_steps);
			sensors.update_temperature_pressure_step = SENSOR_DONE;
			break;

		default:
			break;
	}
}

void calc_accelgyro()
{
	sensors.ax_f = sensors.ax / 16384.0;	// TODO
	sensors.ay_f = sensors.ay / 16384.0;
	sensors.az_f = sensors.az / 16384.0;
}

void calc_mag()
{
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(sensors.my, sensors.mz);
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180 / M_PI;
}

void calc_altitude()
{
	// calculate absolute altitude in meters based on known pressure
	// (may pass a second "sea level pressure" parameter here,
	// otherwise uses the standard value of 101325 Pa)
	sensors.altitude = barometer.getAltitude(sensors.pressure);
}

void check_sensor_updates()
{
	if (sensors.do_update_accel_gyro_mag) {
		sensors.do_update_accel_gyro_mag = false;
		get_accelgyro();
		get_mag();
		sensors.changed_accel_gyro_mag = true;
	}

	if (sensors.do_update_temperature_pressure) {
		sensors.do_update_temperature_pressure = false;
		sensors.update_temperature_pressure_step = SENSOR_REQ_TEMP;	// first step
		get_temperature_pressure();
	} else if (sensors.do_update_temperature_pressure_step) {
		sensors.do_update_temperature_pressure_step = false;
		sensors.update_temperature_pressure_step++;
		get_temperature_pressure();
	}
	if (sensors.update_temperature_pressure_step == SENSOR_DONE){
		sensors.update_temperature_pressure_step = SENSOR_PAUSED;
		sensors.changed_temperatur_pressure = true;
	}
}

void check_sensor_calc()
{
	if (sensors.changed_accel_gyro_mag) {
		sensors.changed_accel_gyro_mag = false;
		calc_accelgyro();
		calc_mag();
	}
	if (sensors.changed_temperatur_pressure) {
		sensors.changed_temperatur_pressure = false;
		calc_altitude();
	}
}

void update_display()
{
	dtostrf_sign(sensors.ax_f, 4, 2, display_struct.tempbuffer[0]);   // -x.x
	dtostrf_sign(sensors.ay_f, 4, 2, display_struct.tempbuffer[1]);   // -x.x
	dtostrf_sign(sensors.az_f, 4, 2, display_struct.tempbuffer[2]);   // -x.x
	snprintf(display_struct.displaybuffer[0], 21, "A %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	snprintf(display_struct.displaybuffer[1], 21, "G: %+4d %+4d %+4d", sensors.gx, sensors.gy, sensors.gz);

	dtostrf(sensors.heading, 3, 0, display_struct.tempbuffer[0]);   // xxx
	dtostrf(sensors.temperature, 5, 2, display_struct.tempbuffer[1]);   // -xx.x
	snprintf(display_struct.displaybuffer[2], 21, "H: %s, T: %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1]);

	dtostrf(sensors.altitude, 4, 0, display_struct.tempbuffer[0]);   // xxxx
	dtostrf(sensors.pressure / 100.0, 4, 0, display_struct.tempbuffer[1]);   // xxxx
	snprintf(display_struct.displaybuffer[3], 21, "Alt: %s, P: %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1]);

	display.clearDisplay();
	display.setCursor(0, 0);

	display.println(display_struct.displaybuffer[0]);
	//  display.println(display_struct.displaybuffer[1]);
	display.println(display_struct.displaybuffer[2]);
	display.println(display_struct.displaybuffer[3]);
}

void setup() {
#if !defined(ESP8266)
	while (!Serial) delay(1);
#endif

	Serial.begin(115200);
	Wire.begin();

	// initialize devices
	initialize_GPIO();
	initialize_display();
	initialize_accelgyro();
	initialize_mag();
	initialize_barometer();
	initialize_rotary_encoder();

	sensors.update_temperature_pressure_step = SENSOR_PAUSED;
	sensors.do_update_temperature_pressure_step = false;
	setup_update_temperature_pressure_timer();
	setup_update_accel_gyro_mag_timer();
	setup_update_display_timer();
}

void loop() {

int32_t perfStopWatch_getvalues;
int32_t perfStopWatch_output;
perfStopWatch_getvalues = micros();

	check_sensor_updates();
	check_sensor_calc();
	
perfStopWatch_getvalues -= micros();
perfStopWatch_output = micros();

perfStopWatch_output -= micros();

	//if (display_struct.update_display) {
	//	display_struct.update_display = false;
	//	update_display();
	//}

Serial.print("performance: ");
Serial.print(-perfStopWatch_getvalues);
Serial.print(" ");
Serial.println(-perfStopWatch_output);

	yield();
	display.display();
	yield();

//	delay(100);
/*
	// display tab-separated accel/gyro x/y/z values
	Serial.print("a/g:\t");
	Serial.print(sensors.ax_f); Serial.print("\t");
	Serial.print(sensors.ay_f); Serial.print("\t");
	Serial.print(sensors.az_f); Serial.print("\t");
	Serial.print(sensors.gx); Serial.print("\t");
	Serial.print(sensors.gy); Serial.print("\t");
	Serial.print(sensors.gz); Serial.print("\t");

	Serial.print("mag:\t");
	Serial.print(sensors.mx); Serial.print("\t");
	Serial.print(sensors.my); Serial.print("\t");
	Serial.print(sensors.mz); Serial.print("\t");

	Serial.print("heading:\t");
	Serial.print(sensors.heading); Serial.print("\t");

	Serial.print("T/P/A\t");
	Serial.print(sensors.temperature); Serial.print("\t");
	Serial.print(sensors.pressure); Serial.print("\t");
	Serial.print(sensors.altitude);

	Serial.print("Rot.enc\t");

	rotenc.actualRotaryTicks = (rotenc.rotaryHalfSteps / 2);
	Serial.print(rotenc.actualRotaryTicks);
	Serial.println("");
*/
}