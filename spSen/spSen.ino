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


// EXAMPLE SEE http://lazyzero.de/elektronik/esp8266/dht_deepsleep/start

#include <Arduino.h>
#include <wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include "myconfig.h"
#include "credentials.h"

#ifdef DEBUG_SPREL
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif


WiFiEventHandler wifiConnectHandler;
AsyncMqttClient mqttClient;
StaticJsonBuffer<300> JSONBuffer;

char temp_text1[30];
char temp_text2[30];

void connectToWifi() {
	DEBUGLOG("spRel: connectToWifi\n");
	WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
	DEBUGLOG("spRel: onWifiConnect\n");
	connectToMqtt();
}

void connectToMqtt() {
	DEBUGLOG("spRel: connectToMqtt\n");
	mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
	DEBUGLOG("spRel: onMqttConnect, session %i\n", sessionPresent);
	uint16_t packetIdSub = mqttClient.subscribe("andreaserd/feeds/sonoff", 0);

	// TODO MQTT send info
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
	DEBUGLOG("spRel: onMqttSubscribe, packetId: %i, qos %i\n", packetId, qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
	DEBUGLOG("spRel: onMqttUnsubscribe, packetId: %i\n", packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
	DEBUGLOG("spRel: onMqttMessage, topic %s, qos %i, dup %i, retain %i, len %i, index %i, total %i, payload %s\n",
		topic, properties.qos, properties.dup, properties.retain, len, index, total, payload);
}

void onMqttPublish(uint16_t packetId) {
	DEBUGLOG("spRel: onMqttPublish, packetId: %i\n", packetId);
}

void initialize_GPIO() {
	//pinMode(PIN_RELAIS, OUTPUT);
	//relais_state = LOW;
	//digitalWrite(PIN_RELAIS, relais_state);	 // relais is active high
}

void initializeOta()
{
	ArduinoOTA.setPassword((const char *)OTA_PASSWORD);

	//OTA
	ArduinoOTA.onStart([]() {
		Serial.println("Start OTA");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
}

void restart() {
	ESP.reset();
}

void setup() {
#ifdef SERIAL_STATUS_OUTPUT
	Serial.begin(115200);
#endif
	initialize_GPIO();
	
	wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);

	mqttClient.onConnect(onMqttConnect);
	mqttClient.onMessage(onMqttMessage);

	mqttClient.setServer(MQTT_SERVER_W12, MQTT_SERVERPORT_W12);
	snprintf(temp_text1, 30, "esp8266-%u", ESP.getChipId());	// TODO get hostname
	mqttClient.setKeepAlive(15).setCleanSession(false).setCredentials(MQTT_USERNAME_W12, MQTT_KEY_W12).setClientId(temp_text1);

	WiFi.mode(WIFI_STA);
	connectToWifi();
	initializeOta();
}

void loop() {
	ArduinoOTA.handle();
	yield();
}
