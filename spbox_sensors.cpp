//
//
//

#include "user_config.h"
#include "spbox_sensors.h"

#ifdef DEBUG_SENSORS
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

SPBOX_SENSORS sensors;

SPBOX_SENSORS::SPBOX_SENSORS()
{
}

bool SPBOX_SENSORS::initializeAccelGyro() {
	accelgyro_.setI2CMasterModeEnabled(false);
	accelgyro_.setI2CBypassEnabled(true);
	accelgyro_.setSleepEnabled(false);
	accelgyro_.initialize();
	//accelgyro_.setFullScaleAccelRange(gConfig.accel_range_scale);
	//accelgyro_.setFullScaleGyroRange(gConfig.gyro_range_scale);

	//do_update_accel_gyro_mag = false;		//TODO
	//sensors.changed_accel_gyro_mag = false;	// TODO

	DEBUGLOG("MPU6050: connection %s\r\n", (accelgyro_.testConnection() ? "successful" : "failed"));
}

bool SPBOX_SENSORS::initializeMag() {
	mag_.initialize();
	DEBUGLOG("HMC5883L: connection %s\r\n", (mag_.testConnection() ? "successful" : "failed"));
}

bool SPBOX_SENSORS::initializeBarometer() {
	barometer_.initialize();
	//sensors.changed_temperatur_pressure = false;	// TODO
	DEBUGLOG("BMP180: connection %s\r\n", (barometer_.testConnection() ? "successful" : "failed"));
}

//void update_accel_gyro_mag_cb(void *arg) {
//	do_update_accel_gyro_mag = true;
//}
//void setup_update_accel_gyro_mag_timer()
//{
//	os_timer_disarm(&timer_update_accel_gyro_mag);
//	os_timer_setfn(&timer_update_accel_gyro_mag, (os_timer_func_t *)update_accel_gyro_mag_cb, (void *)0);
//	os_timer_arm(&timer_update_accel_gyro_mag, DELAY_MS_10HZ, true);
//}

void SPBOX_SENSORS::fetchAccelGyro()
{
	accelgyro_.getMotion6(&ax_, &ay_, &az_, &gx_, &gy_, &gz_);
}

