//
//
//

#include "spbox_com.h"

volatile bool	do_update_mqtt;

// WiFiClient, MQTT Client and Feed, event handler
WiFiClient				client;
WiFiEventHandler		gotIpEventHandler;		// OK
WiFiEventHandler		disconnectedEventHandler;	// OK
Adafruit_MQTT_Client	mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish	battery = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/battery");

void check_mqtt()
{
	if (!do_update_mqtt)
		return;

	do_update_mqtt = false;

	updateVbat();

	if (!battery.publish(g_vbatADC)) {
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Update vbat Failed."));
#endif
	}
	else {
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Update vbat Success!"));
#endif
	}
}

// Start NTP only after IP network is connected
void onSTAGotIP(WiFiEventStationModeGotIP ipInfo) {
	Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
	NTP.begin("pool.ntp.org", 1, true);
	NTP.setInterval(63);
}

// Manage network disconnection
void onSTADisconnected(WiFiEventStationModeDisconnected event_info) {
	Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
	Serial.printf("Reason: %d\n", event_info.reason);
	NTP.stop(); // NTP sync can be disabled to avoid sync errors
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

void connect_adafruit_io() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.print(F("Connecting to Adafruit IO... "));
#endif

	int8_t ret;

	while ((ret = mqtt.connect()) != 0) {
		switch (ret) {
		case 1: Serial.println(F("Wrong protocol")); break;
		case 2: Serial.println(F("ID rejected")); break;
		case 3: Serial.println(F("Server unavail")); break;
		case 4: Serial.println(F("Bad user/pass")); break;
		case 5: Serial.println(F("Not authed")); break;
		case 6: Serial.println(F("Failed to subscribe")); break;
		default: Serial.println(F("Connection failed")); break;
		}

		if (ret >= 0)
			mqtt.disconnect();

#ifdef SERIAL_STATUS_OUTPUT
		Serial.println(F("Retrying connection..."));
#endif
		delay(5000);
	}
#ifdef SERIAL_STATUS_OUTPUT
	Serial.println(F("Adafruit IO Connected!"));
#endif
}

void switch_WLAN(bool turn_on) {
	if (turn_on) {
		if (!gConfig.wlan_initialized) {
			initialize_WLAN();
		}
		else {
			WiFi.mode(WIFI_STA);			// WL_CONNECTED
			WiFi.begin();
#ifdef SERIAL_STATUS_OUTPUT
			Serial.println("WLAN turned on");
#endif
			gConfig.wlan_enabled = true;
		}
	}
	else
	{
		WiFi.mode(WIFI_OFF);
#ifdef SERIAL_STATUS_OUTPUT
		Serial.println("WLAN turned off");
#endif
		gConfig.wlan_enabled = false;
	}
}