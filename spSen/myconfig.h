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

#pragma once
#include <stdint.h>
#include <ESP8266WiFiMulti.h>
#include "credentials.h"

#define STRING_VERSION "v0.3"

// own Mosquitto server
#define MQTT_SERVER_W12		"192.168.2.137"
#define MQTT_SERVERPORT_W12	1883
//#define MQTT_USERNAME_W12	"..."
//#define MQTT_KEY_W12			"..."

// ADAFRUIT IO
#define MQTT_SERVER_AIO      "io.adafruit.com"
#define MQTT_SERVERPORT_AIO  1883
//#define MQTT_USERNAME_AIO  "..."
//#define MQTT_KEY_AIO		"..."

#define PIN_BUTTON		13			// spBox
#define PIN_LED_G		2			// spBox
#define PIN_RELAIS		0			// spBox, LED R			

// #define PIN_BUTTON		0		// spRel
//#define PIN_RELAIS		12			// spRel
// #define PIN_LED_G		13		// spRel
#define PIN_INPUT		14			// spRel

// Threshold
#define THRESHOLD					7		// rot enc and button debounce threshold (milliseconds)
#define RESET_TIMER					3000
#define MQTT_CONNECT_INTERVALL		3000
#define MQTT_HEALTHDATA_INTERVALL	10000
#define WLAN_CONNECT_INTERVALL		500
#define WLAN_CHECK_INTERVALL		10000

// Timer delay constants in milliseconds(MS)
#define DELAY_MS_1HZ	1000
#define DELAY_MS_2HZ	500
#define DELAY_MS_5HZ	200
#define DELAY_MS_10HZ	100
#define DELAY_MS_20HZ	50
#define DELAY_MS_40HZ	25
#define DELAY_MS_100HZ	10
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000


// Output and debug
#define DBG_PORT Serial
#define	SERIAL_STATUS_OUTPUT
#define DEBUG_SPREL

//#define DEBUG_BUTTON
//#define DEBUG_COM
//#define DEBUG_CONF
//#define DEBUG_DISPLAY
//#define DEBUG_LCDML
//#define DEBUG_ROTENC
//#define DEBUG_SENSORS
//#define DEBUG_SPBOX
