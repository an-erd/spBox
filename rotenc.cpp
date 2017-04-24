//
//
//

#include "rotenc.h"

// ISR wrapper functions
void int0() { rotenc.isrInt0(); }
void int1() { rotenc.isrInt1(); }
void int2() { rotenc.isrInt2(); }

ROTENC::ROTENC()
{
}

void ROTENC::initialize()
{
	// rotary encoder
	pinMode(ENCODER_PIN_A, INPUT_PULLUP);
	pinMode(ENCODER_PIN_B, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), int0, CHANGE);
	attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_B), int1, CHANGE);
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

void ROTENC::onROTENCChangeEvent(onChangeEvent_t handler)
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
void ROTENC::isrInt2() {
	uint32_t time_diff;
	time_diff = millis() - button.int_time;
	if (time_diff < THRESHOLD)
		return;
	button.int_history = button.int_signal;
	button.int_signal = digitalRead(ENCODER_SW);
	if (button.int_history == button.int_signal)
		return;
	button.long_diff_change = (time_diff > DELAY_MS_TWOSEC) ? true : false;
	button.very_long_diff_change = (time_diff > DELAY_MS_TENSEC) ? true : false;
	button.int_time = millis();
	button.changed = true;
}

void ROTENC::checkButton() {
	//	ArduinoOTA.handle();		// TODO

	if (!button.changed)
		return;

	if (button.int_signal) {
		if (!button.long_diff_change) {
			// kurz LOW -> HIGH
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz LOW jetzt HIGH");
#endif
		}
		else {
			// lange LOW -> HIGH
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: lange LOW jetzt HIGH");
#endif
		}
	}
	else {
		if (!button.long_diff_change) {
			// kurz HIGH -> LOW
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println(" Button: kurz HIGH jetzt LOW");
#endif
			//if (rotenc.actualRotaryTicks == DISPLAY_SCR_MAXVALUES) {
			//	reset_min_max_accelgyro();
			//}
			button.LCDML_button_pressed = true;
		}
		else {
			if (!button.very_long_diff_change) {
				// lange HIGH ->  LOW
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: lange HIGH jetzt LOW");
#endif

				switch_WLAN((gConfig.wlan_enabled ? false : true));
			}
			else
			{
#ifdef SERIAL_STATUS_OUTPUT
				Serial.println(" Button: very lange HIGH jetzt LOW");
#endif
			}
		}
	}
	button.changed = false;
	button.long_diff_change = false;
	button.very_long_diff_change = false;
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