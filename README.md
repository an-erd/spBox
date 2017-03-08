# spBox

I2C devices
===========
address 0x3C 	Adafruit Featherwing OLED
address 0x1E	GY-87/HMC5883L 	magnetometer
address 0x68	GY-87/MPU6050 	3-axis gyroscope and a 3-axis accelerometer
address 0x77	GY-87/BMP180 	barometric pressure/temperature/altitude

PIN assignment
==============
#0		Rotary Encoder LED R		(Huzzah OLED Button A, red led)
#2		Rotary Encoder LED G		(Huzzah OLED Button C, blue led)
#4		SDA
#5		SCL
#12		Rotary Encoder Pin A
#13		Rotary Encoder Switch 
#14		Rotary Encoder Pin B
#15		
#16									(Huzzah OLED Button B)
GND		Rotary Encoder Pin C/GND
VCC		Rotary Encoder VCC
		
WLAN Status
===========
WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
WL_IDLE_STATUS      = 0,
WL_NO_SSID_AVAIL    = 1,
WL_SCAN_COMPLETED   = 2,
WL_CONNECTED        = 3,
WL_CONNECT_FAILED   = 4,
WL_CONNECTION_LOST  = 5,
WL_DISCONNECTED     = 6		


Software TIMER usage
====================
name	

Read sensors
============
  barometer.setControl(BMP085_MODE_TEMPERATURE);
  wait barometer.getMeasureDelayMicroseconds();
  temperature = barometer.getTemperatureC();

  barometer.setControl(BMP085_MODE_PRESSURE_3);
  wait barometer.getMeasureDelayMicroseconds();
  pressure = barometer.getPressure();



ARDUINO Libraries used
======================
i2cdevlib (see https://www.i2cdevlib.com/usage)
Adafruit GFX
Adafruit SSD1306

Power consumption
=================
not optimizes yet. Device should power off or put itslf in sleep mode after whatever time.
How to wake up again? Reset=

Temperature issue
=================
if the sensor module (gy-87) is turned on for some time, it's getting warm on the PCB and inside the box (~30�C). Find way to deactivate the modle if not necessary.

Serial Output
=============

a/g:	0.40	0.52	0.79	-233	141	6	mag:	149	-347	-532	heading:	0.00	T/P/A	23.00	95865.00	464.83Rot.enc	0
performance: 29829 2651
(first performance value: get sensor data
second value: output)

Sample (debug) output line
==========================
display.println("abcdefghijklmnopqrstu");
display.println("123456789012345678901");
display.println("a:  +0.70 +0.46 +0.59");
display.println("H: 215 T: 21.3");


___________________________________________________________
Crash:


___________________________________________________________
TODO:
- get rid of this crucial stuff
	rotenc.actualRotaryTicks %= MAX_NUMBER_DISPLAY_SCREENS;		// TODO different screens hard coded uuuugh.
- handle OTA
- power saving, in particular
	* sleep modes
	* shut down and restart
	* at least minimize GY-87 power consumption, desolder voltage regulator, LED
	* maybe use a switch and do a EN->GND for the voltage regulator on the HUZZAH FEATHER

___________________________________________________________
Power saving
* https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html, e.g. void MPU6050::setSleepEnabled, ...
* PCB stuff, see https://bengoncalves.wordpress.com/2015/10/02/arduino-power-down-mode-with-accelerometer-compass-and-pressure-sensor/


see also:
https://courses.cs.washington.edu/courses/cse466/14au/labs/l4/MPU6050BasicExample.ino, in particular "void LowPowerAccelOnlyMPU6050()"




___________________________________________________________
Old code segments:


	//// display tab-separated accel/gyro x/y/z values
	//Serial.print("a/g:\t");
	//Serial.print(sensors.ax_f); Serial.print("\t");
	//Serial.print(sensors.ay_f); Serial.print("\t");
	//Serial.print(sensors.az_f); Serial.print("\t");
	//Serial.print(sensors.gx_f); Serial.print("\t");
	//Serial.print(sensors.gy_f); Serial.print("\t");
	//Serial.print(sensors.gz_f); Serial.print("\t");

	//Serial.print("mag:\t");
	//Serial.print(sensors.mx); Serial.print("\t");
	//Serial.print(sensors.my); Serial.print("\t");
	//Serial.print(sensors.mz); Serial.print("\t");

	//Serial.print("heading:\t");
	//Serial.print(sensors.heading); Serial.print("\t");

	//Serial.print("T/P/A\t");
	//Serial.print(sensors.temperature); Serial.print("\t");
	//Serial.print(sensors.pressure); Serial.print("\t");
	//Serial.print(sensors.altitude); Serial.print("\t");

	//if (rotenc.changed_rotEnc || button.changed) {
	//
	//	Serial.print("Rot.enc:\t");
	//	Serial.print(rotenc.actualRotaryTicks);
	//	Serial.print(" Button: changed ");
	//	Serial.print(button.changed);
	//	Serial.print(", long ");
	//	Serial.print(button.long_diff_change);
	//	Serial.print(", int_signal ");
	//	Serial.print(button.int_signal);
	//	Serial.print(",  button signal ");
	//	Serial.print(digitalRead(ENCODER_SW));
	//	Serial.println("");

	//	rotenc.changed_rotEnc = false;
	//	button.changed = false;
	//	button.long_diff_change = false;
	//}