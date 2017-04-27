//
//
//

#include "button.h"
#include "user_config.h"

#define DBG_PORT Serial

#ifdef DEBUG_BUTTON
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

// ISR wrapper functions
void buttonInt0() { button.isrInt0(); }

BUTTON button;

BUTTON::BUTTON()
{
}

void BUTTON::initialize()
{
	pinMode(ENCODER_SW, OUTPUT);
	digitalWrite(ENCODER_SW, 0);
	pinMode(ENCODER_SW, INPUT_PULLUP);
}

void BUTTON::start()
{
	attachInterrupt(digitalPinToInterrupt(ENCODER_SW), buttonInt0, CHANGE);
	int_signal_ = digitalRead(ENCODER_SW);
	int_history_ = int_signal_;
	changed_ = false;
}

void BUTTON::stop()
{
	detachInterrupt(digitalPinToInterrupt(ENCODER_SW));
}

void BUTTON::onButtonChangeEvent(onButtonChangeEvent_t handler)
{
	onChangeEvent = handler;
}

void BUTTON::isrInt0() {
	volatile uint32_t time_diff;
	time_diff = millis() - int_time_;
	if (time_diff < THRESHOLD)
		return;
	int_history_ = int_signal_;
	int_signal_ = digitalRead(ENCODER_SW);
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

	DEBUGLOG("button, event %d\r\n", temp_event);

	if (onChangeEvent != NULL)
		onChangeEvent(temp_event);     // call the handler

	return true;
}