#include <wire.h>
#include "spbox_sensors.h"
#include "spbox_display.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();

	display.initializeDisplay();

	sensors.initializeAccelGyro();
	sensors.initializeMag();
	sensors.initializeBarometer();
}

void loop() {
	display.updatePrintBufferScrTest();
	display.updateDisplayWithPrintBuffer();
	display.display();

	delay(0);
}