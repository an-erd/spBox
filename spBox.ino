// I2Cdevlib:
// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!
//
// Rotary Encoder:
// The code for the rotary encoder has been copied from http://playground.arduino.cc/Main/RotaryEncoders,
// Int0 & Int1 example using bitRead() with debounce handling and true Rotary Encoder pulse tracking, J.Carter(of Earth)
//

#include <arduino.h>
#include "c_types.h"
#include "user_interface.h"
#include "osapi.h"
#include <LCDMenuLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED_WiFi.h>

#define	SERIAL_STATUS_OUTPUT
#undef MEASURE_PREFORMANCE

const char* ssid = "W12";
const char* password = "EYo6Hv4qRO7P1JSpAqZCH6vGVPHwznRWODIIIdhd1pBkeWCYie0knb1pOQ9t2cc";

#define ENCODER_PIN_A	12
#define ENCODER_PIN_B	14
#define ENCODER_SW		13
#define LED_R			0		// rot enc led red (and huzzah led red)
#define LED_G			2		// rot enc led green (and huzzah led blue)

#define THRESHOLD		7		// debounce threshold in milliseconds
#define DELAY_MS_1HZ	1000	// milliseconds delay ->  1 Hz
#define DELAY_MS_2HZ	500
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000

// update_temperature_pressure_step
#define SENSOR_PAUSED					0
#define SENSOR_REQ_TEMP					1
#define SENSOR_READ_TEMP_REQ_PRESSURE	2
#define SENSOR_READ_PRESSURE			3
#define SENSOR_DONE						4

#define MPU6050_AXOFFSET	0
#define MPU6050_AYOFFSET	0
#define MPU6050_AZOFFSET	0
#define MPU6050_A_GAIN		2048		// MPU6050_ACCEL_FS_16

#define MPU6050_GXOFFSET	0
#define MPU6050_GYOFFSET	0
#define MPU6050_GZOFFSET	0
#define MPU6050_G_GAIN		(16.4)		// MPU6050_GYRO_FS_2000
#define MPU6050_DEG_RAD_CONV		0.01745329251994329576	// CONST
#define MPU6050_GAIN_DEG_RAD_CONV	0.00106422515365507901	// MPU6050_DEG_RAD_CONV / MPU6050_G_GAIN

#define MAX_NUMBER_DISPLAY_SCREENS		3
#define DISPLAY_SCR_OVERVIEW			0
#define DISPLAY_SCR_MAXVALUES			1
#define DISPLAY_SCR_WLAN_STATUS			2

///// LCDML 
// lib config
#define _LCDML_DISP_cfg_button_press_time          200    // button press time in ms

// ********************************************************************* 
// LCDML TYPE SELECT
// *********************************************************************
// settings for lcd 
#define _LCDML_DISP_cols             20
#define _LCDML_DISP_rows             4  


// *********************************************************************
// LCDML MENU/DISP
// *********************************************************************
// create menu
// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    11

// LCDML_root        => layer 0 
// LCDML_root_X      => layer 1 
// LCDML_root_X_X    => layer 2 
// LCDML_root_X_X_X  => layer 3 
// LCDML_root_... 	 => layer ... 

