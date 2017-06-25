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

typedef enum RotEncEvent {
	CCW = -1,
	CW = 1
} rotencChangeEvent_t;

typedef struct RotEncPosEvent {
	rotencChangeEvent_t event;
	int diff;
	long pos;
} rotencPosEvent_t;

typedef std::function<void(rotencChangeEvent_t)> onRotencChangeEvent_t;
typedef std::function<void(rotencPosEvent_t)>    onRotencPosEvent_t;

class ROTENC
{
public:
	ROTENC();
	void		initialize();
	void		start();			// attach interrupt and variable initialization
	void		stop();				// detach interrupt

	bool		check();			// call in loop() for events to create

	void		onRotencChangeEvent(onRotencChangeEvent_t handler);
	void		onRotencPosEvent(onRotencPosEvent_t    handler);

	inline void		ICACHE_RAM_ATTR isrInt0() __attribute__((always_inline));				// only for isr wrapper
	inline void		ICACHE_RAM_ATTR isrInt1() __attribute__((always_inline));
private:
	volatile uint32_t	int0_time_;			// ISR threshold
	volatile uint32_t	int1_time_;
	volatile uint8_t	int0_signal_;
	volatile uint8_t	int0_history_;
	volatile uint8_t	int1_signal_;
	volatile uint8_t	int1_history_;
	volatile long		rotary_half_steps_;

	volatile bool		changed_halfSteps_;
	volatile long		actualRotaryTicks;

	bool		changed_;

	long		LCDML_rotenc_value;
	long		LCDML_rotenc_value_history;

	volatile bool		changed__;
protected:
	onRotencChangeEvent_t onChangeEvent;
	onRotencPosEvent_t    onPosEvent;
};

extern ROTENC rotenc;
