// spbox_sensors.h

#ifndef _SPBOX_SENSORS_h
#define _SPBOX_SENSORS_h

extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#include "arduino.h"
#include <I2Cdev.h>
#include <MPU6050.h>
#include <HMC5883L.h>
#include <BMP085.h>

typedef enum BMP085Steps {
	sensorPaused = 0,
	sensorReqTemp,
	sensorReadTempReqPress,
	sensorDone
} BMP085UpdateSteps_t;

typedef struct AccelGyroMagEvent {
	float		ax_f, ay_f, az_f;
	float		gx_f, gy_f, gz_f;
	float		heading;
} accelGyroMagEvent_t;

typedef struct minMaxAccelGyroEvent {
	float		max_ax_f, max_ay_f, max_az_f;
	float		min_ax_f, min_ay_f, min_az_f;
	float		max_gx_f, max_gy_f, max_gz_f;
	float		min_gx_f, min_gy_f, min_gz_f;
} minMaxAccelGyroEvent_t;

typedef struct TempPressAltiEvent {
	float		temperature;
	float		pressure;
	float		altitude;
} tempPressAltiEvent_t;

typedef std::function<void(accelGyroMagEvent_t)>  onAccelGyroMagEvent_t;
typedef std::function<void(minMaxAccelGyroEvent_t)> onMinMaxAccelGyroEvent_t;
typedef std::function<void(tempPressAltiEvent_t)> onTempPressAltiEvent_t;

class SPBOX_SENSORS
{
public:
	SPBOX_SENSORS();

	bool initializeAccelGyro();
	bool initializeMag();
	bool initializeBarometer();

	void setupUpdateAccelGyroMag();
	void startUpdateAccelGyroMag();
	void stopUpdateAccelGyroMag();
	void updateAccelGyroMagCB();
	bool checkAccelGyroMag();
	void fetchAccelGyro();
	void calcAccelGyro();
	void resetMinMaxAccelGyro();
	bool updateMinMaxAccelGyro();
	void fetchMag();
	void calcMag();
	void calcAltitude();

	void setupUpdateTempPress();
	void startUpdateTempPress();
	void stopUpdateTempPress();
	void updateTempPressCB();
	void prepTempPressure();
	bool checkTempPress();

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
	void getAccelGyroMag(accelGyroMagEvent_t *e);
	void getMinMaxAccelGyro(minMaxAccelGyroEvent_t *e);
	void getTemperature(float *temperature);
	void getPressure(float *pressure);
	void getAltitude(float *altitude);
	void updateVBat();
	float getVBat();

	void		onAccelGyroMagEvent(onAccelGyroMagEvent_t handler);
	void		onMinMaxAccelGyroEvent(onMinMaxAccelGyroEvent_t handler);
	void		onTempPressAltiEvent(onTempPressAltiEvent_t    handler);

private:
	MPU6050		accelgyro_;
	HMC5883L	mag_;
	BMP085		barometer_;

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

	volatile	BMP085UpdateSteps_t _updateStep;
	volatile	bool do_update_accel_gyro_mag_;
protected:
	onAccelGyroMagEvent_t	onChangeAccelGyroMagEvent;
	onTempPressAltiEvent_t	onChangeTempPressAltiEvent;
	onMinMaxAccelGyroEvent_t onChangeMinMaxAccelGyroEvent;
	//void timerUpdateAccelGyroMagCB();
	//void timerUpdateTempPressCB();
	//void timerUpdateStepsCB();
};

extern SPBOX_SENSORS sensors;

#endif