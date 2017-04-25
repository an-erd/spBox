// spbox_sensors.h

#ifndef _SPBOX_SENSORS_h
#define _SPBOX_SENSORS_h

#include "arduino.h"
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include "BMP085_nb.h"

#endif

typedef struct
{
	int16_t		ax, ay, az;			// accel values (sensor)
	float		ax_f, ay_f, az_f;	// accel float values (calculated)
	float		max_ax_f, max_ay_f, max_az_f, min_ax_f, min_ay_f, min_az_f;

	int16_t		gx, gy, gz;			// gyro values (sensor)
	float		gx_f, gy_f, gz_f;	// gyro float values (calculated)
	float		max_gx_f, max_gy_f, max_gz_f, min_gx_f, min_gy_f, min_gz_f;

	int16_t		mx, my, mz;			// magnetometer values (sensor)
	float		heading;			// calculated heading (calculated)

	float		temperature;		// temperature (sensor)
	float		pressure;			// pressure (sensor)
	float		altitude;			// altitude (sensor)

	int8_t		update_temperature_pressure_step;
	bool		changed_accel_gyro_mag;			// -> re-calculate
	bool		changed_temperatur_pressure;	// -> re-calculate
} sGlobalSensors;

int		g_vbatADC;	// TODO global variable
volatile bool	do_update_accel_gyro_mag;
// MPU6050 sensor board
MPU6050					accelgyro;
HMC5883L				mag;

// Timer
LOCAL os_timer_t timer_update_accel_gyro_mag;
LOCAL os_timer_t timer_update_mqtt;

extern SPBOX_SENSORS sensors;