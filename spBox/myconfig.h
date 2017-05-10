#ifndef USER_CONFIG_H
#define USER_CONFIG_H

#define STRING_VERSION "0.1.0"

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

typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;

#endif //USER_CONFIG_H
