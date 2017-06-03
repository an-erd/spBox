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

#include "spbox_com.h"

#ifdef DEBUG_COM
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

// regular ping to check availale internet connection
LOCAL os_timer_t timerPing;
void updatePing_CB(void *arc) { com.updatePingCB(); }

// regular MQTT publish on health data
LOCAL os_timer_t timerUpdateMqtt;
void updateMQtt_CB(void *arc) { com.updateMqttCB(); }

AsyncMqttClient mqttClient;

void onMqttConnect(bool sessionPresent) {
	//Serial.println("** Connected to the broker **");
	//Serial.print("Session present: ");
	Serial.println(sessionPresent);
	com.setMqttAvailable(true);

	//uint16_t packetIdSub = mqttClient.subscribe("andreaserd/feeds/battery", 2);
	//Serial.print("Subscribing at QoS 2, packetId: ");
	//Serial.println(packetIdSub);

	//mqttClient.publish("/feeds/battery", 0, true, "111");
	//Serial.println("Publishing at QoS 0");

	//uint16_t packetIdPub1 = mqttClient.publish("/feeds/battery", 1, true, "222");
	//Serial.print("Publishing at QoS 1, packetId: ");
	//Serial.println(packetIdPub1);

	//uint16_t packetIdPub2 = mqttClient.publish("/feeds/battery", 2, true, "333");
	//Serial.print("Publishing at QoS 2, packetId: ");
	//Serial.println(packetIdPub2);
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
	//Serial.println("** Disconnected from the broker **");
	//Serial.print("Reason: "); Serial.println((int)reason);
	com.setMqttAvailable(false);
	//Serial.println("Reconnecting to MQTT...");
	//mqttClient.connect();
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

SPBOX_COM::SPBOX_COM()
{
}

void SPBOX_COM::setConf(SPBOX_CONF *conf)
{
	conf_ = conf;
}

void SPBOX_COM::initialize()
{
	os_timer_disarm(&timerPing);
	os_timer_setfn(&timerPing, (os_timer_func_t *)updatePing_CB, (void *)0);

	os_timer_disarm(&timerUpdateMqtt);
	os_timer_setfn(&timerUpdateMqtt, (os_timer_func_t *)updateMQtt_CB, (void *)0);
	os_timer_arm(&timerUpdateMqtt, DELAY_MS_TENSEC, true);
}

void SPBOX_COM::initializeWlan()
{
	const char *ips[] = { "8.8.8.8" };	// google.com

	DEBUGLOG("Initializing WLAN\r\n");
	if (wlan_initialized_) {
		DEBUGLOG("WLAN already initialized\r\n");
		return;
	}

	if (conf_->getWlanEnabled()) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
#ifdef DEBUG_COM
		WiFi.onEvent([](WiFiEvent_t e) {
			Serial.printf("Event wifi -----> %d\n", e);
		});
#endif

		gotIpEventHandler_ = WiFi.onStationModeGotIP(
			std::bind(&SPBOX_COM::onSTAGotIP, this, std::placeholders::_1));
		disconnectedEventHandler_ = WiFi.onStationModeDisconnected(
			std::bind(&SPBOX_COM::onSTADisconnected, this, std::placeholders::_1));
		NTP.onNTPSyncEvent(
			std::bind(&SPBOX_COM::onNTPSyncEvent, this, std::placeholders::_1));

		// Initialize PING
		IPAddress addr(8, 8, 8, 8);
		pingAddress_ = addr;
		ping_.on(false, [](const AsyncPingResponse& response) {
			com.setInternetAvailable(response.total_sent, response.total_recv, response.total_time);
			return true;
		});
	}

	wlan_initialized_ = true;
}

void SPBOX_COM::disableWlan()
{
	WiFi.mode(WIFI_OFF);
	DEBUGLOG("WLAN disabled\r\n");
	conf_->setWlanEnabled(false);
}

void SPBOX_COM::enableWlan()
{
	if (!wlan_initialized_) {
		initializeWlan();
	}
	else {
		WiFi.mode(WIFI_STA);
		WiFi.begin();
		DEBUGLOG("WLAN enabled\r\n");
		conf_->setWlanEnabled(true);
	}
}

