#pragma once

#define VBAT_PIN		A0

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

#define DBG_PORT Serial
#define DEBUG_SENSORS
#define DEBUG_DISPLAY
#define	SERIAL_STATUS_OUTPUT

typedef enum imputAltiModes {
	BUTTON_ALTITUDE = -1,
	INPUT_ALTITUDE = 0,
	BUTTON_OK = 4,
	BUTTON_CANCEL,
	LAST
} inputAltiModes_t;