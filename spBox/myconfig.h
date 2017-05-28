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

#define STRING_VERSION "v0.3"

// WLAN
//#define WLAN_SSID		"..."
//#define WLAN_PASSWORD	"..."

// ADAFRUIT IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//#define AIO_USERNAME  "..."
//#define AIO_KEY		"..."
#define AIO_ENABLED		1

// Measure battery
#define VBAT_PIN		A0

// Rotary Encoder and switch
#define ENCODER_PIN_A	12
#define ENCODER_PIN_B	14
#define ENCODER_SW		13

// Rotary Encoder knob LEDs
#define LED_R			0		// rot enc led red (and huzzah led red)
#define LED_G			2		// rot enc led green (and huzzah led blue)

// Threshold
#define THRESHOLD		7		// rot enc and button debounce threshold (milliseconds)

// Timer delay constants in milliseconds(MS)
#define DELAY_MS_1HZ	1000
#define DELAY_MS_2HZ	500
#define DELAY_MS_5HZ	200
#define DELAY_MS_10HZ	100
#define DELAY_MS_20HZ	50
#define DELAY_MS_40HZ	25
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000

// MPU6050 accel offsets, referencing the sensor naming (not the adjusted NED namings for orientation reference model)
#define MPU6050_AXOFFSET	0
#define MPU6050_AYOFFSET	0
#define MPU6050_AZOFFSET	0
#define MPU6050_A_GAIN		2048		// MPU6050_ACCEL_FS_16

// MPU6050 gyro offsets, referencing the sensor naming (not the adjusted NED namings for orientation reference model)
#define MPU6050_GXOFFSET	0
#define MPU6050_GYOFFSET	0
#define MPU6050_GZOFFSET	0
#define MPU6050_G_GAIN		(16.4)		// MPU6050_GYRO_FS_2000

// HMC5883L - calibration matrix and offset
#define MAG_X 0
#define MAG_Y 1
#define MAG_Z 2
// calibration values are in spbox_sensors.cpp

// constants to convert deg - rad
#define MPU6050_DEG_RAD_CONV		0.01745329251994329576	// CONST
#define MPU6050_GAIN_DEG_RAD_CONV	0.00106422515365507901	// MPU6050_DEG_RAD_CONV / MPU6050_G_GAIN

// Output and debug
#define DBG_PORT Serial
#define	SERIAL_STATUS_OUTPUT

//#define DEBUG_BUTTON
//#define DEBUG_COM
//#define DEBUG_CONF
//#define DEBUG_DISPLAY
//#define DEBUG_LCDML
//#define DEBUG_ROTENC
//#define DEBUG_SENSORS
//#define DEBUG_SPBOX

typedef enum OTAMode {
	OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2,
} OTAModes_t;

typedef enum inputAltiModes {
	BUTTON_ALTITUDE = 0,
	INPUT_ALTITUDE,
	BUTTON_OK,
	BUTTON_CANCEL,
	LAST
} inputAltiModes_t;

typedef enum compassModes {
	ROSE = 0,
	DIRECTION,
	BAND,
} compassModes_t;

