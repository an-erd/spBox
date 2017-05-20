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

#include <EEPROM.h>
#include "spbox_conf.h"
#include "spbox_com.h"
#include "myconfig.h"
#include "credentials.h"

void processSyncEvent(NTPSyncEvent_t ntpEvent) {
	if (ntpEvent) {
		Serial.print("Time Sync error: ");
		if (ntpEvent == noResponse)
			Serial.println("NTP server not reachable");
		else if (ntpEvent == invalidAddress)
			Serial.println("Invalid NTP server address");
	}
	else {
		Serial.print("Got NTP time: ");
		Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
	}
}

boolean syncEventTriggered = false; // True if a time even has been triggered
NTPSyncEvent_t ntpEvent; // Last triggered event

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	EEPROM.begin(512);

	conf.initialize(false);
	com.setConf(&conf);
	com.initialize();

	NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
		ntpEvent = event;
		syncEventTriggered = true;
	});

	com.initializeWlan();
	com.initializeOta();
	//com.initializeMQTT();

	Serial.println("end of setup()");
}

void loop() {
	//static int i = 0;
	//static int last = 0;

	//if (syncEventTriggered) {
	//	processSyncEvent(ntpEvent);
	//	syncEventTriggered = false;
	//}

	//if ((millis() - last) > 5100) {
	//	//Serial.println(millis() - last);
	//	last = millis();
	//	Serial.print(i); Serial.print(" ");
	//	Serial.print(NTP.getTimeDateString()); Serial.print(" ");
	//	Serial.print(NTP.isSummerTime() ? "Summer Time. " : "Winter Time. ");
	//	Serial.print("WiFi is ");
	//	Serial.print(WiFi.isConnected() ? "connected" : "not connected"); Serial.print(". ");
	//	Serial.print("Uptime: ");
	//	Serial.print(NTP.getUptimeString()); Serial.print(" since ");
	//	Serial.println(NTP.getTimeDateString(NTP.getFirstSync()).c_str());

	//	i++;
	//}

	//com.checkOta();
	com.checkPing();

	delay(0);
}