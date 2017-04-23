#include <arduino.h>
#include <LCDMenuLib.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include <Time.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED_WiFi.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <TimeLib.h>
#include <NtpClientLib.h>

#include "missing_str_util.h"
#include "user_config.h"
#include "LCDML_DEFS.h"

int		g_vbatADC;	// TODO global variable

enum OTAModes { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, };

typedef struct {
	// WLAN
	bool		wlan_initialized;
	bool		wlan_enabled;

	// OTA
	OTAModes	ota_mode;

	// NTP time
	bool		ntp_enabled;

	// Adafruit IO and Home Automation (W12)
	bool		aio_enabled;

	// Accel/Gyro
	uint8_t		accel_range_scale;
	uint8_t		gyro_range_scale;
	uint8_t		accel_gyro_orientation;

	// Magnetometer
	uint8_t		mag_orientation;

	// Barometer/Altitude
	bool		use_configured_sea_level;
	float		sea_level_pressure;
} sGlobalConfig;

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
	long		LCDML_rotenc_value;
	long		LCDML_rotenc_value_history;
} sGlobalRotEnc;

typedef struct
{
	uint32_t	int_time;
	uint8_t		int_signal;
	uint8_t		int_history;
	bool		changed;
	bool		long_diff_change;		// long time gone since change
	bool		very_long_diff_change;	// very long time gone since change
	bool		LCDML_button_pressed;
} sGlobalButton;

typedef struct
{
	char displaybuffer[4][21];  // 4 lines with 21 chars each
	char tempbuffer[3][15];     // temp for float to str conversion
	bool update_display;
} sGlobalDisplay;

volatile sGlobalConfig g_config;
sGlobalSensors	sensors;
sGlobalDisplay	display_struct;
volatile sGlobalRotEnc rotenc;
volatile sGlobalButton button;

volatile bool	do_update_accel_gyro_mag;
volatile bool	do_update_temperature_pressure;
volatile bool	do_update_temperature_pressure_step;
volatile bool	do_update_mqtt;

// WiFiClient, MQTT Client and Feed, event handler
WiFiClient				client;
WiFiEventHandler		gotIpEventHandler;
WiFiEventHandler		disconnectedEventHandler;
Adafruit_MQTT_Client	mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish	battery = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/battery");

// MPU6050 sensor board
MPU6050					accelgyro;
HMC5883L				mag;
BMP085					barometer;

// Adafruit FeatherWing OLED
Adafruit_FeatherOLED_WiFi display = Adafruit_FeatherOLED_WiFi();

// Timer
LOCAL os_timer_t timer_update_temperature_pressure;
LOCAL os_timer_t timer_update_temperature_pressure_steps;
LOCAL os_timer_t timer_update_accel_gyro_mag;
LOCAL os_timer_t timer_update_mqtt;

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

	pinMode(VBAT_PIN, INPUT);
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
	accelgyro.setFullScaleAccelRange(g_config.accel_range_scale);
	accelgyro.setFullScaleGyroRange(g_config.gyro_range_scale);

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

void update_temperature_pressure_cb(void *arg) {		// LOCAL void ICACHE_FLASH_ATTR ...
	do_update_temperature_pressure = true;
}

void update_temperature_pressure_step_cb(void *arg) {
	do_update_temperature_pressure_step = true;
}

void update_accel_gyro_mag_cb(void *arg) {
	do_update_accel_gyro_mag = true;
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

	rotenc.LCDML_rotenc_value = 0;
	rotenc.LCDML_rotenc_value_history = 0;
}

void initialize_button() {
	attachInterrupt(digitalPinToInterrupt(ENCODER_SW), int2, CHANGE);

	button.int_signal = digitalRead(ENCODER_SW);
	button.int_history = button.int_signal;
	button.changed = false;
}

void update_mqtt_cb(void *arg) {
	do_update_mqtt = true;
}

void setup_update_mqtt_timer()
{
	os_timer_disarm(&timer_update_mqtt);
	os_timer_setfn(&timer_update_mqtt, (os_timer_func_t *)update_mqtt_cb, (void *)0);
	os_timer_arm(&timer_update_mqtt, 10000, true);	// DELAY_MS_1MIN
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
		if (true)		// AAARGH TODO
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
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz LOW jetzt HIGH");
#endif
		}
		else {
			// lange LOW -> HIGH
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: lange LOW jetzt HIGH");
#endif
		}
	}
	else {
		if (!button.long_diff_change) {
			// kurz HIGH -> LOW
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz HIGH jetzt LOW");
#endif
			//if (rotenc.actualRotaryTicks == DISPLAY_SCR_MAXVALUES) {
			//	reset_min_max_accelgyro();
			//}
			button.LCDML_button_pressed = true;
		}
		else {
			if (!button.very_long_diff_change) {
				// lange HIGH ->  LOW
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: lange HIGH jetzt LOW");
#endif

				switch_WLAN((g_config.wlan_enabled ? false : true));
			}
			else
			{
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: very lange HIGH jetzt LOW");
#endif
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
			rotenc.LCDML_rotenc_value = rotenc.actualRotaryTicks;
		}
	}
}