// LCDMenuLib_add(id, group, prev_layer_element, new_element_num, lang_char_array, callback_function)
LCDML_DISP_init(_LCDML_DISP_cnt);
LCDML_DISP_add(0, _LCDML_G1, LCDML_root, 1, "Information", LCDML_FUNC_information);
LCDML_DISP_add(1, _LCDML_G1, LCDML_root, 2, "Time info", LCDML_FUNC_timer_info);
LCDML_DISP_add(2, _LCDML_G1, LCDML_root, 3, "Settings", LCDML_FUNC);
LCDML_DISP_add(3, _LCDML_G1, LCDML_root_3, 1, "Change value", LCDML_FUNC);
LCDML_DISP_add(4, _LCDML_G1, LCDML_root_3, 2, "Something", LCDML_FUNC);
LCDML_DISP_add(5, _LCDML_G1, LCDML_root, 4, "Program", LCDML_FUNC);
LCDML_DISP_add(6, _LCDML_G1, LCDML_root_4, 1, "Program 1", LCDML_FUNC);
LCDML_DISP_add(7, _LCDML_G1, LCDML_root_4_1, 1, "P1 start", LCDML_FUNC);
LCDML_DISP_add(8, _LCDML_G1, LCDML_root_4_1, 2, "Settings", LCDML_FUNC);
LCDML_DISP_add(9, _LCDML_G1, LCDML_root_4_1_2, 1, "Warm", LCDML_FUNC);
LCDML_DISP_add(10, _LCDML_G1, LCDML_root_4_1_2, 2, "Long", LCDML_FUNC);
LCDML_DISP_add(11, _LCDML_G1, LCDML_root_4, 2, "Program 2", LCDML_FUNC_p2);
LCDML_DISP_createMenu(_LCDML_DISP_cnt);



// ********************************************************************* 
// LCDML BACKEND (core of the menu, do not change here anything yet)
// ********************************************************************* 
// define backend function  
#define _LCDML_BACK_cnt    1  // last backend function id

LCDML_BACK_init(_LCDML_BACK_cnt);
LCDML_BACK_new_timebased_dynamic(0, (20UL), _LCDML_start, LCDML_BACKEND_control);
LCDML_BACK_new_timebased_dynamic(1, (10000000UL), _LCDML_stop, LCDML_BACKEND_menu);
LCDML_BACK_create();
////////////////////////////////////



#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

