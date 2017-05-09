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


onEvent handler
===============
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


___________________________________________________________
TODO:
- file header
- licence
- magnetometer sollte mit accel verknüpft werden bzgl. Lageerkennung/Ausgleich
- make gyro range scale configurable
- make accel range scale configurable
- complete menu structure and backend functions
- get QR code working
- get multiple WLANs working
- functional class method callback, i.e. fetch commit from 2.4.x ESP core tree, get rid of wrapper function, see https://github.com/esp8266/Arduino/pull/2745
- get MQTT running, incl keepalive
- implement LED usage
- check EEPROM usage
- improve/check OTA update, security w/password and certificate
- naming conventions
- file name and solution/project update
- make barometer / altitude configurable
- nice startup splash screen
- power saving, in particular sleep modes w/PIN16, shut down and restart, at least minimize GY-87 power consumption, desolder voltage regulator, LED
- menu idle screen
- WLAN honeypot ;-)
- OTA progress bar/screen, and implement other OTA method
- PIN Code Abfrage hinzufügen
- in Initscreen/Screensaver gehen und mit Hochheben/Klick wieder anzeigen
- calibrate vbat 
- calibrate accel and gyro
- add tilt-compensating functions to compass inkl. tools and documentation
- cleanup of accel and compass axis inkl. documentation

done - get Umlaute working
done - compass functions and draw routine
done - get NTP working, in particular check, that the time is fetched in the background, seems to be ok.
done - WLAN and Battery status 

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
___________________________________________________________
Crash:

___________________________________________________________
Menu:

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
	char	dec		hex			oct
	ä		132		0x84 \x84	\204
	Ä		142		0x8e \x8e	\216
	ö		148		0x94 \x94	\224
	Ö		153		0x99 \x99	\231
	ü		129		0x81 \x81	\201
	Ü		154		0x9a \x9a	\232
	°		9		0x09 \x09	\011		oder \260 (dec 176)

Kompass Kalibrierung
	1) x, y, z Werte ausgeben lassen, insb. den Kompass im ebenen Zustand hinlegen (Verbaut ist so, dass +x nach unten, und -z nach vorne geht, also +y nach rechts)
	2) den Kompass langsam einmal drehen
	3) über min und max für x und y den Mittelpunkt finden -> dies ergibt den Offset
	3) über Spanne y und z den skalieren auf Werte (nach Anwendung Offset) von -100..100 für x und y Achse -> dies ergibt Scale

	4) Mit echtem Kompass auf Norden ausrichten, Werte messen. 
		Hier:	Mag	194	0	-100 
	super Link: http://www.germersogorb.de/html/kalibrierung_des_hcm5883l.html 
	Total Field  48,378.8 nT -> 0.48378G
	Raw total field: 527
	