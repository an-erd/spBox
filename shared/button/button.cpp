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

//#include <FunctionalInterrupt.h>
#include "button.h"

#ifdef DEBUG_BUTTON
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

BUTTON button;

inline void		ICACHE_RAM_ATTR buttonInt0() { button.isrInt0(); };				// only for isr wrapper

BUTTON::BUTTON()
{
}

void BUTTON::initialize()
{
	DEBUGLOG("button::initialize()");
	pinMode(PIN_BUTTON, OUTPUT);
	digitalWrite(PIN_BUTTON, 0);
	pinMode(PIN_BUTTON, INPUT_PULLUP);
}

void BUTTON::start()
{
	DEBUGLOG("button::start()");
	//attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), std::bind(&BUTTON::isrInt0, &button), CHANGE);

	attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), buttonInt0, CHANGE);

	int_signal_ = digitalRead(PIN_BUTTON);
	int_history_ = int_signal_;
	changed_ = false;
}

void BUTTON::stop()
{
	DEBUGLOG("button::stop()");
	detachInterrupt(digitalPinToInterrupt(PIN_BUTTON));
}

void BUTTON::onButtonChangeEvent(onButtonChangeEvent_t handler)
{
	onChangeEvent = handler;
}

void ICACHE_RAM_ATTR BUTTON::isrInt0() {
	// ISR no debug output
	volatile uint32_t time_diff;
	time_diff = millis() - int_time_;
	if (time_diff < THRESHOLD)
		return;
	int_history_ = int_signal_;
	int_signal_ = digitalRead(PIN_BUTTON);
	if (int_history_ == int_signal_)
		return;
	int_time_ = millis();

	changed_ = true;
	changed_time_diff_ = time_diff;
	changed_signal_ = int_signal_;
}

// status is determined w/the following matrix:
//
//	signal	long	verylong	value	result			description
// ----------------------------------------------------------------------------
//	0		0		0			0		H_L_SHORT		short HIGH, now LOW
//	0		0		1			1						(invalid)
//	0		1		0			2		H_L_LONG		long HIGH, now LOW
//	0		1		1			3		H_L_VERYLONG	verylong HIGH, now LOW
//	1		0		0			4		L_H_SHORT		short LOW, now HIGH
//	1		0		1			5						(invalid)
//	1		1		0			6		L_H_LONG		long LOW, now HIGH
//	1		1		1			7		L_H_VERYLONG	verylong LOW, now HIGH
// ----------------------------------
//	(4)		(2)		(1)			-> gives value of the BUTTONChangeEvent_t
//
bool BUTTON::check() {
	if (!changed_)
		return false;
	changed_ = false;

	bool long_diff_ = (changed_time_diff_ > DELAY_MS_TWOSEC) ? true : false;
	bool verylong_diff_ = (changed_time_diff_ > DELAY_MS_TENSEC) ? true : false;

	buttonChangeEvent_t	temp_event = (buttonChangeEvent_t)
		((changed_signal_ << 2) | (long_diff_ << 1) | (verylong_diff_ << 0));

	DEBUGLOG("button::check(), event %d\r\n", temp_event);

	if (onChangeEvent != NULL)
		onChangeEvent(temp_event);     // call the handler

	return true;
}