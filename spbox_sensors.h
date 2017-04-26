// spbox_sensors.h

#ifndef _SPBOX_SENSORS_h
#define _SPBOX_SENSORS_h

#include "arduino.h"
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include "BMP085_nb.h"

// Timer
LOCAL os_timer_t timerUpdateAccelGyroMag;

class SPBOX_SENSORS
{
public:
	SPBOX_SENSORS();

	bool initializeAccelGyro();
	bool initializeMag();
	bool initializeBarometer();

	void fetchAccelGyro();
	void calcAccelGyro();
	void resetMinMaxAccelGyro();
	void updateMinMaxAccelGyro();
	void fetchMag();
	void calcMag();
	void calcAltitude();

	void getAccel(int16_t *ax, int16_t *ay, int16_t *az);
	void getAccel(float *ax_f, float *ay_f, float *az_f);
	void getMaxAccel(float *max_ax, float *max_ay, float *max_az);
	void getMinAccel(float *max_ax, float *max_ay, float *max_az);
	void getGyro(int16_t *ax, int16_t *ay, int16_t *az);
	void getGyro(float *ax_f, float *ay_f, float *az_f);
	void getMaxGyro(float *max_gx, float *max_gy, float *max_gz);
	void getMinGyro(float *min_gx, float *min_gy, float *min_gz);
	void getMag(int16_t *mx_, int16_t *my, int16_t *mz);
	void getHeading(float *heading);
	void getTemperature(float *temperature);
	void getPressure(float *pressure);
	void getAltitude(float *altitude);
	void updateVBat();
	float getVBat();

private:
	MPU6050		accelgyro_;
	HMC5883L	mag_;
	BMP085_NB	barometer_;

	int16_t		ax_, ay_, az_;			// accel values (sensor)
	float		ax_f_, ay_f_, az_f_;	// accel float values (calculated)
	float		max_ax_f_, max_ay_f_, max_az_f_, min_ax_f_, min_ay_f_, min_az_f_;

	int16_t		gx_, gy_, gz_;			// gyro values (sensor)
	float		gx_f_, gy_f_, gz_f_;	// gyro float values (calculated)
	float		max_gx_f_, max_gy_f_, max_gz_f_, min_gx_f_, min_gy_f_, min_gz_f_;

	int16_t		mx_, my_, mz_;			// magnetometer values (sensor)
	float		heading_;			// calculated heading (calculated)

	float		temperature_;		// temperature (sensor)
	float		pressure_;			// pressure (sensor)
	float		altitude_;			// altitude (sensor)

	float		vbatFloat_;

	int8_t		update_temperature_pressure_step_;
	bool		changed_accel_gyro_mag_;			// -> re-calculate
	bool		changed_temperatur_pressure_;	// -> re-calculate

	//volatile bool	do_update_accel_gyro_mag_;
protected:
	//void timerUpdateAccelGyroMagCB();
	//void timerUpdateTempPressCB();
	//void timerUpdateStepsCB();
};

extern SPBOX_SENSORS sensors;

#endif