typedef struct
{
	int16_t		ax, ay, az;			// accel values (sensor)
	float		ax_f, ay_f, az_f;	// accel float values (calculated)
	float		max_ax_f, max_ay_f, max_az_f, min_ax_f, min_ay_f, min_az_f;

	int16_t		gx, gy, gz;			// gyro values (sensor)
	float		gx_f, gy_f, gz_f;	// gyro float values (calculated)
	float		max_gx_f, max_gy_f, max_gz_f, min_gx_f, min_gy_f, min_gz_f;

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
	long		rotaryHalfSteps;
	bool		changed_halfSteps;
	long		actualRotaryTicks;
	bool		changed_rotEnc;
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
Adafruit_FeatherOLED_WiFi display = Adafruit_FeatherOLED_WiFi();
//Adafruit_SSD1306  display = Adafruit_SSD1306();

sGlobalSensors	sensors;
volatile bool	do_update_accel_gyro_mag;
volatile bool	do_update_temperature_pressure;
volatile bool	do_update_temperature_pressure_step;

sGlobalDisplay	display_struct;
volatile sGlobalRotEnc	rotenc;
volatile sGlobalButton button;

bool	WLAN_initialized;
bool	WLAN_status_on;

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

// rotary encoder and rotary encoder button interrupt routines
void int0() {
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

	rotenc.changed_halfSteps = true;
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
	accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);		// scale factor 2048
	accelgyro.setFullScaleGyroRange(MPU6050_GYRO_FS_2000);		// scale factor 16.4

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
	rotenc.changed_halfSteps = false;
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
	//sensors.gx_f = (float)(sensors.gx - MPU6050_GXOFFSET) / MPU6050_G_GAIN;		// deg/s
	//sensors.gy_f = (float)(sensors.gy - MPU6050_GYOFFSET) / MPU6050_G_GAIN;
	//sensors.gz_f = (float)(sensors.gz - MPU6050_GZOFFSET) / MPU6050_G_GAIN;
	sensors.gx_f = (float)(sensors.gx - MPU6050_GXOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;	// rad/s
	sensors.gy_f = (float)(sensors.gy - MPU6050_GYOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;
	sensors.gz_f = (float)(sensors.gz - MPU6050_GZOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;
}

void reset_min_max_accelgyro() {
	sensors.max_ax_f = sensors.min_ax_f = sensors.ax_f;
	sensors.max_ay_f = sensors.min_ay_f = sensors.ay_f;
	sensors.max_az_f = sensors.min_az_f = sensors.az_f;
	sensors.max_gx_f = sensors.min_gx_f = sensors.gx_f;
	sensors.max_gy_f = sensors.min_gy_f = sensors.gy_f;
	sensors.max_gz_f = sensors.min_gz_f = sensors.gz_f;
}

void update_min_max_accelgyro() {
	if (sensors.ax_f > sensors.max_ax_f)
		sensors.max_ax_f = sensors.ax_f;
	if (sensors.ay_f > sensors.max_ay_f)
		sensors.max_ay_f = sensors.ay_f;
	if (sensors.az_f > sensors.max_az_f)
		sensors.max_az_f = sensors.az_f;
	if (sensors.ax_f < sensors.min_ax_f)
		sensors.min_ax_f = sensors.ax_f;
	if (sensors.ay_f < sensors.min_ay_f)
		sensors.min_ay_f = sensors.ay_f;
	if (sensors.az_f < sensors.min_az_f)
		sensors.min_az_f = sensors.az_f;
	if (sensors.gx_f > sensors.max_gx_f)
		sensors.max_gx_f = sensors.gx_f;
	if (sensors.gy_f > sensors.max_gy_f)
		sensors.max_gy_f = sensors.gy_f;
	if (sensors.gz_f > sensors.max_gz_f)
		sensors.max_gz_f = sensors.gz_f;
	if (sensors.gx_f < sensors.min_gx_f)
		sensors.min_gx_f = sensors.gx_f;
	if (sensors.gy_f < sensors.min_gy_f)
		sensors.min_gy_f = sensors.gy_f;
	if (sensors.gz_f < sensors.min_gz_f)
		sensors.min_gz_f = sensors.gz_f;
}

void calc_mag()
{
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(sensors.mz, sensors.my);
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
		if (rotenc.actualRotaryTicks == DISPLAY_SCR_MAXVALUES)		// AAARGH TODO
			update_min_max_accelgyro();
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
			if (rotenc.actualRotaryTicks == DISPLAY_SCR_MAXVALUES) {
				reset_min_max_accelgyro();
			}
		}
		else {
			if (!button.very_long_diff_change) {
				// lange HIGH ->  LOW
				Serial.println(" Button: lange HIGH jetzt LOW");

				switch_WLAN((WLAN_status_on ? false : true));
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

void check_rotary_encoder() {
	if (rotenc.changed_halfSteps) {
		rotenc.changed_halfSteps = false;
		if (rotenc.rotaryHalfSteps % 2 == 0) {
			rotenc.actualRotaryTicks = rotenc.rotaryHalfSteps / 2;
			rotenc.actualRotaryTicks %= MAX_NUMBER_DISPLAY_SCREENS;		// TODO different screens hard coded uuuugh.
			rotenc.changed_rotEnc = true;
		}
	}
}

void check_menu() {
}

void update_display_scr1() {
	dtostrf_sign(sensors.ax_f, 4, 2, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.ay_f, 4, 2, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.az_f, 4, 2, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[0], 21, "A %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf_sign(sensors.gx_f, 4, 2, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.gy_f, 4, 2, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.gz_f, 4, 2, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[1], 21, "G %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf(sensors.heading, 3, 0, display_struct.tempbuffer[0]);
	dtostrf(sensors.temperature, 5, 2, display_struct.tempbuffer[1]);
	snprintf(display_struct.displaybuffer[2], 21, "H %s T %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1]);

	dtostrf(sensors.altitude, 4, 0, display_struct.tempbuffer[0]);
	dtostrf(sensors.pressure / 100.0, 4, 0, display_struct.tempbuffer[1]);
	dtostrf(WiFi.status(), 1, 0, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[3], 21, "Alt %s P %s   W%s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);
}

void update_display_scr2() {
	dtostrf_sign(sensors.max_ax_f, 5, 1, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.max_ay_f, 5, 1, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.max_az_f, 5, 1, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[0], 21, "A/ %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf_sign(sensors.min_ax_f, 5, 1, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.min_ay_f, 5, 1, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.min_az_f, 5, 1, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[1], 21, "A\\ %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf_sign(sensors.max_gx_f, 5, 1, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.max_gy_f, 5, 1, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.max_gz_f, 5, 1, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[2], 21, "G/ %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);

	dtostrf_sign(sensors.min_gx_f, 5, 1, display_struct.tempbuffer[0]);
	dtostrf_sign(sensors.min_gy_f, 5, 1, display_struct.tempbuffer[1]);
	dtostrf_sign(sensors.min_gz_f, 5, 1, display_struct.tempbuffer[2]);
	snprintf(display_struct.displaybuffer[3], 21, "G\\ %s %s %s", display_struct.tempbuffer[0], display_struct.tempbuffer[1], display_struct.tempbuffer[2]);
}

void update_display_scr3() {
	int8_t rssi = WiFi.RSSI();
	uint32_t ipAddress = WiFi.localIP();

	display.setBatteryVisible(false);
	display.setConnectedVisible(true);
	display.setRSSIVisible(true);
	display.setRSSIIcon(true);

	display.setConnected(WiFi.status() == WL_CONNECTED);

	display.setRSSI(rssi);
	display.setIPAddress(ipAddress);

	display.refreshIcons();
	display.clearMsgArea();
	display.println("test");
}

void update_display_print_buffer() {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(display_struct.displaybuffer[0]);
	display.println(display_struct.displaybuffer[1]);
	display.println(display_struct.displaybuffer[2]);
	display.println(display_struct.displaybuffer[3]);
}

void update_display()
{
	switch (rotenc.actualRotaryTicks)
	{
	case DISPLAY_SCR_OVERVIEW:
		update_display_scr1();
		update_display_print_buffer();
		break;
	case DISPLAY_SCR_MAXVALUES:
		update_display_scr2();
		update_display_print_buffer();
		break;
	case DISPLAY_SCR_WLAN_STATUS:
		update_display_scr3();
		break;
	default:
		break;
	}
}

void initialize_WLAN() {
	Serial.println("Initializing WLAN");
	WLAN_status_on = false;
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	//while (WiFi.waitForConnectResult() != WL_CONNECTED) {
	//	Serial.println("Connection Failed! Rebooting...");
	//	delay(5000);
	//	ESP.restart();
	//}
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("WLAN status: ");
	Serial.println(WiFi.status());

	WLAN_initialized = true;
	WLAN_status_on = true;
}

void switch_WLAN(bool turn_on) {
	if (turn_on) {
		if (!WLAN_initialized) {
			initialize_WLAN();
		}
		else {
			WiFi.mode(WIFI_STA);			// WL_CONNECTED
			WiFi.begin();
			Serial.println("WLAN turned on");
			WLAN_status_on = true;
		}
	}
	else
	{
		WiFi.mode(WIFI_OFF);
		Serial.println("WLAN turned off");
		WLAN_status_on = false;
	}
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

	//initialize_WLAN();
	WiFi.mode(WIFI_OFF);
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


	//// LCDML
	Serial.println(F(_LCDML_VERSION)); // only for examples
									   // Enable all items with _LCDML_G1
	LCDML_DISP_groupEnable(_LCDML_G1); // enable group 1
									   // LCDMenu Setup
	LCDML_setup(_LCDML_BACK_cnt);
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
	check_rotary_encoder();

	check_menu();
	update_display();

	if (rotenc.actualRotaryTicks == DISPLAY_SCR_WLAN_STATUS)
		display_struct.update_display = true;

	if (display_struct.update_display) {
		display_struct.update_display = false;
		update_display();
	}

	LCDML_run(_LCDML_priority);

#ifdef MEASURE_PREFORMANCE
	Serial.print("performance us: ");
	Serial.print(-perfStopWatch_getvalues);
	Serial.print(" ");
	Serial.println(-perfStopWatch_output);
#endif

	yield();
	display.display();
	yield();
}