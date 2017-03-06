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

#include <ESP8266WiFi\src\ESP8266WiFi.h>
#include <ESP8266mDNS\ESP8266mDNS.h>
#include <ESP8266WiFi\src\WiFiUdp.h>
#include <ArduinoOTA\ArduinoOTA.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

#define	SERIAL_STATUS_OUTPUT
#undef MEASURE_PREFORMANCE
#define DEBUG_ESP_OTA
#define DEBUG_ESP_PORT

const char* ssid = "W12";
const char* password = "EYo6Hv4qRO7P1JSpAqZCH6vGVPHwznRWODIIIdhd1pBkeWCYie0knb1pOQ9t2cc";

// #define BUTTON_A		0		// huzzah oled
#define BUTTON_B		16		// huzzah oled
// #define BUTTON_C		2		// huzzah oled
#define ENCODER_PIN_A	12
#define ENCODER_PIN_B	14
#define ENCODER_SW		13
#define LED_R			0		// rot enc led red (and huzzah led red)
#define LED_G			2		// rot enc led green (and huzzah led blue)

#define THRESHOLD		7		// debounce threshold in milliseconds
#define DELAY_MS_1HZ	1000	// milliseconds delay ->  1 Hz
#define DELAY_MS_2HZ	500		// milliseconds delay ->  2 Hz
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000

// update_temperature_pressure_step
#define SENSOR_PAUSED					0
#define SENSOR_REQ_TEMP					1
#define SENSOR_READ_TEMP_REQ_PRESSURE	2
#define SENSOR_READ_PRESSURE			3
#define SENSOR_DONE						4

#define MPU6050_GXOFFSET	0
#define MPU6050_GYOFFSET	0
#define MPU6050_GZOFFSET	0
#define MPU6050_G_GAIN		(32.8)
#define MPU6050_AXOFFSET	0
#define MPU6050_AYOFFSET	0
#define MPU6050_AZOFFSET	0
#define MPU6050_A_GAIN		16384

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

typedef struct
{
	int16_t		ax, ay, az;			// accel values (sensor)
	float		ax_f, ay_f, az_f;	// accel float values (calculated)

	int16_t		gx, gy, gz;			// gyro values (sensor)
	float		gx_f, gy_f, gz_f;	// gyro float values (calculated)

	int16_t		mx, my, mz;			// magnetometer values (sensor)
	float		heading;			// calculated heading (calculated)

	float		temperature;		// temperature (sensor)
	float		pressure;			// pressure (sensor)
	float		altitude;			// altitude (sensor)

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
	uint32_t	int_time;
	uint8_t		int_signal;
	uint8_t		int_history;
	bool		changed;
	bool		long_diff_change;		// long time gone since change
	bool		very_long_diff_change;	// very long time gone since change
} sGlobalButton;

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
volatile bool	do_update_accel_gyro_mag;
volatile bool	do_update_temperature_pressure;
volatile bool	do_update_temperature_pressure_step;

sGlobalDisplay	display_struct;
volatile sGlobalRotEnc	rotenc;
volatile sGlobalButton button;

bool	WIFI_status;
LOCAL os_timer_t timer_update_temperature_pressure;
LOCAL os_timer_t timer_update_temperature_pressure_steps;
LOCAL os_timer_t timer_update_accel_gyro_mag;
LOCAL os_timer_t timer_update_display;
LOCAL os_timer_t timer_long_button_press;

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
	bool calc_ticks = false;
	if (millis() - rotenc.int0time < THRESHOLD)
		return;
	rotenc.int0history = rotenc.int0signal;
	rotenc.int0signal = digitalRead(ENCODER_PIN_A);
	if (rotenc.int0history == rotenc.int0signal)
		return;
	rotenc.int0time = millis();
	if (rotenc.int0signal == rotenc.int1signal) {
		rotenc.rotaryHalfSteps--;
	}
	else {
		rotenc.rotaryHalfSteps++;
	}
	if (rotenc.rotaryHalfSteps % 2 == 0) {
		rotenc.actualRotaryTicks = rotenc.rotaryHalfSteps / 2;
		rotenc.changed_rotEnc = true;
	}
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

