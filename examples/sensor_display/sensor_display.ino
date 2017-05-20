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

#include <wire.h>
#include "spbox_sensors.h"
#include "spbox_display.h"
#include "myconfig.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();
	delay(2000);

	display.initializeDisplay();
	sensors.initializeAccelGyro(MPU6050_ACCEL_FS_16, MPU6050_GYRO_FS_2000);
	sensors.initializeMag();
	sensors.initializeBarometer();

	sensors.setupUpdateAccelGyroMag();
	sensors.startUpdateAccelGyroMag();
	sensors.setupUpdateTempPress();
	sensors.startUpdateTempPress();

	sensors.initializeVBat();

	//sensors.onAccelGyroMagEvent([](accelGyroMagEvent_t e) {
	//	Serial.printf("onAccelGyroMagEvent event: heading: ");
	//	Serial.println(e.heading);
	//	//e.ax_f, e.ay_f, e.az_f, e.gx_f, e.gy_f, e.gz_f);
	//});

	//sensors.onTempPressAltiEvent([](tempPressAltiEvent_t e) {
	//	Serial.printf("onTempPressAltiEvent event: temp: "); Serial.print(e.temperature);
	//	Serial.print(", press: "); Serial.print(e.pressure);
	//	Serial.print(", alti: "); Serial.println(e.altitude);
	//});
}

void loop() {
	display.updatePrintBufferScrTest();
	display.updateDisplayWithPrintBuffer();
	display.display();

	sensors.checkAccelGyroMag();
	sensors.checkTempPress();

	delay(0);
}