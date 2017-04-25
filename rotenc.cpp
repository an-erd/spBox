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
	changed_rotEnc = false;
	changed_halfSteps = false;

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

// rotary encoder and rotary encoder button interrupt routines
void ROTENC::isrInt0() {
	if (millis() - rotenc.int0_time_ < THRESHOLD)
		return;
	rotenc.int0_history_ = rotenc.int0_signal_;
	rotenc.int0_signal_ = digitalRead(ENCODER_PIN_A);
	if (rotenc.int0_history_ == rotenc.int0_signal_)
		return;
	rotenc.int0_time_ = millis();
	if (rotenc.int0_signal_ == rotenc.int1_signal_) {
		rotenc.rotary_half_steps_--;
	}
	else {
		rotenc.rotary_half_steps_++;
	}

	rotenc.changed_halfSteps = true;
}

void ROTENC::isrInt1() {
	if (millis() - rotenc.int1_time_ < THRESHOLD)
		return;
	rotenc.int1_history_ = rotenc.int1_signal_;
	rotenc.int1_signal_ = digitalRead(ENCODER_PIN_B);
	if (rotenc.int1_history_ == rotenc.int1_signal_)
		return;
	rotenc.int1_time_ = millis();
}

void ROTENC::check() {
	if (rotenc.changed_halfSteps) {
		rotenc.changed_halfSteps = false;

		if (rotenc.rotary_half_steps_ % 2 == 0) {
			rotenc.actualRotaryTicks = rotenc.rotary_half_steps_ / 2;
			rotenc.LCDML_rotenc_value = rotenc.actualRotaryTicks;
		}
	}
}

ROTENC rotenc;