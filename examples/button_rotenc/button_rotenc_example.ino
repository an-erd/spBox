#include <wire.h>
#include "button.h"
#include "rotenc.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();

	rotenc.initialize();
	rotenc.start();
	button.initialize();
	button.start();
	button.onButtonChangeEvent([](buttonChangeEvent_t e) {
		Serial.printf("onButtonChangeEvent; %d\n", e);
	});
	rotenc.onRotencChangeEvent([](rotencChangeEvent_t e) {
		Serial.printf("onRotEncChangeEvent: %d\n", e);
	});
	rotenc.onRotencPosEvent([](rotencPosEvent_t e) {
		Serial.printf("onRotEncChangeEvent event: %d, diff: %d, pos: %d\n", e.event, e.diff, e.pos);
	});
}

void loop() {
	rotenc.check();
	button.check();

	delay(0);
}
