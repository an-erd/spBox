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

#include "myconfig.h"
#include "spbox_sensors.h"

#ifdef DEBUG_SENSORS
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

// magnometer calibration values
// (done with Magneto https://sites.google.com/site/sailboatinstruments1/home
// using https://www.ngdc.noaa.gov/geomag-web/ following http://www.germersogorb.de/html/kalibrierung_des_hcm5883l.html)
double cal_matrix[3][3] = {
	{0.970506, 0.002044, 0.004219},
	{0.002044, 0.945654, -0.002506},
	{0.004219, -0.002506, 1.070759} };
double cal_offsets[3] = { -53.818994, 77.549821, 214.359594 };
double declination_angle = 0.04246890849; // in Rad (entspricht 2.43333 Deg =2°26'E)

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
	bool change_minmax, change_max_abs;
	if (!do_update_accel_gyro_mag_)
		return false;
	do_update_accel_gyro_mag_ = false;

	fetchAccelGyro();
	change_minmax = updateMinMaxAccelGyro();
	updateAbsAccel();
	change_max_abs = updateMaxAbsAccel();

	fetchMag();
	//float cM1 = calcMag();
	float cM2 = calcMagCompensated();
	//Serial.print("\t\tH1 "); Serial.print(cM1, 1);
	//Serial.print("\tH2 "); Serial.print(cM2, 1);	Serial.println();

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
	int16_t temp_ax, temp_ay, temp_az, temp_gx, temp_gy, temp_gz;
	accelgyro_.getMotion6(&temp_ax, &temp_ay, &temp_az, &temp_gx, &temp_gy, &temp_gz);

	// apply offset and gain, and change to NED reference orientation:
	//		x' = -z; y' = y; z'=x	for accel and gyro
	ax_f_ = (float)(temp_az - MPU6050_AZOFFSET) / MPU6050_A_GAIN;
	ay_f_ = -(float)(temp_ay - MPU6050_AYOFFSET) / MPU6050_A_GAIN;
	az_f_ = -(float)(temp_ax - MPU6050_AXOFFSET) / MPU6050_A_GAIN;

	gx_f_ = -(float)(temp_gz - MPU6050_GZOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;	// rad/s
	gy_f_ = (float)(temp_gy - MPU6050_GYOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;
	gz_f_ = (float)(temp_gx - MPU6050_GXOFFSET) * MPU6050_GAIN_DEG_RAD_CONV;

	//Serial.print("\tA "); Serial.print(ax_f_, 1); Serial.print("\t"); Serial.print(ay_f_, 1); Serial.print("\t"); Serial.print(az_f_, 1);
	//Serial.print("\tG "); Serial.print(gx_f_, 1); Serial.print("\t"); Serial.print(gy_f_, 1); Serial.print("\t"); Serial.print(gz_f_, 1); Serial.println();
}

void SPBOX_SENSORS::resetMinMaxAccelGyro()
{
	max_ax_f_ = min_ax_f_ = ax_f_;
	max_ay_f_ = min_ay_f_ = ay_f_;
	max_az_f_ = min_az_f_ = az_f_;
	max_gx_f_ = min_gx_f_ = gx_f_;
	max_gy_f_ = min_gy_f_ = gy_f_;
	max_gz_f_ = min_gz_f_ = gz_f_;
}

void SPBOX_SENSORS::resetMaxAbsAccel()
{
	max_abs_accel_f_ = 0;
}

bool SPBOX_SENSORS::updateMinMaxAccelGyro()
{
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

void SPBOX_SENSORS::updateAbsAccel()
{
	abs_accel_f_ = sqrt(ax_f_ * ax_f_ + ay_f_ * ay_f_ + az_f_ * az_f_);
}

bool SPBOX_SENSORS::updateMaxAbsAccel()
{
	bool chg = false;

	if (abs_accel_f_ > max_abs_accel_f_) {
		max_abs_accel_f_ = abs_accel_f_;
		chg = true;
	}

	return chg;
}

void SPBOX_SENSORS::fetchMag()
{
	int16_t temp_mx, temp_my, temp_mz;
	float temp_mxf, temp_myf, temp_mzf;

	mag_.getHeading(&temp_mx, &temp_my, &temp_mz);
	//Serial.printf("%d\t%d\t%d\t", temp_mx, temp_my, temp_mz);

	// apply offset and calibration matrix, and change to NED reference orientation:
	//		x' = -z; y' = y; z'=x	for accel and gyro
	temp_mxf = (float)temp_mx + cal_offsets[MAG_X];
	temp_myf = (float)temp_my + cal_offsets[MAG_Y];
	temp_mzf = (float)temp_mz + cal_offsets[MAG_Z];

	mx_f_ = -(cal_matrix[MAG_Z][MAG_X] * temp_mxf + cal_matrix[MAG_Z][MAG_Y] * temp_myf + cal_matrix[MAG_Z][MAG_Z] * temp_mzf);
	my_f_ = cal_matrix[MAG_Y][MAG_X] * temp_mxf + cal_matrix[MAG_Y][MAG_Y] * temp_myf + cal_matrix[MAG_Y][MAG_Z] * temp_mzf;
	mz_f_ = cal_matrix[MAG_X][MAG_X] * temp_mxf + cal_matrix[MAG_X][MAG_Y] * temp_myf + cal_matrix[MAG_X][MAG_Z] * temp_mzf;

	//Serial.print("\tM "); Serial.print(mx_f_, 1); Serial.print("\t"); Serial.print(my_f_, 1); Serial.print("\t"); Serial.print(mz_f_, 1);
}

float SPBOX_SENSORS::calcMag()
{
	// To calculate heading in degrees. 0 degree indicates North, take adjusted NED reference orientation into account
	float heading = atan2(-my_f_, mx_f_);
	heading += declination_angle;
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180.0 / M_PI;
	heading_ = heading;

	//Serial.print("H1: "); Serial.print(heading_, 1);

	return heading_;
}

float SPBOX_SENSORS::calcMagCompensated()
{
	float roll, cos_roll, sin_roll;
	float pitch, cos_pitch, sin_pitch;
	float cmx, cmy, cmz;
	float heading;

	roll = atan2(ay_f_, az_f_);
	cos_roll = cos(roll);
	sin_roll = sin(roll);

	pitch = atan2(-ax_f_, ay_f_*sin_roll + az_f_*cos_roll);
	cos_pitch = cos(pitch);
	sin_pitch = sin(pitch);

	cmx = mx_f_ * cos_pitch + my_f_ * sin_pitch * sin_roll + mz_f_ * sin_pitch * cos_roll;
	cmy = my_f_*cos_roll - mz_f_ * sin_roll;
	cmz = -mx_f_ * sin_pitch + my_f_ * cos_pitch*sin_roll + mz_f_ * cos_pitch * cos_roll;

	heading = atan2(-cmy, cmx) + declination_angle;
	if (heading < 0)
		heading += M_TWOPI;
	heading *= 180.0 / M_PI;

	heading_ = heading;

	//Serial.print("\tRoll: "); Serial.print(roll); Serial.print("\tPitch: "); Serial.print(pitch);
	//Serial.print("\tM2 ");
	//Serial.print(cmx, 1); Serial.print("\t"); Serial.print(cmy, 1); Serial.print("\t"); Serial.print(cmz, 1);
	//Serial.print("\tH2: "); Serial.print(heading_, 1); Serial.println();

	return heading_;
}

void SPBOX_SENSORS::calcAltitude()
{
	// calculate absolute altitude in meters based on known pressure
	// (may pass a second "sea level pressure" parameter here,
	// otherwise uses the standard value of 101325 Pa)
	altitude_ = barometer_.getAltitude(pressure_, pressureAtSealevel_);
}

void SPBOX_SENSORS::getAccel(float * ax_f, float * ay_f, float * az_f) { *ax_f = ax_f_; *ay_f = ay_f_; *az_f = az_f_; }
void SPBOX_SENSORS::getMaxAccel(float * max_ax, float * max_ay, float * max_az) { *max_ax = max_ax_f_; *max_ay = max_ay_f_; *max_az = max_az_f_; }
void SPBOX_SENSORS::getMinAccel(float * min_ax, float * min_ay, float * min_az) { *min_ax = min_ax_f_; *min_ay = min_ay_f_; *min_az = min_az_f_; }
void SPBOX_SENSORS::getAbsAccel(float * abs_accel) { *abs_accel = abs_accel_f_; }
void SPBOX_SENSORS::getMaxAbsAccel(float * max_abs) { *max_abs = max_abs_accel_f_; }

void SPBOX_SENSORS::getGyro(float * gx_f, float * gy_f, float * gz_f) { *gx_f = gx_f_; *gy_f = gy_f_; *gz_f = gz_f_; }
void SPBOX_SENSORS::getMaxGyro(float * max_gx, float * max_gy, float * max_gz) { *max_gx = max_gx_f_; *max_gy = max_gy_f_; *max_gz = max_gz_f_; }
void SPBOX_SENSORS::getMinGyro(float * min_gx, float * min_gy, float * min_gz) { *min_gx = min_gx_f_; *min_gy = min_gy_f_; *min_gz = min_gz_f_; }

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

// calc pressure at set level with given an absolute altitude according to the BMP085 datasheet, P. 13
float SPBOX_SENSORS::calcPressureAtSealevel(float altitude)
{
	pressureAtSealevel_ = pressure_ / pow((1.0 - altitude / 44330.0), 5.255);

	return pressureAtSealevel_;
}

void SPBOX_SENSORS::setPressureAtSealevel(float seaLevelPressure)
{
	pressureAtSealevel_ = seaLevelPressure;	// according to BMP085 data sheet
}

void SPBOX_SENSORS::updateVBat() {
	uint16_t vbatADC;
	float vbatFloat = 0.0F;
	float vbatLSB = 0.97751F;		// 1000mV/1023 -> mV per LSB
	float vbatVoltDiv = 0.20015680;	// 271K/1271K resistor voltage divider

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