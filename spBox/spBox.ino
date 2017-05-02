//#include <GDBStub.h>
#include <EEPROM.h>

#include "missing_str_util.h"
#include "spBox.h"
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
	EEPROM.begin(512);

	conf.initialize(false);
	com.setConf(&conf);
	com.initializeWlan();
	com.initializeOta();
	com.initializeMQTT();
	initialize_GPIO();

	display.initializeDisplay();
	sensors.initializeAccelGyro();
	sensors.initializeMag();
	sensors.initializeBarometer();

	sensors.setupUpdateAccelGyroMag();
	sensors.startUpdateAccelGyroMag();
	sensors.setupUpdateTempPress();
	sensors.startUpdateTempPress();

	sensors.onAccelGyroMagEvent([](accelGyroMagEvent_t e) {
		Serial.printf("onAccelGyroMagEvent event: heading: ");
		Serial.println(e.heading);
		//e.ax_f, e.ay_f, e.az_f, e.gx_f, e.gy_f, e.gz_f);
	});

	sensors.onTempPressAltiEvent([](tempPressAltiEvent_t e) {
		Serial.printf("onTempPressAltiEvent event: temp: "); Serial.print(e.temperature);
		Serial.print(", press: "); Serial.print(e.pressure);
		Serial.print(", alti: "); Serial.println(e.altitude);
	});

	rotenc.initialize();
	rotenc.start();
	button.initialize();
	button.start();

	button.onButtonChangeEvent([](buttonChangeEvent_t e) {
		Serial.printf("onButtonChangeEvent: %d\n", e);
	});

	//rotenc.onRotencChangeEvent([](rotencChangeEvent_t e) {
	//	Serial.printf("onRotEncChangeEvent: %d\n", e);
	//});

	rotenc.onRotencPosEvent([](rotencPosEvent_t e) {
		Serial.printf("onRotEncChangeEvent event: %d, diff: %d, pos: %d\n", e.event, e.diff, e.pos);
	});

	//LCDML_DISP_groupEnable(_LCDML_G1);
	//LCDML_setup(_LCDML_BACK_cnt);
}

void loop() {
	display.updatePrintBufferScrTest();
	display.updateDisplayWithPrintBuffer();
	display.display();

	sensors.checkAccelGyroMag();
	sensors.checkTempPress();

	button.check();
	rotenc.check();

	com.checkOta();

	//LCDML_run(_LCDML_priority);

	yield();
}