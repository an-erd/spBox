#ifndef USER_CONFIG_H
#define USER_CONFIG_H

//#include "missing_dec.h"

#define STRING_VERSION "0.0.1"

// WLAN
#define WLAN_SSID		"W12"
#define WLAN_PASSWORD	"EYo6Hv4qRO7P1JSpAqZCH6vGVPHwznRWODIIIdhd1pBkeWCYie0knb1pOQ9t2cc"

// ADAFRUIT IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "andreaserd"
#define AIO_KEY			"ee3974dd87d3450490aa2840667e8162"
#define AIO_ENABLED		1

// Measure battery
#define VBAT_ENABLED	1
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
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000

// MPU6050 accel offsets
#define MPU6050_AXOFFSET	0
#define MPU6050_AYOFFSET	0
#define MPU6050_AZOFFSET	0
#define MPU6050_A_GAIN		2048		// MPU6050_ACCEL_FS_16

// MPU6050 gyro offsets
#define MPU6050_GXOFFSET	0
#define MPU6050_GYOFFSET	0
#define MPU6050_GZOFFSET	0
#define MPU6050_G_GAIN		(16.4)		// MPU6050_GYRO_FS_2000

// constants to convert deg - rad
#define MPU6050_DEG_RAD_CONV		0.01745329251994329576	// CONST
#define MPU6050_GAIN_DEG_RAD_CONV	0.00106422515365507901	// MPU6050_DEG_RAD_CONV / MPU6050_G_GAIN

typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;

// Output and debug
#define	SERIAL_STATUS_OUTPUT
#undef	MEASURE_PREFORMANCE

#define IPRINT_LEVEL INFO
enum debug_level
{
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

//#define iprintf(dlevel, ...) if (dlevel >= IPRINT_LEVEL) ets_printf(__VA_ARGS__)
//
//#if (SSD1306_LCDHEIGHT != 32)
//#error("Height incorrect, please fix Adafruit_SSD1306.h!");
//#endif

#endif //USER_CONFIG_H