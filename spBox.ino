#define DEBUG_SPBOX //Uncomment this to enable debug messages over serial port
#define DBG_PORT Serial
#define DEBUG_SPBOX

#ifdef DEBUG_SPBOX
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

#include <arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <LCDMenuLib.h>
#include "LCDML_DEFS.h"
#include "missing_str_util.h"
#include "user_config.h"
#include "spbox_conf.h"
#include "rotenc.h"
#include "button.h"
#include "spbox_display.h"
#include "spbox_sensors.h"
#include "spbox_com.h"

void initialize_WLAN() {
	DEBUGLOG("Initializing WLAN\r\n");
	if (gConfig.wlan_initialized) {
		DEBUGLOG("WLAN already initialized\r\n");
		return;
	}

	if (gConfig.wlan_enabled) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
#ifdef DEBUG_SPBOX
		WiFi.onEvent([](WiFiEvent_t e) {
			Serial.printf("Event wifi -----> %d\n", e);
		});
#endif

		gotIpEventHandler = WiFi.onStationModeGotIP(onSTAGotIP);
		disconnectedEventHandler = WiFi.onStationModeDisconnected(onSTADisconnected);

		DEBUGLOG("IP address: %s, WiFI status: %d\r\n", WiFi.localIP(), WiFi.status());

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

#ifdef DEBUG_SPBOX
		ArduinoOTA.onStart([]() { Serial.println("Arduino OTA Start"); });
		ArduinoOTA.onEnd([]() { Serial.println("\nArduino OTA End"); });
		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
			Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
		ArduinoOTA.onError([](ota_error_t error) {
			Serial.printf("Arduino OTA Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR) Serial.println("End Failed");
		});
#endif

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

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();
	EEPROM.begin(512);

	initialize_basic_config();
	initialize_WLAN();
	initialize_OTA();
	initialize_GPIO();
	initialize_display();
	initialize_accelgyro();
	initialize_mag();
	initialize_barometer();

	rotenc.initialize();
	rotenc.start();

	button.initialize();
	button.start();

	setup_update_accel_gyro_mag_timer();

	LCDML_DISP_groupEnable(_LCDML_G1);
	LCDML_setup(_LCDML_BACK_cnt);

	connect_adafruit_io();
	setup_update_mqtt_timer();
}

void loop() {
	check_sensor_updates();
	check_sensor_calc();
	button.check();
	rotenc.checkRotaryEncoder();

	check_mqtt();

	LCDML_run(_LCDML_priority);

	static int i = 0;
	static int last = 0;

	if (syncEventTriggered) {
		processSyncEvent(ntpEvent);
		syncEventTriggered = false;
	}

	ArduinoOTA.handle();

	yield();
}