void check_mqtt()
{
	if (!do_update_mqtt)
		return;

	do_update_mqtt = false;

	updateVbat();

	if (!battery.publish(g_vbatADC)) {
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Update vbat Failed."));
#endif
	}
	else {
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Update vbat Success!"));
#endif
	}
}

void update_print_buffer_scr1() {
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

void update_print_buffer_scr2() {
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

void updateVbat()
{
	float vbatFloat = 0.0F;
	float vbatLSB = 0.97751F;		// 1000mV/1023 -> mV per LSB
	float vbatVoltDiv = 0.201321;	// 271K/1271K resistor voltage divider

	g_vbatADC = analogRead(VBAT_PIN);
	vbatFloat = ((float)g_vbatADC * vbatLSB) / vbatVoltDiv;
	display.setBattery(vbatFloat / 1000.);

#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(g_vbatADC);
#endif
}

void update_display_scr3() {
	int8_t rssi = WiFi.RSSI();
	uint32_t ipAddress = WiFi.localIP();

	//int level = analogRead(VBAT_PIN);
	// analog read level is 10 bit 0-1023 (0V-1V).
	// resistors of the voltage divider 271K and 1M -> 271 / 1271 =
	// lipo values:
	// 3.14V -> 0.669V -> analog value 684
	// 4.20V -> 0.895V -> analog value 915
	//int perc_level = map(level, 684, 915, 0, 100); // level in percentage
	//float temp_volt = level * 0.97751 * 4.69;
	//display.setBattery(temp_volt);
	//Serial.print("Battery: "); Serial.print(temp_volt); Serial.print(", level "); Serial.println(perc_level);
	//Serial.println(level);
	//Adafruit_IO_Feed battery = aio.getFeed("battery");
	//battery.send(level);
	updateVbat();
	bool is_connected;
	is_connected = WiFi.status() == WL_CONNECTED;
	display.setConnected(is_connected);
	if (is_connected) {
		display.setRSSI(rssi);
		display.setIPAddress(ipAddress);
	}
	display.refreshIcons();
	display.clearMsgArea();
	//display.print(level);
	//switch (WiFi.status()) {
	//case WL_IDLE_STATUS:
	//	display.print("Idle Status");
	//	break;
	//case WL_NO_SSID_AVAIL:
	//	display.print("");
	//	break;
	//case WL_SCAN_COMPLETED:
	//	display.print("Scan Completed");
	//	break;
	//case WL_CONNECTED:
	//	display.print(WiFi.SSID());
	//	break;
	//case WL_CONNECT_FAILED:
	//	display.print("Connect Failed");
	//	break;
	//case WL_CONNECTION_LOST:
	//	display.print("Connection Lost");
	//	break;
	//case WL_DISCONNECTED:
	//	display.print("Disconnected");
	//	break;
	//case WL_NO_SHIELD:
	//	display.print("No Shield");
	//	break;
	//}

	display.display();
}

void update_display_with_print_buffer() {
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println(display_struct.displaybuffer[0]);
	display.println(display_struct.displaybuffer[1]);
	display.println(display_struct.displaybuffer[2]);
	display.println(display_struct.displaybuffer[3]);
	display.display();
}

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
	Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
	NTP.begin("pool.ntp.org", 1, true);
	NTP.setInterval(63);
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
	Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
	Serial.printf("Reason: %d\n", event_info.reason);
	NTP.stop(); // NTP sync can be disabled to avoid sync errors
}

void processSyncEvent(NTPSyncEvent_t ntpEvent) {
	if (ntpEvent) {
		Serial.print("Time Sync error: ");
		if (ntpEvent == noResponse)
			Serial.println("NTP server not reachable");
		else if (ntpEvent == invalidAddress)
			Serial.println("Invalid NTP server address");
	}
	else {
		Serial.print("Got NTP time: ");
		Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
	}
}

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void initialize_WLAN() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println("Initializing WLAN");
#endif
	if (g_config.wlan_enabled) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

#ifdef SERIAL_STATUS_OUTPUT
		WiFi.onEvent([](WiFiEvent_t e) {
			Serial.printf("Event wifi -----> %d\n", e);
		});
#endif

		gotIpEventHandler = WiFi.onStationModeGotIP(onSTAGotIP);
		disconnectedEventHandler = WiFi.onStationModeDisconnected(onSTADisconnected);

#ifdef SERIAL_STATUS_OUTPUT
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());
		Serial.print("WLAN status: ");
		Serial.println(WiFi.status());
