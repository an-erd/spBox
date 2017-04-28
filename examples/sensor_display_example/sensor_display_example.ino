#include <wire.h>
#include "spbox_sensors.h"
#include "spbox_display.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();

	sensors.initializeAccelGyro();
	sensors.initializeMag();
	sensors.initializeBarometer();
}

void loop() {
	delay(0);
}