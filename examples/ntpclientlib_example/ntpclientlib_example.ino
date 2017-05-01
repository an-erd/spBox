#include <TimeLib.h>
#include <NtpClientLib.h>
#include <ESP8266WiFi.h>

#ifndef WIFI_CONFIG_H
#define YOUR_WIFI_SSID ".."
#define YOUR_WIFI_PASSWD ".."
#endif // !WIFI_CONFIG_H

#define ONBOARDLED 2 // Built in LED on ESP-12/ESP-07

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
	Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
	NTP.begin("pool.ntp.org", 1, true);
	NTP.setInterval(63);
	digitalWrite(ONBOARDLED, LOW); // Turn on LED
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
	Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
	Serial.printf("Reason: %d\n", event_info.reason);
	digitalWrite(ONBOARDLED, HIGH); // Turn off LED
	//NTP.stop(); // NTP sync can be disabled to avoid sync errors
}

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

void setup()
{
	static WiFiEventHandler e1, e2;

	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(YOUR_WIFI_SSID, YOUR_WIFI_PASSWD);

	//pinMode(ONBOARDLED, OUTPUT); // Onboard LED
	//digitalWrite(ONBOARDLED, HIGH); // Switch off LED

	NTP.onNTPSyncEvent([](NTPSyncEvent_t event) {
		ntpEvent = event;
		syncEventTriggered = true;
	});

	// Deprecated
	/*WiFi.onEvent([](WiFiEvent_t e) {
		Serial.printf("Event wifi -----> %d\n", e);
	});*/

	e1 = WiFi.onStationModeGotIP(onSTAGotIP);// As soon WiFi is connected, start NTP Client
	e2 = WiFi.onStationModeDisconnected(onSTADisconnected);
}

void loop()
{
	static int i = 0;
	static int last = 0;

	if (syncEventTriggered) {
		processSyncEvent(ntpEvent);
		syncEventTriggered = false;
	}

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
	delay(0);
}