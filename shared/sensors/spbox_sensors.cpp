#include "myconfig.h"
#include "spbox_sensors.h"

#ifdef DEBUG_SENSORS
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

// magnometer calibration values
double cal_matrix[3][3] = {
	{0.970506, 0.002044, 0.004219},
	{0.002044, 0.945654, -0.002506},
	{0.004219, -0.002506, 1.070759} };
double cal_offsets[3] = { -53.818994, 77.549821, 214.359594 };
double declination_angle = 0.04246890849;// in Rad, entspricht 2.43333 Rad

SPBOX_SENSORS sensors;

// Timer
LOCAL os_timer_t timerUpdateAccelGyroMag;	// read Accel, Gyro and Mag regularly
LOCAL os_timer_t timerUpdateTempPress;		// prepare temperature and pressure regularly
LOCAL os_timer_t timerUpdateSteps;			// handle different BMP085 preparation duration for temperature and pressure
void updateAccelGyroMag_CB(void *arg) { sensors.updateAccelGyroMagCB(); }
void updateTempPress_CB(void *arg) { sensors.updateTempPressCB(); }

SPBOX_SENSORS::SPBOX_SENSORS()
{
}

bool SPBOX_SENSORS::initializeAccelGyro(uint8_t accel_range, uint8_t gyro_range) {
	accelgyro_.setI2CMasterModeEnabled(false);
	accelgyro_.setI2CBypassEnabled(true);
	accelgyro_.setSleepEnabled(false);
	accelgyro_.initialize();
	setFullScaleAccelRange(accel_range);
	setFullScaleGyroRange(gyro_range);
	DEBUGLOG("MPU6050: connection %s\r\n", (accelgyro_.testConnection() ? "successful" : "failed"));
}
bool SPBOX_SENSORS::initializeMag() {
	mag_.initialize();
	DEBUGLOG("HMC5883L: connection %s\r\n", (mag_.testConnection() ? "successful" : "failed"));
}
bool SPBOX_SENSORS::initializeBarometer() {
	barometer_.initialize();
	DEBUGLOG("BMP180: connection %s\r\n", (barometer_.testConnection() ? "successful" : "failed"));
}

void SPBOX_SENSORS::initializeVBat()
{
	// ADC for battery measurement
	pinMode(VBAT_PIN, INPUT);
}

void SPBOX_SENSORS::setFullScaleAccelRange(uint8_t range)
{
	accelgyro_.setFullScaleAccelRange(range);
}

void SPBOX_SENSORS::setFullScaleGyroRange(uint8_t range)
{
	accelgyro_.setFullScaleGyroRange(range);
}

void SPBOX_SENSORS::setupUpdateAccelGyroMag()
{
	os_timer_disarm(&timerUpdateAccelGyroMag);
	os_timer_setfn(&timerUpdateAccelGyroMag, (os_timer_func_t *)updateAccelGyroMag_CB, (void *)0);
}

void SPBOX_SENSORS::startUpdateAccelGyroMag()
{
	os_timer_arm(&timerUpdateAccelGyroMag, DELAY_MS_10HZ, true);
}

void SPBOX_SENSORS::stopUpdateAccelGyroMag()
{
	os_timer_disarm(&timerUpdateAccelGyroMag);
}

void SPBOX_SENSORS::setupUpdateTempPress()
{
	os_timer_disarm(&timerUpdateTempPress);
	os_timer_setfn(&timerUpdateTempPress, (os_timer_func_t *)updateTempPress_CB, (void *)0);
}
void SPBOX_SENSORS::startUpdateTempPress()
{
	os_timer_disarm(&timerUpdateTempPress);
	os_timer_setfn(&timerUpdateTempPress, (os_timer_func_t *)updateTempPress_CB, (void *)0);
	os_timer_arm(&timerUpdateTempPress, DELAY_MS_1HZ, true);
}
void SPBOX_SENSORS::stopUpdateTempPress()
{
	os_timer_disarm(&timerUpdateTempPress);
	os_timer_disarm(&timerUpdateSteps);
	_updateStep = sensorPaused;
}

void SPBOX_SENSORS::updateAccelGyroMagCB()
{
	do_update_accel_gyro_mag_ = true;
}

bool SPBOX_SENSORS::checkAccelGyroMag()
{
	bool change_minmax;
	if (!do_update_accel_gyro_mag_)
		return false;
	do_update_accel_gyro_mag_ = false;

	fetchAccelGyro();
	calcAccelGyro();
	change_minmax = updateMinMaxAccelGyro();

	fetchMag();
	calibrateMag();
	calcMag();
	calcAltitude();

	accelGyroMagEvent_t temp_event;
	getAccelGyroMag(&temp_event);

	if (onChangeAccelGyroMagEvent != NULL)
		onChangeAccelGyroMagEvent(temp_event);     // call the handler

	if (onChangeMinMaxAccelGyroEvent != NULL)
		if (change_minmax) {
			minMaxAccelGyroEvent_t temp_minmaxevent;
			getMinMaxAccelGyro(&temp_minmaxevent);
			onChangeMinMaxAccelGyroEvent(temp_minmaxevent);     // call the handler
		}

	return true;
}

void SPBOX_SENSORS::updateTempPressCB()
{
	prepTempPressure();
}

