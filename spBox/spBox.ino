#include <arduino.h>
#include <EEPROM.h>
#include "missing_str_util.h"
#include "myconfig.h"
#include "credentials.h"
//#include <LCDMenuLib.h>
#include "spbox_com.h"
#include "spbox_conf.h"
#include "spbox_display.h"
#include "spbox_sensors.h"
#include "rotenc.h"
#include "button.h"
//#include "LCDML_DEFS.h"

void initialize_GPIO() {
	// red and green knob leds
	pinMode(LED_R, OUTPUT);
	pinMode(LED_G, OUTPUT);
	digitalWrite(LED_R, HIGH);
	digitalWrite(LED_G, HIGH);
}

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();
	//EEPROM.begin(512);

	conf.initialize(false);
	com.setConf(&conf);
	com.initializeWlan();
	com.initializeOta();
	com.initializeMQTT();
	initialize_GPIO();

	//sensors.initializeAccelGyro();
	//sensors.initializeMag();
	//sensors.initializeBarometer();

	//display.begin();
	//rotenc.initialize();
	//rotenc.start();
	//button.initialize();
	//button.start();

	//LCDML_DISP_groupEnable(_LCDML_G1);
	//LCDML_setup(_LCDML_BACK_cnt);
}

void loop() {
	//button.check();
	//rotenc.check();

	//LCDML_run(_LCDML_priority);
	//ArduinoOTA.handle();

	yield();
}