//
//
//

#include "BMP085_nb.h"

LOCAL os_timer_t timerUpdateTempPress;	// prepare temperature and pressure regularly
LOCAL os_timer_t timerUpdateSteps;		// handle different BMP085 preparation duration for temperature and pressure

void updateTempPressCB(void *arg) {
	//barometer.prepTempPressure();
}

BMP085_NB::BMP085_NB()
{
}

void BMP085_NB::initialize()
{
	BMP085::initialize();
	_updateStep = sensorPaused;
	_updateInterval = 1000;
}

void BMP085_NB::setUpdateInterval(int interval)
{
	_updateInterval = interval;
}

void BMP085_NB::startAutomaticUpdate(void)
{
	os_timer_disarm(&timerUpdateTempPress);
	os_timer_setfn(&timerUpdateTempPress, (os_timer_func_t *)updateTempPressCB, (void *)0);
	os_timer_arm(&timerUpdateTempPress, _updateInterval, true);
}

void BMP085_NB::stopAutomaticUpdate(void)
{
	os_timer_disarm(&timerUpdateTempPress);
	os_timer_disarm(&timerUpdateSteps);
	_updateStep = sensorPaused;
}

void BMP085_NB::onBMP085PrepDoneEvent(onPrepDoneEvent_t handler)
{
	onPrepDoneEvent = handler;
}

void BMP085_NB::prepTempPressure()
{
	switch (_updateStep) {
	case sensorPaused:
		_updateStep = sensorReqTemp;
		BMP085::setControl(BMP085_MODE_TEMPERATURE);
		os_timer_disarm(&timerUpdateSteps);
		os_timer_setfn(&timerUpdateSteps, (os_timer_func_t *)updateTempPressCB, (void *)0);
		os_timer_arm(&timerUpdateSteps, BMP085::getMeasureDelayMilliseconds(), false);
		break;
	case sensorReqTemp:
		_updateStep = sensorReadTempReqPress;
		_temperatureC = BMP085::getTemperatureC();
		BMP085::setControl(BMP085_MODE_PRESSURE_3);
		os_timer_disarm(&timerUpdateSteps);
		os_timer_setfn(&timerUpdateSteps, (os_timer_func_t *)updateTempPressCB, (void *)0);
		os_timer_arm(&timerUpdateSteps, BMP085::getMeasureDelayMilliseconds(), false);
		break;
	case sensorReadTempReqPress:
		_updateStep = sensorDone;
		_pressure = BMP085::getPressure();
		os_timer_disarm(&timerUpdateSteps);
		if (onPrepDoneEvent != NULL)
			onPrepDoneEvent(prepOk);     // call the handler
		break;
	default:
		break;
	};
}

float BMP085_NB::getPressure()
{
	return _pressure;
}

float BMP085_NB::getTemperatureC()
{
	return _temperatureC;
}