// Gimp exported as .png, converter used http://javl.github.io/image2cpp/
// 'splash'@128x32
const unsigned char mySplash[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfc, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x1e, 0x07, 0x80, 0x00, 0x00, 0x00, 0x06, 0x03, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x38, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xe0, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 0x00, 0x00, 0x00,
0x3f, 0x80, 0x1f, 0x80, 0xe1, 0xec, 0x60, 0x3f, 0x06, 0x7e, 0x06, 0x01, 0x80, 0x7c, 0x0e, 0x03,
0x7f, 0xe0, 0x7f, 0xc0, 0xc3, 0xfc, 0x60, 0xff, 0x87, 0xff, 0x06, 0x01, 0x01, 0xff, 0x07, 0x07,
0x60, 0x70, 0xe0, 0xe1, 0xc7, 0x0c, 0x31, 0xc1, 0x87, 0x83, 0x86, 0x07, 0x03, 0x83, 0x83, 0x06,
0x00, 0x30, 0xc0, 0x61, 0x8e, 0x0c, 0x31, 0x80, 0x06, 0x01, 0x87, 0xfe, 0x03, 0x01, 0x81, 0x8c,
0x00, 0x31, 0x80, 0x31, 0x8c, 0x0c, 0x31, 0x80, 0x06, 0x00, 0xc7, 0xff, 0x86, 0x00, 0xc1, 0xdc,
0x07, 0xf1, 0x80, 0x31, 0x8c, 0x0c, 0x31, 0xe0, 0x06, 0x00, 0xc6, 0x01, 0xc6, 0x00, 0xc0, 0xf8,
0x3f, 0xf1, 0xff, 0xf1, 0x8c, 0x0c, 0x30, 0xfe, 0x06, 0x00, 0xc6, 0x00, 0xe6, 0x00, 0xc0, 0x70,
0x78, 0x31, 0xff, 0xf1, 0x8c, 0x0c, 0x30, 0x3f, 0x86, 0x00, 0xc6, 0x00, 0x66, 0x00, 0xc0, 0x70,
0xe0, 0x31, 0x80, 0x01, 0x86, 0x1c, 0x60, 0x01, 0xc6, 0x00, 0xc6, 0x00, 0x66, 0x00, 0xc0, 0xf8,
0xc0, 0x31, 0x80, 0x00, 0xc7, 0xff, 0xe0, 0x00, 0xc6, 0x01, 0xc6, 0x00, 0x66, 0x00, 0xc1, 0xdc,
0xc0, 0x30, 0xc0, 0x10, 0xc3, 0xef, 0xc1, 0x00, 0xc6, 0x01, 0x86, 0x00, 0xe3, 0x01, 0x81, 0x8c,
0xe0, 0xf0, 0xf0, 0x70, 0x60, 0x00, 0x01, 0xc1, 0xc7, 0x07, 0x06, 0x01, 0xc3, 0x83, 0x83, 0x06,
0x7f, 0xf0, 0x7f, 0xf0, 0x38, 0x00, 0x01, 0xff, 0x87, 0xfe, 0x07, 0xff, 0x81, 0xff, 0x07, 0x07,
0x3f, 0x30, 0x1f, 0xc0, 0x1e, 0x00, 0x00, 0x7e, 0x06, 0xfc, 0x07, 0xfe, 0x00, 0x7c, 0x0e, 0x03,
0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x03, 0xfc, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// 'mountains'@32x32, v2
const unsigned char myMountains[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00,
0x00, 0x02, 0x80, 0x00, 0x00, 0x02, 0x81, 0x00, 0x00, 0x04, 0x42, 0x80, 0x00, 0x04, 0x22, 0x80,
0x00, 0x08, 0x14, 0x40, 0x00, 0x08, 0x14, 0x20, 0x00, 0x10, 0x08, 0x10, 0x00, 0x10, 0x08, 0x10,
0x00, 0x20, 0x10, 0x08, 0x00, 0x20, 0x10, 0x04, 0x0c, 0x40, 0x20, 0x02, 0x0b, 0x40, 0x20, 0x02,
0x10, 0x80, 0x40, 0x01, 0x10, 0x80, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00,
0x22, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

// 'force'
const unsigned char myForce[] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x03, 0x80,
0x00, 0x00, 0x0d, 0xa0, 0x00, 0x00, 0x30, 0x80, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x03, 0x00, 0x08,
0x00, 0x0c, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00,
0x0c, 0x00, 0x00, 0x04, 0x30, 0x00, 0x00, 0x06, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
0x40, 0x00, 0x00, 0x06, 0x20, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00,
0x10, 0x00, 0x00, 0x80, 0x08, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00,
0x04, 0x00, 0x80, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x84, 0x00, 0x00, 0x03, 0xa0, 0x00, 0x00,
0x07, 0x80, 0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
