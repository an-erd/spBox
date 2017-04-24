#include "rotenc.h"
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
#include "BMP085_nb.h"
#include "rotenc.h"

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
	char displaybuffer[4][21];  // 4 lines with 21 chars each
	char tempbuffer[3][15];     // temp for float to str conversion
	bool update_display;
} sGlobalDisplay;

volatile sGlobalConfig gConfig;
sGlobalSensors	sensors;
sGlobalDisplay	display_struct;

volatile bool	do_update_accel_gyro_mag;
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

// Adafruit FeatherWing OLED
Adafruit_FeatherOLED_WiFi display = Adafruit_FeatherOLED_WiFi();

// Timer

LOCAL os_timer_t timer_update_accel_gyro_mag;
LOCAL os_timer_t timer_update_mqtt;

void update_accel_gyro_mag_cb(void *arg) {
	do_update_accel_gyro_mag = true;
}

void setup_update_accel_gyro_mag_timer()
{
	os_timer_disarm(&timer_update_accel_gyro_mag);
	os_timer_setfn(&timer_update_accel_gyro_mag, (os_timer_func_t *)update_accel_gyro_mag_cb, (void *)0);
	os_timer_arm(&timer_update_accel_gyro_mag, DELAY_MS_10HZ, true);
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
		if (!gConfig.wlan_initialized) {
			initialize_WLAN();
		}
		else {
			WiFi.mode(WIFI_STA);			// WL_CONNECTED
			WiFi.begin();
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println("WLAN turned on");
#endif
			gConfig.wlan_enabled = true;
		}
	}
	else
	{
		WiFi.mode(WIFI_OFF);
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println("WLAN turned off");
#endif
		gConfig.wlan_enabled = false;
	}
}

void initialize_basic_config()
{
	gConfig.wlan_initialized = false;
	gConfig.wlan_enabled = true;
	gConfig.ota_mode = OTA_IDE;
	gConfig.ntp_enabled = true;
	gConfig.aio_enabled = true;
	gConfig.accel_range_scale = MPU6050_ACCEL_FS_16;
	gConfig.gyro_range_scale = MPU6050_GYRO_FS_2000;
	gConfig.use_configured_sea_level = false;
	gConfig.sea_level_pressure = 101325;
}

void initialize_WLAN() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println("Initializing WLAN");
#endif
	if (gConfig.wlan_initialized) {
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println("WLAN already initialized");
#endif
		return;
	}

	if (gConfig.wlan_enabled) {
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
		gConfig.wlan_initialized = true;
	}
}

void initialize_OTA() {
	if (gConfig.ota_mode == OTA_OFF)
		return;

	if (gConfig.ota_mode | OTA_IDE) {
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
}

void initialize_GPIO() {
	// knob leds
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	digitalWrite(LED_R, HIGH);
	digitalWrite(LED_G, HIGH);

	// ADC for battery measurement
	pinMode(VBAT_PIN, INPUT);
}

void initialize_display() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.display();
	display_struct.update_display = true;	// TODO
}

void initialize_accelgyro() {
	accelgyro.setI2CMasterModeEnabled(false);
	accelgyro.setI2CBypassEnabled(true);
	accelgyro.setSleepEnabled(false);
	accelgyro.initialize();
	accelgyro.setFullScaleAccelRange(gConfig.accel_range_scale);
	accelgyro.setFullScaleGyroRange(gConfig.gyro_range_scale);

	do_update_accel_gyro_mag = false;		//TODO
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
	sensors.changed_temperatur_pressure = false;
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(barometer.testConnection() ? "BMP180 connection successful" : "BMP180 connection failed");
#endif // SERIAL_STATUS_OUTPUT
}

void setup() {
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
	rotenc.initialize();

	setup_update_accel_gyro_mag_timer();

	LCDML_DISP_groupEnable(_LCDML_G1);
	LCDML_setup(_LCDML_BACK_cnt);

	connect_adafruit_io();
	setup_update_mqtt_timer();
}

void loop() {
	check_sensor_updates();
	check_sensor_calc();
	rotenc.checkButton();
	rotenc.checkRotaryEncoder();

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

	ArduinoOTA.handle();
}

yield();
}