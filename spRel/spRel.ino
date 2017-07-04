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

#include <Arduino.h>
#include <wire.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "myconfig.h"
#include "button.h"
#include "credentials.h"

LOCAL os_timer_t timerRestart;
AsyncMqttClient mqttClient;
Ticker ticker;
bool relais_state;
char temp_text1[30];
char temp_text2[30];

void onMqttConnect(bool sessionPresent) {
	Serial.println("** Connected to the broker **");
	Serial.print("Session present: ");
	Serial.println(sessionPresent);
	uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
	Serial.print("Subscribing at QoS 2, packetId: ");
	Serial.println(packetIdSub);
	mqttClient.publish("test/lol", 0, true, "test 1");
	Serial.println("Publishing at QoS 0");
	uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
	Serial.print("Publishing at QoS 1, packetId: ");
	Serial.println(packetIdPub1);
	uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
	Serial.print("Publishing at QoS 2, packetId: ");
	Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
	Serial.println("** Disconnected from the broker **");
	Serial.println("Reconnecting to MQTT...");
	mqttClient.connect();
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
	Serial.println("** Subscribe acknowledged **");
	Serial.print("  packetId: ");
	Serial.println(packetId);
	Serial.print("  qos: ");
	Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
	Serial.println("** Unsubscribe acknowledged **");
	Serial.print("  packetId: ");
	Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
	Serial.println("** Publish received **");
	Serial.print("  topic: ");
	Serial.println(topic);
	Serial.print("  qos: ");
	Serial.println(properties.qos);
	Serial.print("  dup: ");
	Serial.println(properties.dup);
	Serial.print("  retain: ");
	Serial.println(properties.retain);
	Serial.print("  len: ");
	Serial.println(len);
	Serial.print("  index: ");
	Serial.println(index);
	Serial.print("  total: ");
	Serial.println(total);
}

void onMqttPublish(uint16_t packetId) {
	Serial.println("** Publish acknowledged **");
	Serial.print("  packetId: ");
	Serial.println(packetId);
}

void initialize_GPIO() {
	pinMode(PIN_LED_G, OUTPUT);
	digitalWrite(PIN_LED_G, HIGH);	// led is active low

	pinMode(PIN_RELAIS, OUTPUT);
	relais_state = LOW;
	digitalWrite(PIN_RELAIS, relais_state);	 // relais is active high
}

void tick() {
	int state = digitalRead(PIN_LED_G);
	digitalWrite(PIN_LED_G, !state);
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
	Wire.begin();
	initialize_GPIO();

	ticker.attach(0.2, tick);

	WiFi.mode(WIFI_STA);
	WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected");  
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	ticker.detach();
	digitalWrite(PIN_LED_G, true); // active low

	initializeOta();

	mqttClient.setServer(MQTT_SERVER_W12, MQTT_SERVERPORT_W12);
	snprintf(temp_text1, 30, "esp8266-%u", ESP.getChipId());	// TODO get hostname
	mqttClient.setKeepAlive(15).setCleanSession(false).setCredentials(MQTT_USERNAME_W12, MQTT_KEY_W12).setClientId(temp_text1);
		
	button.initialize();
	button.start();
	button.onButtonChangeEvent([](buttonChangeEvent_t e) {
		switch (e) {
		case H_L_SHORT:
			relais_state = !relais_state;
			digitalWrite(PIN_RELAIS, relais_state);
			snprintf(temp_text2, 30, "%d", (int)relais_state);
			mqttClient.publish("andreaserd/feeds/sonoff", 0, true, temp_text2);
			break;
		case H_L_LONG:
			relais_state = false;
			digitalWrite(PIN_RELAIS, relais_state);
			snprintf(temp_text2, 30, "%d", (int)relais_state);
			mqttClient.publish("andreaserd/feeds/sonoff", 0, true, temp_text2);
			os_timer_disarm(&timerRestart);
			os_timer_setfn(&timerRestart, (os_timer_func_t *)restart, (void *)0);
			os_timer_arm(&timerRestart, 3000, true);
			break;
		case H_L_VERYLONG:
			break;
		default:
			break;
		}
	});
}

void loop() {
	button.check();
	ArduinoOTA.handle();
	yield();
}