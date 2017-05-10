#include <EEPROM.h>
#include <LCDMenuLib.h>

#include "missing_str_util.h"
#include "myconfig.h"
#include "credentials.h"
#include "spbox_com.h"
#include "spbox_conf.h"
#include "spbox_display.h"
#include "spbox_sensors.h"
#include "rotenc.h"
#include "button.h"
#include "LCDML_DEFS.h"

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
	EEPROM.begin(512);

	conf.initialize(false);
	com.setConf(&conf);
	com.initializeWlan();
	com.initializeOta();
	com.initializeMQTT();
	initialize_GPIO();

	display.initializeDisplay();
	sensors.initializeAccelGyro(conf.getAccelRangeScale(), conf.getGyroRangeScale());
	sensors.initializeMag();
	sensors.initializeBarometer();

	sensors.setupUpdateAccelGyroMag();
	sensors.startUpdateAccelGyroMag();
	sensors.setupUpdateTempPress();
	sensors.startUpdateTempPress();

	sensors.initializeVBat();

	rotenc.initialize();
	rotenc.start();
	button.initialize();
	button.start();

	button.onButtonChangeEvent([](buttonChangeEvent_t e) {
		switch (e) {
		case H_L_SHORT:
			LCDML_BUTTON_enter();
			break;
		case H_L_LONG:
			LCDML_BUTTON_left();
			break;
		case H_L_VERYLONG:
			LCDML_BUTTON_right();
			break;
		default:
			break;
		}
	});
	rotenc.onRotencPosEvent([](rotencPosEvent_t e) {
		switch (e.event) {
		case CW:
			LCDML_BUTTON_up();
			break;
		case CCW:
			LCDML_BUTTON_down();
		default:
			break;
		}
	});

	LCDML_DISP_groupEnable(_LCDML_G1);
	LCDML_setup(_LCDML_BACK_cnt);
}

void loop() {
	sensors.checkAccelGyroMag();
	sensors.checkTempPress();
	button.check();
	rotenc.check();
	com.checkOta();
	LCDML_run(_LCDML_priority);
	yield();
}