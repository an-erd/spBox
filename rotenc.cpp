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
	// rotary encoder
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), rotencInt0, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), rotencInt1, CHANGE);
	int0signal = digitalRead(ENCODER_PIN_A);
	int0history = int0signal;
	int1signal = digitalRead(ENCODER_PIN_B);
	int1history = int1signal;
	rotaryHalfSteps = 0;
	actualRotaryTicks = 0;
	changed_rotEnc = false;
	changed_halfSteps = false;

	LCDML_rotenc_value = 0;
	LCDML_rotenc_value_history = 0;

	// switch
	pinMode(ENCODER_SW, OUTPUT);
	digitalWrite(ENCODER_SW, 0);
	pinMode(ENCODER_SW, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(ENCODER_SW), int2, CHANGE);
	int_signal = digitalRead(ENCODER_SW);
	int_history = button.int_signal;
	changed = false;
}

void ROTENC::onROTENCChangeEvent(onRotencChangeEvent_t handler)
{
	onChangeEvent = handler;
}

// rotary encoder and rotary encoder button interrupt routines
void ROTENC::isrInt0() {
	if (millis() - rotenc.int0time < THRESHOLD)
		return;
	rotenc.int0history = rotenc.int0signal;
	rotenc.int0signal = digitalRead(ENCODER_PIN_A);
	if (rotenc.int0history == rotenc.int0signal)
		return;
	rotenc.int0time = millis();
	if (rotenc.int0signal == rotenc.int1signal) {
		rotenc.rotaryHalfSteps--;
	}
	else {
		rotenc.rotaryHalfSteps++;
	}

	rotenc.changed_halfSteps = true;
}
void ROTENC::isrInt1() {
	if (millis() - rotenc.int1time < THRESHOLD)
		return;
	rotenc.int1history = rotenc.int1signal;
	rotenc.int1signal = digitalRead(ENCODER_PIN_B);
	if (rotenc.int1history == rotenc.int1signal)
		return;
	rotenc.int1time = millis();
}


void ROTENC::checkRotaryEncoder() {
	if (rotenc.changed_halfSteps) {
		rotenc.changed_halfSteps = false;

		if (rotenc.rotaryHalfSteps % 2 == 0) {
			rotenc.actualRotaryTicks = rotenc.rotaryHalfSteps / 2;
			rotenc.LCDML_rotenc_value = rotenc.actualRotaryTicks;
		}
	}
}

ROTENC rotenc;