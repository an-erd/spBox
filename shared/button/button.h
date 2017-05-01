// button.h

#ifndef _BUTTON_h
#define _BUTTON_h

extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#include "arduino.h"
#include "myconfig.h"

// for the description see table in button.cpp, before BUTTON::check()
typedef enum ButtonEvent {
	H_L_SHORT = 0, H_L_LONG = 2, H_L_VERYLONG = 3, L_H_SHORT = 4, L_H_LONG = 6, L_H_VERYLONG = 7
} buttonChangeEvent_t;

typedef std::function<void(buttonChangeEvent_t)> onButtonChangeEvent_t;

class BUTTON
{
public:
	BUTTON();
	void	initialize();
	void	start();				// attach interrupt and variable initialization
	void	stop();					// detach interrupt

	bool	check();				// call in loop() for events to create
	void	onButtonChangeEvent(onButtonChangeEvent_t handler);

	void	isrInt0();				// only for isr wrapper
private:
	volatile uint32_t	int_time_;				// store time for threshold and duration
	volatile uint8_t	int_signal_;				// store status between subsequent calls
	volatile uint8_t	int_history_;			// --"--

	volatile bool		changed_;
	volatile uint8_t	changed_signal_;
	volatile uint32_t	changed_time_diff_;		// time diff between last button change
protected:
	onButtonChangeEvent_t onChangeEvent;
};

extern BUTTON button;
#endif
