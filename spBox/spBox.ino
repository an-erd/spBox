/*
MIT License

Copyright (c) 2017 Andreas Erdmann

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

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
	com.initialize();
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
	LCDML_DISP_resetIsTimer();
}

void loop() {
	sensors.checkAccelGyroMag();
	sensors.checkTempPress();

	button.check();
	rotenc.check();

	com.checkPing();
	if (com.getAndClearInternetChanged()) {
		display.setInternetAvailable(com.getInternetAvailable());
	}
	display.setMqttAvailable(com.getMqttAvailable());
	com.checkMqtt();
	//com.checkOta();

	// display init screen
	if ((millis() - g_lcdml_initscreen) >= _LCDML_DISP_cfg_initscreen_time) {
		g_lcdml_initscreen = millis();

		LCDML_DISP_jumpToFunc(LCDML_FUNC_initscreen);
	}
	yield();

	LCDML_run(_LCDML_priority);

	yield();
}