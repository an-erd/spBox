// BMP085_nb.h
// "BMP085 non-blocking" implementation based on I2CDEV prepares and provides temperatur and pressure measurements

#ifndef _BMP085_NB_h
#define _BMP085_NB_h

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#endif

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

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

#ifdef ARDUINO_ARCH_ESP8266
#include <functional>
typedef std::function<void(BMP085PrepDoneEvent_t)> onPrepDoneEvent_t;
#else
typedef void(*onPrepDoneEvent_t)(BMP085PrepDoneEvent_t);
#endif

class BMP085_NB : public BMP085
{
public:
	//BMP085_NB();
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

extern BMP085_NB barometer;

#endif
