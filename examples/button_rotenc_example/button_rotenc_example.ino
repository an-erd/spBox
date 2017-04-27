#include <wire.h>
#include "button.h"

//#include "rotenc.h"

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	Wire.begin();

	//rotenc.initialize();
	//rotenc.start();
	button.initialize();
	button.start();
}

void loop() {
	//rotenc.check();
	button.check();

	delay(0);
}