void SPBOX_SENSORS::calcAccelGyro()
{
	ax_f_ = (float)(ax_ - MPU6050_AXOFFSET) / MPU6050_A_GAIN;
	ay_f_ = (float)(ay_ - MPU6050_AYOFFSET) / MPU6050_A_GAIN;
	az_f_ = (float)(az_ - MPU6050_AZOFFSET) / MPU6050_A_GAIN;
	gx_f_ = (float)(gx_ - MPU6050_GXOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;	// rad/s
	gy_f_ = (float)(gy_ - MPU6050_GYOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;
	gz_f_ = (float)(gz_ - MPU6050_GZOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;
}

void SPBOX_SENSORS::resetMinMaxAccelGyro() {
	max_ax_f_ = min_ax_f_ = ax_f_;
	max_ay_f_ = min_ay_f_ = ay_f_;
	max_az_f_ = min_az_f_ = az_f_;
	max_gx_f_ = min_gx_f_ = gx_f_;
	max_gy_f_ = min_gy_f_ = gy_f_;
	max_gz_f_ = min_gz_f_ = gz_f_;
}

void SPBOX_SENSORS::updateMinMaxAccelGyro() {
	if (ax_f_ > max_ax_f_) max_ax_f_ = ax_f_;
	if (ay_f_ > max_ay_f_) max_ay_f_ = ay_f_;
	if (az_f_ > max_az_f_) max_az_f_ = az_f_;
	if (ax_f_ < min_ax_f_) min_ax_f_ = ax_f_;
	if (ay_f_ < min_ay_f_) min_ay_f_ = ay_f_;
	if (az_f_ < min_az_f_) min_az_f_ = az_f_;
	if (gx_f_ > max_gx_f_) max_gx_f_ = gx_f_;
	if (gy_f_ > max_gy_f_) max_gy_f_ = gy_f_;
	if (gz_f_ > max_gz_f_) max_gz_f_ = gz_f_;
	if (gx_f_ < min_gx_f_) min_gx_f_ = gx_f_;
	if (gy_f_ < min_gy_f_) min_gy_f_ = gy_f_;
	if (gz_f_ < min_gz_f_) min_gz_f_ = gz_f_;
}

void SPBOX_SENSORS::fetchMag()
{
	mag_.getHeading(&mx_, &my_, &mz_);
}

void SPBOX_SENSORS::calcMag()
{
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(mz_, my_);
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180.0 / M_PI;
	heading = heading;
}

void SPBOX_SENSORS::calcAltitude()
{
	// calculate absolute altitude in meters based on known pressure
	// (may pass a second "sea level pressure" parameter here,
	// otherwise uses the standard value of 101325 Pa)
	altitude_ = barometer_.getAltitude(pressure_);
}

void SPBOX_SENSORS::getAccel(int16_t * ax, int16_t * ay, int16_t * az) { *ax = ax_; *ay = ay_; *az = az_; }
void SPBOX_SENSORS::getAccel(float * ax_f, float * ay_f, float * az_f) { *ax_f = ax_f_; *ay_f = ay_f_; *az_f = az_f_; }
void SPBOX_SENSORS::getMaxAccel(float * max_ax, float * max_ay, float * max_az) { *max_ax = max_ax_f_; *max_ay = max_ay_f_; *max_az = max_az_f_; }
void SPBOX_SENSORS::getMinAccel(float * min_ax, float * min_ay, float * min_az) { *min_ax = min_ax_f_; *min_ay = min_ay_f_; *min_az = min_az_f_; }

void SPBOX_SENSORS::getGyro(int16_t * gx, int16_t * gy, int16_t * gz) { *gx = gx_; *gy = gy_; *gz = gz_; }
void SPBOX_SENSORS::getGyro(float * gx_f, float * gy_f, float * gz_f) { *gx_f = gx_f_; *gy_f = gy_f_; *gz_f = gz_f_; }
void SPBOX_SENSORS::getMaxGyro(float * max_gx, float * max_gy, float * max_gz) { *max_gx = max_gx_f_; *max_gy = max_gy_f_; *max_gz = max_gz_f_; }
void SPBOX_SENSORS::getMinGyro(float * min_gx, float * min_gy, float * min_gz) { *min_gx = min_gx_f_; *min_gy = min_gy_f_; *min_gz = min_gz_f_; }

void SPBOX_SENSORS::getMag(int16_t * mx, int16_t * my, int16_t * mz) { *mx = mx_; *my = my_; *mz = mz_; }
void SPBOX_SENSORS::getHeading(float * heading) { *heading = heading_; }

void SPBOX_SENSORS::getTemperature(float * temperature) { *temperature = temperature_; }
void SPBOX_SENSORS::getPressure(float * pressure) { *pressure = pressure_; }
void SPBOX_SENSORS::getAltitude(float * altitude) { *altitude = altitude_; }

void SPBOX_SENSORS::updateVBat() {
	uint8_t vbatADC;
	float vbatFloat = 0.0F;
	float vbatLSB = 0.97751F;		// 1000mV/1023 -> mV per LSB
	float vbatVoltDiv = 0.201321;	// 271K/1271K resistor voltage divider

	vbatADC = analogRead(VBAT_PIN);
	vbatFloat = ((float)vbatADC * vbatLSB) / vbatVoltDiv;
	vbatFloat /= 1000.;

	vbatFloat_ = vbatFloat;
}

float SPBOX_SENSORS::getVBat()
{
	return vbatFloat_;
}

void check_sensor_updates()
{
	//if (do_update_accel_gyro_mag) {
	//	do_update_accel_gyro_mag = false;
	//	get_accelgyro();
	//	get_mag();
	//	changed_accel_gyro_mag = true;
	//}
}

void check_sensor_calc()
{
	//if (changed_accel_gyro_mag) {
	//	changed_accel_gyro_mag = false;
	//	calc_accelgyro();
	//	if (true)		// AAARGH TODO
	//		update_min_max_accelgyro();
	//	calc_mag();
	//}
	//if (changed_temperatur_pressure) {
	//	changed_temperatur_pressure = false;
	//	calc_altitude();
	//}
}