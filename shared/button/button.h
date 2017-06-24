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

#pragma once

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

	void	ICACHE_RAM_ATTR isrInt0();				// only for isr wrapper
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