void int2() {
	uint32_t time_diff;
	time_diff = millis() - button.int_time;
	if (time_diff < THRESHOLD)
		return;
	button.int_history = button.int_signal;
	button.int_signal = digitalRead(ENCODER_SW);
	if (button.int_history == button.int_signal)
		return;
	button.long_diff_change = (time_diff > DELAY_MS_TWOSEC) ? true : false;
	button.very_long_diff_change = (time_diff > DELAY_MS_TENSEC) ? true : false;
	button.int_time = millis();
	button.changed = true;
}

void initialize_GPIO() {
	//pinMode(BUTTON_A, INPUT_PULLUP);
	pinMode(BUTTON_B, INPUT_PULLUP);
	//pinMode(BUTTON_C, INPUT_PULLUP);
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	digitalWrite(LED_R, HIGH);
	digitalWrite(LED_G, HIGH);

	pinMode(ENCODER_SW, OUTPUT);
	digitalWrite(ENCODER_SW, 0);
	pinMode(ENCODER_SW, INPUT_PULLUP);
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
	accelgyro.initialize();		// init w/gyro FS_250 -> div. by 131 and AFS_SEL=0, -> div. by 16,384
	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_1000);	// ... div. by 32.8
	do_update_accel_gyro_mag = false;
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
	do_update_temperature_pressure = false;
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

void initialize_button() {
	attachInterrupt(digitalPinToInterrupt(ENCODER_SW), int2, CHANGE);

	button.int_signal = digitalRead(ENCODER_SW);
	button.int_history = button.int_signal;
	button.changed = false;
}

void update_temperature_pressure_cb(void *arg) {		// LOCAL void ICACHE_FLASH_ATTR ...
	do_update_temperature_pressure = true;
}

void update_temperature_pressure_step_cb(void *arg) {
	do_update_temperature_pressure_step = true;
}

void update_accel_gyro_mag_cb(void *arg) {
	do_update_accel_gyro_mag = true;
}

void update_display_cb(void *arg) {
	display_struct.update_display = true;
}

