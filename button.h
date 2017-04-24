// switch.h

#ifndef _SWITCH_h
#define _SWITCH_h

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

typedef enum {
	NONE = 0, H_L_SHORT, H_L_LONG, H_L_VERYLONG, L_H_SHORT, L_H_LONG, L_H_VERYLONG
} SWITCHChangeEvent_t;

#ifdef ARDUINO_ARCH_ESP8266
#include <functional>
typedef std::function<void(SWITCHChangeEvent_t)> onSwitchChangeEvent_t;
#else
typedef void(*onSwitchChangeEvent_t)(SWITCHChangeEvent_t);
#endif

class BUTTON
{
public:
	BUTTON();
	void		initialize();
	void		start();				// attach interrupt and variable initialization
	void		stop();					// detach interrupt

	void		check();				// call in loop() for events to create
	void		onSWITCHChangeEvent(onSwitchChangeEvent_t handler);

	void		isrInt0();				// only for isr wrapper
private:
	uint32_t	int_time_;				// store time for threshold and duration
	uint8_t		int_signal_;				// store status between subsequent calls
	uint8_t		int_history_;			// --"--
	bool		changed_;
	bool		time_long_diff_;		// long time between change (>2 sec)
	bool		time_verylong_diff_;	// very long time between change (>10 sec)
	
	bool		LCDML_button_pressed;	// TODO LCDML
protected:
	onSwitchChangeEvent_t onChangeEvent;
};

extern BUTTON button;

#endif