#endif
		g_config.wlan_initialized = true;
	}
}

void connect_adafruit_io() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.print(F("Connecting to Adafruit IO... "));
#endif

	int8_t ret;

	while ((ret = mqtt.connect()) != 0) {
		switch (ret) {
		case 1: Serial.println(F("Wrong protocol")); break;
		case 2: Serial.println(F("ID rejected")); break;
		case 3: Serial.println(F("Server unavail")); break;
		case 4: Serial.println(F("Bad user/pass")); break;
		case 5: Serial.println(F("Not authed")); break;
		case 6: Serial.println(F("Failed to subscribe")); break;
		default: Serial.println(F("Connection failed")); break;
		}

		if (ret >= 0)
			mqtt.disconnect();

#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Retrying connection..."));
#endif
		delay(5000);
	}
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(F("Adafruit IO Connected!"));
#endif
}

void switch_WLAN(bool turn_on) {
	if (turn_on) {
		if (!g_config.wlan_initialized) {
			initialize_WLAN();
		}
		else {
			WiFi.mode(WIFI_STA);			// WL_CONNECTED
			WiFi.begin();
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println("WLAN turned on");
#endif
			g_config.wlan_enabled = true;
		}
	}
	else
	{
		WiFi.mode(WIFI_OFF);
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println("WLAN turned off");
#endif
		g_config.wlan_enabled = false;
	}
}

void initialize_OTA() {
	// ArduinoOTA.setPort(8266);
	// ArduinoOTA.setHostname("esp8266-XXX");
	// ArduinoOTA.setPassword((const char *)"123");

	ArduinoOTA.onStart([]() {
		Serial.println("Arduino OTA Start");
	});

	ArduinoOTA.onEnd([]() {
		Serial.println("\nArduino OTA End");
	});

	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});

	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Arduino OTA Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});

	ArduinoOTA.begin();
}

void initialize_basic_config()
{
	g_config.wlan_initialized = false;
	g_config.wlan_enabled = true;
	g_config.ota_mode = OTA_IDE;
	g_config.ntp_enabled = true;
	g_config.aio_enabled = true;
	g_config.accel_range_scale = MPU6050_ACCEL_FS_16;
	g_config.gyro_range_scale = MPU6050_GYRO_FS_2000;
	g_config.use_configured_sea_level = false;
	g_config.sea_level_pressure = 101325;
}

void setup() {
#if !defined(ESP8266)
	while (!Serial) delay(1);
#endif

#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();

	initialize_basic_config();
	initialize_WLAN();
	initialize_OTA();
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

	LCDML_DISP_groupEnable(_LCDML_G1);
	LCDML_setup(_LCDML_BACK_cnt);

	connect_adafruit_io();
	setup_update_mqtt_timer();
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

	check_mqtt();

	LCDML_run(_LCDML_priority);

	static int i = 0;
	static int last = 0;

	if (syncEventTriggered) {
		processSyncEvent(ntpEvent);
		syncEventTriggered = false;
	}

	if ((millis() - last) > 5100) {
		//Serial.println(millis() - last);
		last = millis();
		Serial.print(i); Serial.print(" ");
		Serial.print(NTP.getTimeDateString()); Serial.print(" ");
		Serial.print(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
		Serial.print("WiFi is ");
		Serial.print(WiFi.isConnected() ? "connected" : "not connected"); Serial.print(". ");
		Serial.print("Uptime: ");
		Serial.print(NTP.getUptimeString()); Serial.print(" since ");
		Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());

		i++;
	}

#ifdef MEASURE_PREFORMANCE
	Serial.print("performance us: ");
	Serial.print(-perfStopWatch_getvalues);
	Serial.print(" ");
	Serial.println(-perfStopWatch_output);
#endif

	ArduinoOTA.handle();

	yield();
}

void test_macros(void)
{
	static unsigned long i = 0;

	i++;
	iprintf(INFO, "Debug macro test run %lu\n\n", i);
	iprintf(DEBUG, "This is a debug message.\n");
	iprintf(WARNING, "This is a warning.\n");
	iprintf(ERROR, "This is an error\n\n");
}