void SPBOX_SENSORS::prepTempPressure()
{
	switch (_updateStep) {
	case sensorPaused:
		_updateStep = sensorReqTemp;
		barometer_.setControl(BMP085_MODE_TEMPERATURE);
		os_timer_disarm(&timerUpdateSteps);
		os_timer_setfn(&timerUpdateSteps, (os_timer_func_t *)updateTempPress_CB, (void *)0);
		os_timer_arm(&timerUpdateSteps, barometer_.getMeasureDelayMilliseconds(), false);
		break;
	case sensorReqTemp:
		_updateStep = sensorReadTempReqPress;
		temperature_ = barometer_.getTemperatureC();
		barometer_.setControl(BMP085_MODE_PRESSURE_3);
		os_timer_disarm(&timerUpdateSteps);
		os_timer_setfn(&timerUpdateSteps, (os_timer_func_t *)updateTempPress_CB, (void *)0);
		os_timer_arm(&timerUpdateSteps, barometer_.getMeasureDelayMilliseconds(), false);
		break;
	case sensorReadTempReqPress:
		_updateStep = sensorDone;
		pressure_ = barometer_.getPressure();
		os_timer_disarm(&timerUpdateSteps);
		break;
	default:
		break;
	};
}

bool SPBOX_SENSORS::checkTempPress()
{
	if (_updateStep != sensorDone)
		return false;
	_updateStep = sensorPaused;
	calcAltitude();

	tempPressAltiEvent_t temp_event;
	temp_event.altitude = altitude_;
	temp_event.pressure = pressure_;
	temp_event.temperature = temperature_;

	if (onChangeTempPressAltiEvent != NULL)
		onChangeTempPressAltiEvent(temp_event);     // call the handler

	return true;
}

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

bool SPBOX_SENSORS::updateMinMaxAccelGyro() {
	bool chg = false;

	if (ax_f_ > max_ax_f_) { max_ax_f_ = ax_f_; chg = true; }
	if (ay_f_ > max_ay_f_) { max_ay_f_ = ay_f_; chg = true; }
	if (az_f_ > max_az_f_) { max_az_f_ = az_f_; chg = true; }
	if (ax_f_ < min_ax_f_) { min_ax_f_ = ax_f_; chg = true; }
	if (ay_f_ < min_ay_f_) { min_ay_f_ = ay_f_; chg = true; }
	if (az_f_ < min_az_f_) { min_az_f_ = az_f_; chg = true; }
	if (gx_f_ > max_gx_f_) { max_gx_f_ = gx_f_; chg = true; }
	if (gy_f_ > max_gy_f_) { max_gy_f_ = gy_f_; chg = true; }
	if (gz_f_ > max_gz_f_) { max_gz_f_ = gz_f_; chg = true; }
	if (gx_f_ < min_gx_f_) { min_gx_f_ = gx_f_; chg = true; }
	if (gy_f_ < min_gy_f_) { min_gy_f_ = gy_f_; chg = true; }
	if (gz_f_ < min_gz_f_) { min_gz_f_ = gz_f_; chg = true; }

	return chg;
}

void SPBOX_SENSORS::fetchMag()
{
	mag_.getHeading(&mx_, &my_, &mz_);

	/*Serial.printf("%d\t%d\t%d\t", mx_, my_, mz_);*/
}

void SPBOX_SENSORS::calibrateMag()
{
	mx_f_ = (float)mx_ + cal_offsets[MAG_X];
	my_f_ = (float)my_ + cal_offsets[MAG_Y];
	mz_f_ = (float)mz_ + cal_offsets[MAG_Z];

	mx_f_ = cal_matrix[MAG_X][MAG_X] * mx_f_ + cal_matrix[MAG_X][MAG_Y] * my_f_ + cal_matrix[MAG_X][MAG_Z] * mz_f_;
	my_f_ = cal_matrix[MAG_Y][MAG_X] * mx_f_ + cal_matrix[MAG_Y][MAG_Y] * my_f_ + cal_matrix[MAG_Y][MAG_Z] * mz_f_;
	mz_f_ = cal_matrix[MAG_Z][MAG_X] * mx_f_ + cal_matrix[MAG_Z][MAG_Y] * my_f_ + cal_matrix[MAG_Z][MAG_Z] * mz_f_;

	//Serial.printf("%d\t%d\t%d\t", (int16_t)mx_f_, (int16_t)my_f_, (int16_t)mz_f_);
}

void SPBOX_SENSORS::calcMag()
{
	// To calculate heading in degrees. 0 degree indicates North
	float heading = atan2(-my_f_, -mz_f_);
	heading += declination_angle;
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180.0 / M_PI;
	heading_ = heading;

	//char tempbuffer[8][20], disp[100];
	//dtostrf(heading_, 4, 2, tempbuffer[0]);
	//Serial.printf("%s\n", tempbuffer[0]);
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

void SPBOX_SENSORS::getAccelGyroMag(accelGyroMagEvent_t *e)
{
	getAccel(&(e->ax_f), &(e->ay_f), &(e->az_f));
	getAccel(&(e->gx_f), &(e->gy_f), &(e->gz_f));
	getHeading(&(e->heading));
}

void SPBOX_SENSORS::getMinMaxAccelGyro(minMaxAccelGyroEvent_t *e)
{
	getMinAccel(&(e->min_ax_f), &(e->min_ay_f), &(e->min_az_f));
	getMaxAccel(&(e->max_ax_f), &(e->max_ay_f), &(e->max_az_f));
	getMinGyro(&(e->min_gx_f), &(e->min_gy_f), &(e->min_gz_f));
	getMaxGyro(&(e->max_gx_f), &(e->max_gy_f), &(e->max_gz_f));
}

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

void SPBOX_SENSORS::onAccelGyroMagEvent(onAccelGyroMagEvent_t handler)
{
	onChangeAccelGyroMagEvent = handler;
}

void SPBOX_SENSORS::onMinMaxAccelGyroEvent(onMinMaxAccelGyroEvent_t handler)
{
	onChangeMinMaxAccelGyroEvent = handler;
}

void SPBOX_SENSORS::onTempPressAltiEvent(onTempPressAltiEvent_t handler)
{
	onChangeTempPressAltiEvent = handler;
}