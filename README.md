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

# spBox

In this project several libraries and tools will be used which will be 
referenced here:

I2Cdevlib:
==========
The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks
to Jeff Rowberg and further contributers for providing that great work! In 
particular, the following sensor drivers are used are used:
  - HMC5883L
  - MPU6050
  - BMP180

Rotary Encoder:
===============
The code for the rotary encoder is inspired (and partially based) by the 
discussions in http://playground.arduino.cc/Main/RotaryEncoders, in particular
"Int0 & Int1 example using bitRead() with debounce handling and true Rotary
Encoder pulse tracking, J.Carter(of Earth)"

LCDMenuLib:
===========
The code for the menu navigation uses the excellent library LCDMenuLib (see
https://github.com/Jomelo/LCDMenuLib) by Nils Feldkämper. The following 
files follow the implementation proposal from LCDMenuLib, but are using own
functions to accomplish the functions: LCDML_CONTROL.ino, LCDML_DISP.ino, 
LCDML_FUNC_BACKEND.ino, LCDML_FUNC_DISP.ino and LCDML_DEFS.h.

Display:
========
The hardware (ESP board and OLED extension) used (and thus some of the 
libraries) are from Adafruit, so the following libraries are used:
  - Adafruit_GFX
  - Adafruit_SSD1306
  - Adafruit_FeatherOLED_WiFi

NTP:
====
For NTP time syncrhonisation the NtpClientLib library from German Martin is
used.

PING:
=====
To test the availability of an internet connection, the asynchronous ping 
library "AsyncPing" is used, see https://github.com/akaJes/AsyncPing.

MQTT:
=====
The non-blocking MQTT client "AsyncMQTT" from Marvin Roger is used, see 
https://github.com/marvinroger/async-mqtt-client. It works out of the box 
incl. keepalive, and nice callbacks. The library is based on "ESPAsyncESP", 
see https://github.com/me-no-dev/ESPAsyncTCP. 

OTA:
====
ArduinoOTA

ESP8266:
========
The Arduino IDE is used, with  Visual Studio 2017 and Visual Micro (Arduino 
IDE for Microsoft Visual Studio, http://www.visualmicro.com/) is used as 
development environment, using the ESP8266 core from
https://github.com/esp8266/Arduino providing the "board".


The board is configured as follows: 

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
EN		Connected via switch to GND to turn of the board.

Software TIMER usage
====================
In the software the following timer are used. In addition to them, there may 
be used mor (e.g. in time.h used from NTPClientLib)

LOCAL os_timer_t timerUpdateAccelGyroMag;	// read Accel, Gyro and Mag regularly
LOCAL os_timer_t timerUpdateTempPress;		// prepare temperature and pressure regularly
LOCAL os_timer_t timerUpdateSteps;			// handle different BMP085 preparation duration for temperature and pressure
LOCAL os_timer_t timerUpdateMqtt;			// pusblish health data to IO

Power consumption
=================
not optimizes yet. Device should power off or put itself in sleep mode after 
whatever time. How to wake up again? Reset?
* https://www.i2cdevlib.com/docs/html/class_m_p_u6050.html
* PCB stuff, see https://bengoncalves.wordpress.com/2015/10/02/arduino-power-down-mode-with-accelerometer-compass-and-pressure-sensor/
see also https://courses.cs.washington.edu/courses/cse466/14au/labs/l4/MPU6050BasicExample.ino, in particular "void LowPowerAccelOnlyMPU6050()"
* Display https://forums.adafruit.com/viewtopic.php?f=47&t=45520


Temperature issue
=================
if the sensor module (gy-87) is turned on for some time, it's getting warm on the PCB and inside the box (~30°C). Find way to deactivate the module if not necessary.

onEvent handler
===============
The following event handler are examples that can be used to retrieve 
information on events occuring. For example, the onButtonChangeEvent is used
to trigger the LCDMenuLib "Enter" function.

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

button.onButtonChangeEvent([](buttonChangeEvent_t e) {
	Serial.printf("onButtonChangeEvent: %d\n", e);
});

rotenc.onRotencChangeEvent([](rotencChangeEvent_t e) {
	Serial.printf("onRotEncChangeEvent: %d\n", e);
});

rotenc.onRotencPosEvent([](rotencPosEvent_t e) {
	Serial.printf("onRotEncChangeEvent event: %d, diff: %d, pos: %d\n", e.event, e.diff, e.pos);
});


Menu:
=====

Sensor
	Overview
	Accel/Gyro Max. Werte		Reset: lange drücken
	Temp/Druck/Höhe
	Kompass
	Zurueck
Status
	WLAN/Batterie/NTP
	MQTT
	Zurueck
Konfiguration
	WLAN
	Sensor
		Accel
		Gyro
		Kompass
		Höhe
		Zurueck
	MQTT
	EEPROM
		Save to EEPROM
		Read from EEPROM
		Zurueck
	Zurueck
Information
	Uhrzeit
	Besitzer
	QR
	Sensor-Ausrichtung
		Accel
		Gyro
		Kompass
		Zurueck
	Zurueck

Umlaute:
========
Umlaute can be used using octal values (e.g., \204 for the German "ae")
	char	dec		hex		oct
	ä/ae	132		0x84	\204
	Ä/AE	142		0x8e	\216
	ö/oe	148		0x94	\224
	Ö/OE	153		0x99	\231
	ü/ue	129		0x81	\201
	Ü/UE	154		0x9a	\232
	°/grad	9		0x09	\011	

Compass calibration:
====================
The following process had/can be used to performa a compass calibration:
	1) x, y, z Werte ausgeben lassen, insb. den Kompass im ebenen Zustand hinlegen (Verbaut ist so, dass +x nach unten, und -z nach vorne geht, also +y nach rechts)
	2) den Kompass langsam einmal drehen
	3) �ber min und max f�r x und y den Mittelpunkt finden -> dies ergibt den Offset
	3) �ber Spanne y und z den skalieren auf Werte (nach Anwendung Offset) von -100..100 f�r x und y Achse -> dies ergibt Scale

	4) Mit echtem Kompass auf Norden ausrichten, Werte messen. 
		Hier:	Mag	194	0	-100 
	super Link: http://www.germersogorb.de/html/kalibrierung_des_hcm5883l.html 
	Total Field  48,378.8 nT -> 0.48378G
	Raw total field: 527

PCB and sensor orientation:
===========================
There a several different sensor orientation information and thus I fixed on 
the NED definition, see https://en.wikipedia.org/wiki/Axes_conventions, 
  - North: positiv x-axis, this is the heading
  - East: positiv y-axis, pointing to right
  - Down: positiv z-axis, pointing down
The sensors used are positioned on the GY-87 board (consisting of accel/gyro 
MPU6050 and magnetometer HMC5883L). It is placed vertically in the box.
  - GY-87 axis as assembled are: positiv x-axis down, positiv y-axis right.
  - this is than mapped to NED in functions 
From MPU-6050 datasheet: "When the device is placed on a flat surface, it will
measure 0g on the X- and Y-axes and +1g on the Z-axis." This is inverse to 
common literature. In our implementation, wie follow the details in 
https://cache.freescale.com/files/sensors/doc/app_note/AN4248.pdf, Document 
Number: AN4248 Rev. 4.0, 11/2015

Useful links:
  - http://www.starlino.com/imu_guide.html
	http://www.olliw.eu/storm32bgc-wiki/Manually_Setting_the_IMU_Orientation
