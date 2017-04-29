#include <wire.h>
#include "spbox_sensors.h"
#include "spbox_display.h"
#include "user_config.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();
	delay(2000);

	display.initializeDisplay();
	sensors.initializeAccelGyro();
	sensors.initializeMag();
	//sensors.initializeBarometer();

	sensors.setupUpdateAccelGyroMag();
	sensors.startUpdateAccelGyroMag();

	//sensors.setupUpdateTempPress();
	//sensors.startUpdateTempPress();

	sensors.onAccelGyroMagEvent([](accelGyroMagEvent_t e) {
		Serial.printf("onAccelGyroMagEvent event: heading: ");
		Serial.println(e.heading);
		//e.ax_f, e.ay_f, e.az_f, e.gx_f, e.gy_f, e.gz_f);
	});
}

void loop() {
	display.updatePrintBufferScrTest();
	display.updateDisplayWithPrintBuffer();
	display.display();

	sensors.checkAccelGyroMag();
	//sensors.checkTempPress();

	delay(0);
}