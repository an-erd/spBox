// 
// 
// 

#include "button.h"

// ISR wrapper functions
void buttonInt0() { button.isrInt0(); }

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

void BUTTON::onSWITCHChangeEvent(onSwitchChangeEvent_t handler)
{
	onChangeEvent = handler;
}

void BUTTON::isrInt0() {
	uint32_t time_diff;
	time_diff = millis() - int_time_;
	if (time_diff < THRESHOLD)
		return;
	int_history_ = int_signal_;
	int_signal_ = digitalRead(ENCODER_SW);
	if (int_history_ == int_signal_)
		return;
	int_time_ = millis();

	time_long_diff_ = (time_diff > DELAY_MS_TWOSEC) ? true : false;
	time_verylong_diff_ = (time_diff > DELAY_MS_TENSEC) ? true : false;
	changed_ = true;
}

// status is determined w/the following matrix
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
//	(4)		(2)		(1)			-> gives value of the SWITCHChangeEvent_t
//

void BUTTON::check() {
	if (!changed_)
		return;

	SWITCHChangeEvent_t	temp_event = SWITCHChangeEvent_t::NONE;
	int temp = int_signal_*bit(3) | time_long_diff_*bit(2) | time_verylong_diff_*bit(3);

	if (int_signal_) {
		if (!time_long_diff_) {
			// kurz LOW -> HIGH
		}
		else {
			// lange LOW -> HIGH
		}
	}
	else {
		if (!time_long_diff_) {
			// kurz HIGH -> LOW
			LCDML_button_pressed = true;
		}
		else {
			if (!time_verylong_diff_) {
				// lange HIGH ->  LOW
			}
			else
			{
			}
		}
	}
	changed_ = false;
	time_long_diff_ = false;
	time_verylong_diff_ = false;

		//if (onSyncEvent != NULL)
	//			onSyncEvent(timeSyncd);     // call the handler


}

BUTTON button;