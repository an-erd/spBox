// BMP085_nb.h
// "BMP085 non-blocking" implementation based on I2CDEV prepares and provides temperatur and pressure measurements

#ifndef _BMP085_NB_h
#define _BMP085_NB_h

extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#include "arduino.h"
#include <BMP085.h>

typedef enum {
	prepOk,
	prepFail
} BMP085PrepDoneEvent_t;

typedef enum {
	sensorPaused = 0,
	sensorReqTemp,
	sensorReadTempReqPress,
	sensorDone
} BMP085UpdateSteps_t;

typedef std::function<void(BMP085PrepDoneEvent_t)> onPrepDoneEvent_t;

class BMP085_NB : public BMP085
{
public:
	BMP085_NB();
	void	initialize();

	void	setUpdateInterval(int interval = 1000);
	void	startAutomaticUpdate(void);
	void	stopAutomaticUpdate(void);
	void	onBMP085PrepDoneEvent(onPrepDoneEvent_t handler);

	void	prepTempPressure();
	float	getPressure();
	float	getTemperatureC();
protected:
	uint8_t	_updateInterval;	// in ms
	BMP085UpdateSteps_t _updateStep;
	onPrepDoneEvent_t onPrepDoneEvent;

	float	_temperatureC;
	float	_pressure;
private:
};

#endif
