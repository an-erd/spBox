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
if the sensor module (gy-87) is turned on for some time, it's getting warm on the PCB and inside the box (~30°C). Find way to deactivate the modle if not necessary.

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




// I2Cdevlib:
// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!
//
// Rotary Encoder:
// The code for the rotary encoder has been copied from http://playground.arduino.cc/Main/RotaryEncoders,
// Int0 & Int1 example using bitRead() with debounce handling and true Rotary Encoder pulse tracking, J.Carter(of Earth)
//


void test_macros(void)
{
	static unsigned long i = 0;

	i++;
	iprintf(INFO, "Debug macro test run %lu\n\n", i);
	iprintf(DEBUG, "This is a debug message.\n");
	iprintf(WARNING, "This is a warning.\n");
	iprintf(ERROR, "This is an error\n\n");
}

void BUTTON::check() {
	//	ArduinoOTA.handle();		// TODO

	if (!changed_)
		return;

	if (int_signal_) {
		if (!button.time_long_diff_) {
			// kurz LOW -> HIGH
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz LOW jetzt HIGH");
#endif
		}
		else {
			// lange LOW -> HIGH
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: lange LOW jetzt HIGH");
#endif
		}
	}
	else {
		if (!time_long_diff_) {
			// kurz HIGH -> LOW
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz HIGH jetzt LOW");
#endif
			//if (rotenc.actualRotaryTicks == DISPLAY_SCR_MAXVALUES) {
			//	reset_min_max_accelgyro();
			//}
			LCDML_button_pressed = true;
		}
		else {
			if (!time_verylong_diff_) {
				// lange HIGH ->  LOW
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: lange HIGH jetzt LOW");
#endif

				switch_WLAN((gConfig.wlan_enabled ? false : true));
			}
			else
			{
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: very lange HIGH jetzt LOW");
#endif
			}
		}
	}
	changed_ = false;
	time_long_diff_ = false;
	time_verylong_diff_ = false;
}



	if ((millis() - last) > 5100) {
		//Serial.println(millis() - last);
		last = millis();
		Serial.print(i); Serial.print(" ");
		Serial.print(NTP.getTimeDateString()); Serial.print(" ");
		Serial.print(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
		Serial.print("WiFi is ");
		Serial.print(WiFi.isConnected() ? "connected" : "not connected"); Serial.print(". ");
		Serial.print("Uptime: ");
		Serial.print(NTP.getUptimeString()); Serial.print(" since ");
		Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());

		i++;
	}