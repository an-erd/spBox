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
#include "rotenc.h"

#ifdef DEBUG_ROTENC
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

ROTENC rotenc;

inline void		ICACHE_RAM_ATTR rotencInt0() { rotenc.isrInt0(); };				// only for isr wrapper
inline void		ICACHE_RAM_ATTR rotencInt1() { rotenc.isrInt1(); };

ROTENC::ROTENC()
{
}

void ROTENC::initialize()
{
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
	rotary_half_steps_ = 0;
	actualRotaryTicks = 0;
}

void ROTENC::start()
{
	//attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), std::bind(&ROTENC::isrInt0, &rotenc), CHANGE);
	//attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), std::bind(&ROTENC::isrInt1, &rotenc), CHANGE);

	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotencInt0, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotencInt1, CHANGE);

	int0_signal_ = digitalRead(ENCODER_PIN_A);
	int0_history_ = int0_signal_;
	int1_signal_ = digitalRead(ENCODER_PIN_B);
	int1_history_ = int1_signal_;

	changed_ = false;
	changed_halfSteps_ = false;
}

void ROTENC::stop()
{
	detachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A));
	detachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B));
}

void ROTENC::onRotencChangeEvent(onRotencChangeEvent_t handler)
{
	onChangeEvent = handler;
}

void ROTENC::onRotencPosEvent(onRotencPosEvent_t handler)
{
	onPosEvent = handler;
}

void ICACHE_RAM_ATTR ROTENC::isrInt0() {
	if (millis() - int0_time_ < THRESHOLD)
		return;
	int0_history_ = int0_signal_;
	int0_signal_ = digitalRead(ENCODER_PIN_A);
	if (int0_history_ == int0_signal_)
		return;
	int0_time_ = millis();
	if (int0_signal_ == int1_signal_) {
		rotary_half_steps_--;
	}
	else {
		rotary_half_steps_++;
	}

	changed_halfSteps_ = true;
}

void ICACHE_RAM_ATTR ROTENC::isrInt1() {
	if (millis() - int1_time_ < THRESHOLD)
		return;
	int1_history_ = int1_signal_;
	int1_signal_ = digitalRead(ENCODER_PIN_B);
	if (int1_history_ == int1_signal_)
		return;
	int1_time_ = millis();
}

bool ROTENC::check() {
	if (!changed_halfSteps_)
		return false;
	changed_halfSteps_ = false;

	if (rotary_half_steps_ % 2)
		return false;	// no new full rot enc pos

	long historyTicks = actualRotaryTicks;
	long tempTicks = rotary_half_steps_ / 2;
	int	diff = tempTicks - historyTicks;

	if (!diff)
		return false;

	actualRotaryTicks = tempTicks;
	rotencPosEvent_t temp_event;
	temp_event.event = (diff > 0 ? CW : CCW);
	temp_event.diff = diff;
	temp_event.pos = actualRotaryTicks;

	DEBUGLOG("rotary encoder, event: %d, diff: %d, pos: %d\r\n", temp_event.event, temp_event.diff, temp_event.pos);

	// call the handler
	if (onChangeEvent != NULL)
		onChangeEvent(temp_event.event);
	if (onPosEvent != NULL)
		onPosEvent(temp_event);
}