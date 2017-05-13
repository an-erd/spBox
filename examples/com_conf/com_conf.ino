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