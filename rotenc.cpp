//
//
//

#include "rotenc.h"

// ISR wrapper functions
void rotencInt0() { rotenc.isrInt0(); }
void rotencInt1() { rotenc.isrInt1(); }

ROTENC::ROTENC()
{
}

void ROTENC::initialize()
{
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
	rotary_half_steps_ = 0;
	actualRotaryTicks = 0;

	//LCDML_rotenc_value = 0;
	//LCDML_rotenc_value_history = 0;
}

void ROTENC::start()
{
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

void ROTENC::isrInt0() {
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

void ROTENC::isrInt1() {
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
	int	diff = historyTicks - tempTicks;

	if (!diff)
		return false;

	actualRotaryTicks = tempTicks;
	//LCDML_rotenc_value = actualRotaryTicks;

	rotencPosEvent_t temp_event;
	temp_event.event = (diff > 0 ? CW : CCW);
	temp_event.diff = diff;
	temp_event.pos = actualRotaryTicks;

	// call the handler
	if (onChangeEvent != NULL)
		onChangeEvent(temp_event.event);
	if (onPosEvent != NULL)
		onPosEvent(temp_event);
}

ROTENC rotenc;