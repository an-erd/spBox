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
#include "credentials.h"

// WLAN
//#define WLAN_SSID		"..."
//#define WLAN_PASSWORD	"..."

// own Mosquitto server
#define W12_SERVER		"192.168.2.137"
#define W12_SERVERPORT	1883
//#define W12_USERNAME	"..."
//#define W12_KEY			"..."

// ADAFRUIT IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//#define AIO_USERNAME  "..."
//#define AIO_KEY		"..."

typedef struct {
	char *name;
	int port;
	char *uid;
	char *key;
} mqttConfig_t;

#define NUM_MQTT_CONFIG	2
const mqttConfig_t mqttConfigs[NUM_MQTT_CONFIG] = {
	{W12_SERVER, W12_SERVERPORT, W12_USERNAME, W12_KEY},
	{AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY},
	{"n/a", 0, "n/a", "n/a"},
};

// Measure battery
#define VBAT_PIN		A0

// Timer delay constants in milliseconds(MS)
#define DELAY_MS_1HZ	1000
#define DELAY_MS_2HZ	500
#define DELAY_MS_5HZ	200
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000

// typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;

// Output and debug
#define DBG_PORT Serial
#define DEBUG_COM
#define DEBUG_CONF
#define	SERIAL_STATUS_OUTPUT

typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;