void SPBOX_COM::initializeOta(OTAModes_t ota_mode)
{
	if (conf_->getOtaMode() == OTA_OFF)
		return;

	if (conf_->getOtaMode() == OTA_IDE) {
		// ArduinoOTA.setPort(8266);
		// ArduinoOTA.setHostname("esp8266-XXX");
		// ArduinoOTA.setPassword((const char *)"123");

#ifdef DEBUG_COM
		ArduinoOTA.onStart([]() { Serial.println("Arduino OTA Start"); });
		ArduinoOTA.onEnd([]() { Serial.println("\nArduino OTA End"); });
		ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
			Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
		ArduinoOTA.onError([](ota_error_t error) {
			Serial.printf("Arduino OTA Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR) Serial.println("End Failed");
		});
#endif
		ArduinoOTA.begin();
		ota_initialized_ = true;
	}
}

void SPBOX_COM::checkOta()
{
	DEBUGLOG("checkOta()\r\n");
	if (conf_->getOtaMode() == OTA_IDE) {
		ArduinoOTA.handle();
	}
}

void SPBOX_COM::initializeMQTT()
{
	DEBUGLOG("Initialize MQTT\r\n");

	mqttClient.onConnect(onMqttConnect);
	mqttClient.onDisconnect(onMqttDisconnect);
	mqttClient.onSubscribe(onMqttSubscribe);
	mqttClient.onUnsubscribe(onMqttUnsubscribe);
	mqttClient.onMessage(onMqttMessage);
	mqttClient.onPublish(onMqttPublish);

	mqttClient.setServer(AIO_SERVER, AIO_SERVERPORT);
	mqttClient.setKeepAlive(15).setCleanSession(false).setCredentials(AIO_USERNAME, AIO_KEY);
	mqttClient.disconnect();
	DEBUGLOG("Initialize MQTT done\r\n");
}

void SPBOX_COM::updatePingCB()
{
	doPing_ = true;
}

void SPBOX_COM::checkPing()
{
	if (wlan_initialized_ && doPing_) {
		doPing_ = false;
		ping_.begin(pingAddress_, 1, 300);
	}
}

void SPBOX_COM::setInternetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time)
{
	bool internetAvail = total_sent == total_recv;
	if (internetAvail == internetAvailable_)
		return;

	if (internetAvail) {
		//Serial.println("Inet available");
		NTP.begin("pool.ntp.org", 1, true);
		mqttClient.connect();
	}
	else {
		//Serial.println("Inet not available");
		NTP.stop(); // NTP sync can be disabled to avoid sync errors
		mqttClient.disconnect();
	}

	internetAvailable_ = internetAvail;
	internetChanged_ = true;
}

bool SPBOX_COM::getInternetAvailable()
{
	return internetAvailable_;
}

bool SPBOX_COM::getAndClearInternetChanged()
{
	if (!internetChanged_)
		return false;

	internetChanged_ = false;
	return true;
}

void SPBOX_COM::updateMqttCB()
{
	doUpdateMqtt_ = true;
}

void SPBOX_COM::checkMqtt()
{
	if (!doUpdateMqtt_)
		return;
	doUpdateMqtt_ = false;

	if (!mqttClient.connected()) {
		if (internetAvailable_)
			mqttClient.connect();
		return;
	}

	//Serial.println("updateMqtt: ");
	uint16_t vbatADC;
	char text[7];

	vbatADC = analogRead(VBAT_PIN);
	//Serial.println(vbatADC);
	snprintf(text, 7, "%d", vbatADC);

	mqttClient.publish("andreaserd/feeds/battery", 0, true, text);
	//Serial.println("Publishing at QoS 0");
}

void SPBOX_COM::setMqttAvailable(bool avail)
{
	mqttAvailable_ = avail;
}

bool SPBOX_COM::getMqttAvailable()
{
	return mqttAvailable_;
}

void SPBOX_COM::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
	DEBUGLOG("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
#ifdef DEBUG_COM
	Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
#endif

	os_timer_disarm(&timerPing);
	os_timer_arm(&timerPing, DELAY_MS_TENSEC, true);
}

void SPBOX_COM::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
	DEBUGLOG("Disconnected from SSID: %s, Reason: %d\n", event_info.ssid.c_str(), event_info.reason);
#ifdef DEBUG_COM
	Serial.printf("Disconnected from SSID: %s\n", event_info.ssid.c_str());
	Serial.printf("Reason: %d\n", event_info.reason);
#endif

	os_timer_disarm(&timerPing);
	ping_.cancel();
}

void SPBOX_COM::onNTPSyncEvent(NTPSyncEvent_t event)
{
#ifdef DEBUG_COM
	Serial.printf("NTP Sync Event: ");
	if (event) {
		Serial.print("Time Sync error: ");
		if (event == noResponse)
			Serial.println("NTP server not reachable");
		else if (event == invalidAddress)
			Serial.println("Invalid NTP server address");
	}
	else {
		Serial.print("Got NTP time: ");
		Serial.println(NTP.getTimeDateString(NTP.getLastNTPSync()));
	}
#endif
}

SPBOX_COM com;