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

# spRel

In this project several libraries and tools will be used which will be 
referenced here:

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


The SONOFF board is configured as follows: 

PIN assignment
==============
(info taken from github.com/tzapu/SonoffBoilerplate)
Board configuration:
1MB flash sizee

GPIO
#0		Button
#12		Relay
#13		LED G, active low
#14		PIN 5 (on SONOFF header)

SONOFF header
1 - VCC 3v3
2 - RX
3 - TX
4 - GND
5 - GPIO 14