void setup_update_temperature_pressure_timer()
{
	os_timer_disarm(&timer_update_temperature_pressure);
	os_timer_setfn(&timer_update_temperature_pressure, (os_timer_func_t *)update_temperature_pressure_cb, (void *)0);
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
	os_timer_arm(&timer_update_display, DELAY_MS_2HZ, true);
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
	switch (sensors.update_temperature_pressure_step) {
	case SENSOR_REQ_TEMP:
		//Serial.println("SENSOR_REQ_TEMP");
		barometer.setControl(BMP085_MODE_TEMPERATURE);
		os_timer_disarm(&timer_update_temperature_pressure_steps);
		os_timer_setfn(&timer_update_temperature_pressure_steps, (os_timer_func_t *)update_temperature_pressure_step_cb, (void *)0);
		os_timer_arm(&timer_update_temperature_pressure_steps, barometer.getMeasureDelayMilliseconds(), false);
		break;

	case SENSOR_READ_TEMP_REQ_PRESSURE:
		//Serial.println("SENSOR_READ_TEMP_REQ_PRESSURE");
		sensors.temperature = barometer.getTemperatureC();
		barometer.setControl(BMP085_MODE_PRESSURE_3);
		os_timer_disarm(&timer_update_temperature_pressure_steps);
		os_timer_setfn(&timer_update_temperature_pressure_steps, (os_timer_func_t *)update_temperature_pressure_step_cb, (void *)0);
		os_timer_arm(&timer_update_temperature_pressure_steps, barometer.getMeasureDelayMilliseconds(), false);
		break;

	case SENSOR_READ_PRESSURE:
		//Serial.println("SENSOR_READ_PRESSURE");
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
	sensors.ax_f = (float)(sensors.ax - MPU6050_AXOFFSET) / MPU6050_A_GAIN;
	sensors.ay_f = (float)(sensors.ay - MPU6050_AYOFFSET) / MPU6050_A_GAIN;
	sensors.az_f = (float)(sensors.az - MPU6050_AZOFFSET) / MPU6050_A_GAIN;
	sensors.gx_f = (float)(sensors.gx - MPU6050_GXOFFSET) / MPU6050_G_GAIN;
	sensors.gy_f = (float)(sensors.gy - MPU6050_GYOFFSET) / MPU6050_G_GAIN;
	sensors.gz_f = (float)(sensors.gz - MPU6050_GZOFFSET) / MPU6050_G_GAIN;
}

void calc_mag()
{
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(sensors.my, sensors.mz);
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180.0 / M_PI;
	sensors.heading = heading;
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
	if (do_update_accel_gyro_mag) {
		do_update_accel_gyro_mag = false;
		get_accelgyro();
		get_mag();
		sensors.changed_accel_gyro_mag = true;
	}

	if (do_update_temperature_pressure) {
		do_update_temperature_pressure = false;
		sensors.update_temperature_pressure_step = SENSOR_REQ_TEMP;	// first step
		get_temperature_pressure();
	}
	else if (do_update_temperature_pressure_step) {
		do_update_temperature_pressure_step = false;
		sensors.update_temperature_pressure_step++;
		get_temperature_pressure();
	}
	if (sensors.update_temperature_pressure_step == SENSOR_DONE) {
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

void check_button() {
	//	ArduinoOTA.handle();		// TODO

	if (!button.changed)
		return;

	if (button.int_signal) {
		if (!button.long_diff_change) {
			// kurz LOW -> HIGH
			Serial.println(" Button: kurz LOW jetzt HIGH");
		}
		else {
			// lange LOW -> HIGH
			Serial.println(" Button: lange LOW jetzt HIGH");
		}
	}
	else {
		if (!button.long_diff_change) {
			// kurz HIGH -> LOW
			Serial.println(" Button: kurz HIGH jetzt LOW");
		}
		else {
			if (!button.very_long_diff_change) {
				// lange HIGH ->  LOW
				Serial.println(" Button: lange HIGH jetzt LOW");
				if (WIFI_status == false) {
					WiFi.mode(WIFI_STA);			// WL_CONNECTED
					WiFi.begin();
					Serial.println("WIFI turned on");
					WIFI_status = true;
				}
				else {
					WiFi.mode(WIFI_OFF);
					Serial.println("WIFI turned off");
					WIFI_status = false;
				}
			}
			else
			{
				Serial.println(" Button: very lange HIGH jetzt LOW");
			}
		}
	}
	button.changed = false;
	button.long_diff_change = false;
	button.very_long_diff_change = false;
}

void update_display()
{
	dtostrf_sign(sensors.ax_f, 4, 2, display_struct.tempbuffer[0]);   // -x.x
	dtostrf_sign(sensors.ay_f, 4, 2, display_struct.tempbuffer[1]);   // -x.x
	dtostrf_sign(sensors.az_f, 4, 2, display_struct.tempbuffer[2]);   // -x.x
	snprintf(display_struct.displaybuffer[0], 21, "A %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf_sign(sensors.gx_f, 5, 1, display_struct.tempbuffer[0]);   // -x.x
	dtostrf_sign(sensors.gy_f, 5, 1, display_struct.tempbuffer[1]);   // -x.x
	dtostrf_sign(sensors.gz_f, 5, 1, display_struct.tempbuffer[2]);   // -x.x
	snprintf(display_struct.displaybuffer[1], 21, "G %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf(sensors.heading, 3, 0, display_struct.tempbuffer[0]);   // xxx
	dtostrf(sensors.temperature, 5, 2, display_struct.tempbuffer[1]);   // -xx.x
	snprintf(display_struct.displaybuffer[2], 21, "H %s T %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1]);

	dtostrf(sensors.altitude, 4, 0, display_struct.tempbuffer[0]);			// xxxx
	dtostrf(sensors.pressure / 100.0, 4, 0, display_struct.tempbuffer[1]);  // xxxx
	dtostrf(WiFi.status(), 1, 0, display_struct.tempbuffer[2]);				// x
	snprintf(display_struct.displaybuffer[3], 21, "Alt %s P %s   W%s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	display.clearDisplay();
	display.setCursor(0, 0);

	display.println(display_struct.displaybuffer[0]);
	display.println(display_struct.displaybuffer[1]);
	display.println(display_struct.displaybuffer[2]);
	display.println(display_struct.displaybuffer[3]);
}

void initialize_WLAN() {
	Serial.println("Booting");
	WIFI_status = false;
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	while (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Connection Failed! Rebooting...");
		delay(5000);
		ESP.restart();
	}
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("WLAN status: ");
	Serial.println(WiFi.status());

	WIFI_status = true;
}

void initialize_OTA() {
	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("esp8266-XXX");

	// No authentication by default
	// ArduinoOTA.setPassword((const char *)"123");

	ArduinoOTA.onStart([]() {
		Serial.println("Start");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
}

void setup() {
#if !defined(ESP8266)
	while (!Serial) delay(1);
#endif

	Serial.begin(115200);
	Wire.begin();

	initialize_WLAN();
	//initialize_OTA();
	initialize_GPIO();
	initialize_display();
	initialize_accelgyro();
	initialize_mag();
	initialize_barometer();
	initialize_rotary_encoder();
	initialize_button();

	sensors.update_temperature_pressure_step = SENSOR_PAUSED;
	do_update_temperature_pressure_step = false;
	setup_update_temperature_pressure_timer();
	setup_update_accel_gyro_mag_timer();
	setup_update_display_timer();
}

void loop() {
#ifdef MEASURE_PREFORMANCE
	int32_t perfStopWatch_getvalues;
	int32_t perfStopWatch_output;
	perfStopWatch_getvalues = micros();
#endif

	check_sensor_updates();
	check_sensor_calc();

#ifdef MEASURE_PREFORMANCE
	perfStopWatch_getvalues -= micros();
	perfStopWatch_output = micros();
	perfStopWatch_output -= micros();
#endif

	check_button();

	if (display_struct.update_display) {
		display_struct.update_display = false;
		update_display();
}
#ifdef MEASURE_PREFORMANCE
	Serial.print("performance us: ");
	Serial.print(-perfStopWatch_getvalues);
	Serial.print(" ");
	Serial.println(-perfStopWatch_output);
#endif

	yield();
	display.display();
	yield();

	//// display tab-separated accel/gyro x/y/z values
	//Serial.print("a/g:\t");
	//Serial.print(sensors.ax_f); Serial.print("\t");
	//Serial.print(sensors.ay_f); Serial.print("\t");
	//Serial.print(sensors.az_f); Serial.print("\t");
	//Serial.print(sensors.gx_f); Serial.print("\t");
	//Serial.print(sensors.gy_f); Serial.print("\t");
	//Serial.print(sensors.gz_f); Serial.print("\t");

	//Serial.print("mag:\t");
	//Serial.print(sensors.mx); Serial.print("\t");
	//Serial.print(sensors.my); Serial.print("\t");
	//Serial.print(sensors.mz); Serial.print("\t");

	//Serial.print("heading:\t");
	//Serial.print(sensors.heading); Serial.print("\t");

	//Serial.print("T/P/A\t");
	//Serial.print(sensors.temperature); Serial.print("\t");
	//Serial.print(sensors.pressure); Serial.print("\t");
	//Serial.print(sensors.altitude); Serial.print("\t");

	//if (rotenc.changed_rotEnc || button.changed) {
	//
	//	Serial.print("Rot.enc:\t");
	//	Serial.print(rotenc.actualRotaryTicks);
	//	Serial.print(" Button: changed ");
	//	Serial.print(button.changed);
	//	Serial.print(", long ");
	//	Serial.print(button.long_diff_change);
	//	Serial.print(", int_signal ");
	//	Serial.print(button.int_signal);
	//	Serial.print(",  button signal ");
	//	Serial.print(digitalRead(ENCODER_SW));
	//	Serial.println("");

	//	rotenc.changed_rotEnc = false;
	//	button.changed = false;
	//	button.long_diff_change = false;
	//}
}