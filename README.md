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
be used more (e.g. in time.h used from NTPClientLib)

LOCAL os_timer_t timerUpdateAccelGyroMag;	// read Accel, Gyro and Mag regularly
LOCAL os_timer_t timerUpdateTempPress;		// prepare temperature and pressure regularly
LOCAL os_timer_t timerUpdateSteps;			// handle different BMP085 preparation duration for temperature and pressure
LOCAL os_timer_t timerUpdateMqtt;			// pusblish health data to IO

Power consumption
=================
not optimized yet. Device should power off or put itself in sleep mode after 
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
	abs |	178		0xb2	\262
	dreickhoch		0x1e	\036
(Link: https://forums.adafruit.com/viewtopic.php?f=8&t=51999)

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
	
	
Splash screen:
==============
To create the splash screen, one way is to use Gimp, only 1bit (B/W), 
canvas size 128x32, export as .png, then use the following tool (use invert)
https://github.com/javl/image2cpp
Copy/paste to myconfig.h -> const unsigned char mySplash[] = {...};
Another way is to save in Gimp as .h, but then use display.drawXBitmap() (with
reversed bit order left to right is  LSB..MSB). 

Calculate Voltage out of measured digital PIN read
==================================================
- Maximum value masured 941 (0..1023), which is 4,22 V
- mV per LSB from 1000 mV/1023 gives V/LSB = 0.97751


Useful stuff on stack traces and exceptions
===========================================
https://github.com/esp8266/Arduino/blob/master/doc/faq/a02-my-esp-crashes.rst


/// stack trace 19.06.2017 - I
Decoding 51 results
0x40106402: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 128
0x40100ec2: pp_post at ?? line ?
0x401033c3: lmacRecycleMPDU at ?? line ?
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x4024e130: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x4021c4a2: _svfprintf_r at /Users/igrokhotkov/e/newlib-xtensa/xtensa-lx106-elf/newlib/libc/stdio/../../../.././newlib/libc/stdio/nano-vfprintf.c line 505
0x4021a095: _vsnprintf_r at /Users/igrokhotkov/e/newlib-xtensa/xtensa-lx106-elf/newlib/libc/stdio/../../../.././newlib/libc/stdio/vsnprintf.c line 73
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x402104b5: ROTENC::isrInt1() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/rotenc.cpp line 67
0x40106376: millis at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring.c line 67
0x40216e03: std::_Function_handler    (ROTENC*)> >::_M_invoke(std::_Any_data const&) at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/rotenc.cpp line 67
0x40214752: interruptFunctional(void*) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/FunctionalInterrupt.cpp line 18
0x401052a3: flash_gd25q32c_read_status at ?? line ?
0x4024e130: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x40105514: spi_flash_erase_sector at ?? line ?
0x40106788: pvPortZalloc at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/heap.c line 68
0x402411f8: wifi_param_save_protect_with_check at ?? line ?
0x402411e1: wifi_param_save_protect_with_check at ?? line ?
0x402412cb: system_param_save_with_protect at ?? line ?
0x402412ae: system_param_save_with_protect at ?? line ?
0x402415fd: wifi_station_ap_number_set at ?? line ?
0x402415b0: wifi_station_ap_number_set at ?? line ?
0x40207a68: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x402079a3: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x4020d7b4: EEPROMClass::commit() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\EEPROM/EEPROM.cpp line 132
0x402172f0: Print::write(unsigned char const*, unsigned int) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 38
0x40214bd9: Print::write(char const*) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 211
0x40240f1a: wifi_get_opmode at ?? line ?
0x4024203b: wifi_station_get_connect_status at ?? line ?
0x40214d8c: Print::println() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 211
0x4021e21c: __ieee754_atan2 at /Users/igrokhotkov/e/newlib-xtensa/xtensa-lx106-elf/newlib/libm/math/../../../.././newlib/libm/math/e_atan2.c line 116
0x40103aae: lmacProcessCollision at ?? line ?
0x4021438c: esp_yield at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_main.cpp line 57
0x40207b08: ESP8266WiFiSTAClass::begin(char*, char*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x4020738f: ESP8266WiFiMulti::run(WifiAPProfile_t) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiMulti.cpp line 124
0x4020fd9d: TwoWire::available() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\Wire/Wire.cpp line 252
0x402110d7: SPBOX_COM::enableWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spbox_com.cpp line 234
0x40211159: SPBOX_COM::checkWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spbox_com.cpp line 234
0x40205176: loop at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spBox.cpp line 1422
0x402143d8: loop_wrapper at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_main.cpp line 57
0x4010070c: cont_norm at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/cont.S line 109

/// stack trace 19.06.2017 - II

Exception (0):
epc1=0x40214740 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000

ctx: cont
sp: 3fff2120 end: 3fff26a0 offset: 01a0

>>>stack>>>
3fff22c0:  40106402 3fffc6fc 00000001 00000000
3fff22d0:  e0030035 ffffffff 00000020 401064fa
3fff22e0:  00000000 00000000 0000001f 00000022
3fff22f0:  3fffc200 401063c8 3fffc258 4000050c
3fff2300:  400043df 00000030 00000016 ffffffff
3fff2310:  400044ab 3fffc718 3fff2400 08000000
3fff2320:  60000200 08000000 00000003 00000000
3fff2330:  0000ffff 00042035 00002035 003fd000
3fff2340:  4024e130 00000494 003fd000 00000030
3fff2350:  e0035025 00000000 0000001f 00000022
3fff2360:  3fffc200 401063c8 3fffc258 00000022
3fff2370:  3fffc200 401063c8 3fffc258 4000050c
3fff2380:  402015a0 00000030 0000001c ffffffff
3fff2390:  402016ce 00000013 0000000a 60000318
3fff23a0:  e0035025 000002b2 3fff0e90 401064fa
3fff23b0:  000000f0 3fffc6fc 3fff0ef0 401064fa
3fff23c0:  3ffe8778 3fff0eb4 3fff0ef0 401064fa
3fff23d0:  00000000 00000000 3fff0ef0 40210466
3fff23e0:  00000000 4000444e 00000000 40216e03
3fff23f0:  00000002 401052a3 00000001 60000200
3fff2400:  00000002 4000410f 00001001 00000205
3fff2410:  3fffc718 40004a3c 000003fd 4024e130
3fff2420:  3fffc718 40105514 000003fd 40106788
3fff2430:  000003fd 402411f8 3fffc258 402411e1
3fff2440:  3fff4814 402412cb 3fff437c 00000494
3fff2450:  000003fd 00000004 3fff437c 402412ae
3fff2460:  ffffff00 55aa55aa 0000001c 00000020
3fff2470:  00000020 00000069 00000053 aa55aa55
3fff2480:  000003ff 402415fd 00000002 3fff437c
3fff2490:  3fff25e4 bf85c9cf 6583a200 402415b0
3fff24a0:  40207a68 3fff3e44 3fff353c 402079a3
3fff24b0:  40000500 000000f8 feefeffe feefeffe
3fff24c0:  4020d7b4 00000001 3ffe9edd 402172f0
3fff24d0:  3fff1600 00000014 3ffe9490 00000000
3fff24e0:  3fff26e8 00000015 3fff26e8 40214bd9
3fff24f0:  40240f1a 3fff0f3c 3fff0c80 00000001
3fff2500:  4024203b 00000001 3fff26e8 40214d8c
3fff2510:  6b08ec00 573aae8a 00003231 002e0000
3fff2520:  00000226 00000020 3fff1288 3fff166c
3fff2530:  00000010 45ff25d0 48366f59 52713476
3fff2540:  3150374f 4170534a 48435a71 56477636
3fff2550:  7a774850 4f57526e 49494944 31646864
3fff2560:  656b4270 69594357 6e6b3065 4f703162
3fff2570:  32743951 01006363 8a6b08ec 40103aae
3fff2580:  00000001 3fff0c80 00000001 3fff0c80
3fff2590:  3fff353c 3fff3e44 4021438c 3fff3e44
3fff25a0:  3fff353c 3fff0c80 00000000 40207b08
3fff25b0:  3fff0c80 3fff0c80 00000000 4020738f
3fff25c0:  3fff25dc 3fff25f4 00000000 3fff1288
3fff25d0:  00000000 00000000 00000000 00000006
3fff25e0:  000003e8 8a6b08ec 3fff3aae 3fff444c
3fff25f0:  ffffffb3 3fff0700 00000008 40205e81
3fff2600:  3fff353c ffffffb9 3fff3e44 3fff0f3c
3fff2610:  00000000 00000001 00000004 00000001
3fff2620:  3fff25d0 00000000 3fff2600 3fff2630
3fff2630:  bd380000 3f9a1000 3fff0ef0 3fff138c
3fff2640:  3fffdad0 3fff26e8 3fff0f48 402110d7
3fff2650:  3fff307c 3fff10c0 3fff0f3a 40211159
3fff2660:  3fffdad0 3fff1288 3fff0f48 40205176
3fff2670:  00000000 00000000 00000001 3fff1678
3fff2680:  3fffdad0 00000000 3fff1670 402143d8
3fff2690:  feefeffe feefeffe 3fff1680 4010070c
<<<stack<<<

ets Jan  8 2013,rst cause:2, boot mode:(3,7)

load 0x4010f000, len 1384, room 16
tail 8
chksum 0x2d
csum 0x2d
v00000000
~ld


Decoding 49 results
0x40106402: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 128
0x401064fa: __digitalRead at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 95
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x4024e130: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x401063c8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x402015a0: twi_delay at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_si2c.c line 81
0x402016ce: twi_write_bit at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_si2c.c line 81
0x401064fa: __digitalRead at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 95
0x401064fa: __digitalRead at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 95
0x401064fa: __digitalRead at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 95
0x40210466: ROTENC::isrInt0() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/rotenc.cpp line 67
0x40216e03: std::_Function_handler    (ROTENC*)> >::_M_invoke(std::_Any_data const&) at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/rotenc.cpp line 67
0x401052a3: flash_gd25q32c_read_status at ?? line ?
0x4024e130: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x40105514: spi_flash_erase_sector at ?? line ?
0x40106788: pvPortZalloc at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/heap.c line 68
0x402411f8: wifi_param_save_protect_with_check at ?? line ?
0x402411e1: wifi_param_save_protect_with_check at ?? line ?
0x402412cb: system_param_save_with_protect at ?? line ?
0x402412ae: system_param_save_with_protect at ?? line ?
0x402415fd: wifi_station_ap_number_set at ?? line ?
0x402415b0: wifi_station_ap_number_set at ?? line ?
0x40207a68: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x402079a3: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x4020d7b4: EEPROMClass::commit() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\EEPROM/EEPROM.cpp line 132
0x402172f0: Print::write(unsigned char const*, unsigned int) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 38
0x40214bd9: Print::write(char const*) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 211
0x40240f1a: wifi_get_opmode at ?? line ?
0x4024203b: wifi_station_get_connect_status at ?? line ?
0x40214d8c: Print::println() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 211
0x40103aae: lmacProcessCollision at ?? line ?
0x4021438c: esp_yield at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_main.cpp line 57
0x40207b08: ESP8266WiFiSTAClass::begin(char*, char*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x4020738f: ESP8266WiFiMulti::run(WifiAPProfile_t) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiMulti.cpp line 124
0x40205e81: LCDMenuLib::doScroll() at C:\Users\AKAEM\Documents\Arduino\libraries\LCDMenuLib\src/LCDMenuLib.cpp line 479
0x402110d7: SPBOX_COM::enableWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spbox_com.cpp line 234
0x40211159: SPBOX_COM::checkWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spbox_com.cpp line 234
0x40205176: loop at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Debug/spBox.cpp line 1422
0x402143d8: loop_wrapper at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_main.cpp line 57
0x4010070c: cont_norm at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/cont.S line 109

26.06.17, another stack trace
=============================
[WIFI] 00055974: Connecting BSSID: EC:08:6B:8A:AE:3A SSID: W12 Channal: 1 (-68)

Exception (0):
epc1=0x40214624 epc2=0x00000000 epc3=0x00000000 excvaddr=0x00000000 depc=0x00000000

ctx: cont 
sp: 3fff1ec0 end: 3fff2450 offset: 01a0

>>>stack>>>
3fff2060:  40106556 40100823 00000000 3ffeed50  
3fff2070:  20017035 00000000 00000002 40100ec2  
3fff2080:  00000000 00000000 0000001f 00000022  
3fff2090:  3fffc200 4010651c 3fffc258 4000050c  
3fff20a0:  400043e6 00000030 00000016 ffffffff  
3fff20b0:  400044ab 3fffc718 3fff21a0 08000000  
3fff20c0:  60000200 08000000 08000000 00000000  
3fff20d0:  0000ffff 00042035 00002035 003fe000  
3fff20e0:  4024dfe0 00000494 003fe000 00000030  
3fff20f0:  00000000 3fff21b1 3fff2120 401049ed  
3fff2100:  4000050c 4010651c 3fffc258 4010664e  
3fff2110:  4021c0fc 3fff2210 3fff0c70 40106343  
3fff2120:  00000075 3fff122c 00000015 60000318  
3fff2130:  3ffe82f8 00000006 00000000 40216c47  
3fff2140:  3fff21b3 3ffe9591 80000000 40214636  
3fff2150:  401065c1 3ffe8778 60000314 401065a8  
3fff2160:  00000002 4000444e 3fff2210 4021c3a4  
3fff2170:  00000000 4000422e 60000200 3fff2220  
3fff2180:  00000000 4000444e 00000100 003ff000  
3fff2190:  00000002 401052a3 00000001 60000200  
3fff21a0:  00000002 4000410f 00001001 00000205  
3fff21b0:  3fffc718 40004a3c 000003fe 4024dfe0  
3fff21c0:  3fffc718 40105514 000003fe 401068dc  
3fff21d0:  000003fe 402410a0 000003ff 40241089  
3fff21e0:  3fff475c 40241173 3fff42c4 00000494  
3fff21f0:  000003fd 00000004 3fff42c4 40241156  
3fff2200:  ffffff01 55aa55aa 0000000f 00000020  
3fff2210:  00000020 00000033 00000033 aa55aa55  
3fff2220:  000003ff 402414a5 00000001 3fff42c4  
3fff2230:  3fff238c bc2c58a4 00000000 40241458  
3fff2240:  40207b6d 3fff3cac 3fff33a4 40207aa3  
3fff2250:  00000000 00000000 00000001 402101bc  
3fff2260:  3fff1130 3fff0ad0 00000000 402100e9  
3fff2270:  3fff1100 3fff0ad0 3fff0c18 4020f428  
3fff2280:  000003e8 00000068 0000ab1d 0000003a  
3fff2290:  3fff23e0 3fff23e0 3fff102c ffffffb3  
3fff22a0:  40240dc2 3fff0ad0 3fff0ce4 00000001  
3fff22b0:  6b08ec00 573aae8a 3f003231 40214b3a  
3fff22c0:  4649575b 30205d49 35353030 3a343739  
3fff22d0:  6e6f4320 4563656e 48366f59 52713476  
3fff22e0:  3150374f 4170534a 48435a71 56477636  
3fff22f0:  7a774850 4f57526e 49494944 31646864  
3fff2300:  656b4270 69594357 6e6b3065 4f703162  
3fff2310:  32743951 01006363 8a6b08ec 00003aae  
3fff2320:  00000001 3fff0ad0 00000001 3fff33a4  
3fff2330:  3ffe953c 00000051 3fff2310 00000000  
3fff2340:  3fff2498 3fff33a4 3fff0ad0 40207c08  
3fff2350:  3fff2498 3fff33a4 3fff0ad0 40207423  
3fff2360:  0000008a 000000ae 0000003a 3fff33a4  
3fff2370:  00000001 ffffffbc 0000002a 40210274  
3fff2380:  00000000 00000000 00000000 8a6b08ec  
3fff2390:  00003aae 00000001 3fff3264 ffffffbb  
3fff23a0:  3fff0400 3fff0ad0 3fff102c 0000001f  
3fff23b0:  ffffffbc 3fff3cac 3fff3cac 00000001  
3fff23c0:  3fff0cd8 00000000 00000002 00000001  
3fff23d0:  3fff2380 00000000 3fff049c bf246000  
3fff23e0:  bd6c0000 3f3d4000 bf246000 3fff1130  
3fff23f0:  3fffdad0 3fff0e58 3fff0ce4 4021103a  
3fff2400:  3fff087c 00000000 3fff0cd6 402110b1  
3fff2410:  3fffdad0 3fff102c 3fff0ce4 4020512a  
3fff2420:  00000000 00000000 00000001 3fff141c  
3fff2430:  3fffdad0 00000000 3fff1414 402142bc  
3fff2440:  feefeffe feefeffe 3fff1430 4010070c  
<<<stack<<<

 ets Jan  8 2013,rst cause:2, boot mode:(1,1)

 
Decoding 49 results
0x40106556: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 128
0x40100823: ppProcessTxQ at ?? line ?
0x40100ec2: pp_post at ?? line ?
0x4010651c: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x4024dfe0: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x401049ed: ets_timer_disarm at ?? line ?
0x4010651c: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 122
0x4010664e: __digitalRead at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 95
0x4021c0fc: __ssputs_r at /Users/igrokhotkov/e/newlib-xtensa/xtensa-lx106-elf/newlib/libc/stdio/../../../.././newlib/libc/stdio/nano-vfprintf.c line 180
0x40106343: BUTTON::isrInt0() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Release/button.cpp line 63
0x40216c47: std::_Function_handler    (BUTTON*)> >::_M_invoke(std::_Any_data const&) at c:\users\akaem\documents\arduino\hardware\esp8266com\esp8266\tools\xtensa-lx106-elf\xtensa-lx106-elf\include\c++\4.8.2/functional line 2073
0x40214636: interruptFunctional(void*) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/FunctionalInterrupt.cpp line 18
0x401065c1: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 151
0x401065a8: interrupt_handler at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_wiring_digital.c line 141
0x4021c3a4: _svfprintf_r at /Users/igrokhotkov/e/newlib-xtensa/xtensa-lx106-elf/newlib/libc/stdio/../../../.././newlib/libc/stdio/nano-vfprintf.c line 531
0x401052a3: flash_gd25q32c_read_status at ?? line ?
0x4024dfe0: sleep_reset_analog_rtcreg_8266 at ?? line ?
0x40105514: spi_flash_erase_sector at ?? line ?
0x401068dc: pvPortZalloc at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/heap.c line 68
0x402410a0: wifi_param_save_protect_with_check at ?? line ?
0x40241089: wifi_param_save_protect_with_check at ?? line ?
0x40241173: system_param_save_with_protect at ?? line ?
0x40241156: system_param_save_with_protect at ?? line ?
0x402414a5: wifi_station_ap_number_set at ?? line ?
0x40241458: wifi_station_ap_number_set at ?? line ?
0x40207b6d: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x40207aa3: ESP8266WiFiSTAClass::begin(char const*, char const*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x402101bc: TwoWire::requestFrom(unsigned char, unsigned int, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\Wire/Wire.cpp line 252
0x402100e9: TwoWire::available() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\Wire/Wire.cpp line 252
0x4020f428: I2Cdev::readBytes(unsigned char, unsigned char, unsigned char, unsigned char*, unsigned short) at C:\Users\AKAEM\Documents\Arduino\libraries\I2Cdev/I2Cdev.cpp line 148
0x40240dc2: wifi_get_opmode at ?? line ?
0x40214b3a: Print::printf(char const*, ...) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/Print.cpp line 211
0x40207c08: ESP8266WiFiSTAClass::begin(char*, char*, int, unsigned char const*, bool) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiSTA.cpp line 516
0x40207423: ESP8266WiFiMulti::run(WifiAPProfile_t) at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\ESP8266WiFi\src/ESP8266WiFiMulti.cpp line 124
0x40210274: TwoWire::endTransmission() at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\libraries\Wire/Wire.cpp line 252
0x4021103a: SPBOX_COM::enableWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Release/spbox_com.cpp line 234
0x402110b1: SPBOX_COM::checkWlan() at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Release/spbox_com.cpp line 234
0x4020512a: loop at C:\Users\AKAEM\AppData\Local\Temp\VMBuilds\spBox\esp8266com_huzzah\Release/spBox.cpp line 1419
0x402142bc: loop_wrapper at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/core_esp8266_main.cpp line 57
0x4010070c: cont_norm at C:\Users\AKAEM\Documents\Arduino\hardware\esp8266com\esp8266\cores\esp8266/cont.S line 109
