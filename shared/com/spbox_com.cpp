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
#include "spbox_display.h"

//#define DBG_PORT Serial
//#define DEBUG_COM

#ifdef DEBUG_COM
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

const char* otaErrorNames[] = {
	"Auth Failed",		// OTA_AUTH_ERROR
	"Begin Failed",		// OTA_BEGIN_ERROR
	"Connect Failed",	// OTA_CONNECT_ERROR
	"Receive Failed",	// OTA_RECEIVE_ERROR
	"End Failed",		// OTA_END_ERROR
};

// regular ping to check availale internet connection
LOCAL os_timer_t timerPing;
void updatePing_CB(void *arc) { com.updatePingCB(); }

// regular MQTT publish on health data
LOCAL os_timer_t timerUpdateMqtt;
void updateMQtt_CB(void *arc) { com.updateMqttCB(); }

AsyncMqttClient mqttClient;

SPBOX_COM::SPBOX_COM() {}

void SPBOX_COM::setConf(SPBOX_CONF *conf) { conf_ = conf; }

void SPBOX_COM::initialize()
{
	// Timer
	os_timer_disarm(&timerPing);
	os_timer_setfn(&timerPing, (os_timer_func_t *)updatePing_CB, (void *)0);

	os_timer_disarm(&timerUpdateMqtt);
	os_timer_setfn(&timerUpdateMqtt, (os_timer_func_t *)updateMQtt_CB, (void *)0);
	os_timer_arm(&timerUpdateMqtt, MQTT_HEALTHDATA_INTERVALL, true);

	// WLAN
	gotIpEventHandler_ = WiFi.onStationModeGotIP(std::bind(&SPBOX_COM::onSTAGotIP, this, std::placeholders::_1));
	disconnectedEventHandler_ = WiFi.onStationModeDisconnected(std::bind(&SPBOX_COM::onSTADisconnected, this, std::placeholders::_1));
	// NTP
	NTP.onNTPSyncEvent(std::bind(&SPBOX_COM::onNTPSyncEvent, this, std::placeholders::_1));

	// Initialize PING
	IPAddress addr_Inet(8, 8, 8, 8);		// google.com
	pingAddressInet_ = addr_Inet;
	pingInet_.on(false, std::bind(&SPBOX_COM::onPingInternet, this, std::placeholders::_1));

	IPAddress addr_Local(192, 168, 2, 137);	// local MQTT broker
	pingAddressLocal_ = addr_Local;
	pingLocal_.on(false, std::bind(&SPBOX_COM::onPingLocalnet, this, std::placeholders::_1));

	// MQTT
	mqttClient.onConnect(std::bind(&SPBOX_COM::onMqttConnect, this, std::placeholders::_1));
	mqttClient.onDisconnect(std::bind(&SPBOX_COM::onMqttDisconnect, this, std::placeholders::_1));
	mqttClient.onSubscribe(std::bind(&SPBOX_COM::onMqttSubscribe, this, std::placeholders::_1, std::placeholders::_2));
	mqttClient.onUnsubscribe(std::bind(&SPBOX_COM::onMqttUnsubscribe, this, std::placeholders::_1));
	mqttClient.onMessage(std::bind(&SPBOX_COM::onMqttMessage, this, std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
	mqttClient.onPublish(std::bind(&SPBOX_COM::onMqttPublish, this, std::placeholders::_1));
}

void SPBOX_COM::initializeWlan()
{
	DEBUGLOG("Initializing WLAN\r\n");
	if (wlan_initialized_) {
		DEBUGLOG("WLAN already initialized\r\n");
		return;
	}

	if (conf_->getWlanEnabled()) {
		WiFi.mode(WIFI_STA);
		WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
	}

	wlan_initialized_ = true;
	DEBUGLOG("Initializing WLAN done\r\n");
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

		ArduinoOTA.onStart(std::bind(&SPBOX_COM::onOtaStart, this));
		ArduinoOTA.onEnd(std::bind(&SPBOX_COM::onOtaEnd, this));
		ArduinoOTA.onProgress(std::bind(&SPBOX_COM::onOtaProgress, this, std::placeholders::_1, std::placeholders::_2));
		ArduinoOTA.onError(std::bind(&SPBOX_COM::onOtaError, this, std::placeholders::_1));

		ArduinoOTA.begin();
		ota_initialized_ = true;
		otaUpdate_.otaUpdateStarted_ = false;
	}
}

void SPBOX_COM::checkOta()
{
	if (conf_->getOtaMode() == OTA_IDE) {
		ArduinoOTA.handle();
	}
}

void SPBOX_COM::initializeMQTT()
{
	DEBUGLOG("Initialize MQTT\r\n");

	cntMqttSent_ = cntMqttRecv_ = 0;

	mqttClient.setServer(mqttConfigs[conf.getMqttConfigNr()].name, mqttConfigs[conf.getMqttConfigNr()].port);
	mqttClient.setKeepAlive(15).setCleanSession(false).setCredentials(mqttConfigs[conf.getMqttConfigNr()].uid, mqttConfigs[conf.getMqttConfigNr()].key);
	//mqttClient.setWill();
	//mqttLastWillSet_ = true;

	DEBUGLOG("Initialize MQTT done\r\n");
}

void SPBOX_COM::updateMqttCB()
{
	doUpdateMqtt_ = true;
}

void SPBOX_COM::checkMqttContent()
{
	if (!doUpdateMqtt_)
		return;

	doUpdateMqtt_ = false;

	if (!mqttClient.connected())
		return;

	if (conf.getMQTTHealthdata()) {
		uint16_t vbatADC;
		char text[7];

		vbatADC = analogRead(VBAT_PIN);
		DEBUGLOG("checkMqttContent, send healthdata, vbat:  %d\n", vbatADC);
		snprintf(text, 7, "%d", vbatADC);
		mqttClient.publish("andreaserd/feeds/battery", 0, true, text);
		cntMqttSent_++;
	}
}

void SPBOX_COM::setMqttAvailable(bool avail)
{
	mqttAvailable_ = avail;
}

bool SPBOX_COM::getMqttAvailable()
{
	return mqttAvailable_;
}

void SPBOX_COM::disableMqtt()
{
	DEBUGLOG("Disable MQTT\r\n");
	conf_->setMQTTEnabled(false);
	checkMqttConnection();
}

void SPBOX_COM::enableMqtt()
{
	DEBUGLOG("Enable MQTT\r\n");
	conf_->setMQTTEnabled(true);
	checkMqttConnection();
}

void SPBOX_COM::checkMqttConnection(bool now)
{
	static uint16_t lastMillis = 0;
	uint16_t diff = millis() - lastMillis;

	// DEBUGLOG("checkMqttConnection()\n");

	if (!now && diff < MQTT_CONNECT_INTERVALL)
		return;

	lastMillis = millis();

	if (conf_->getMQTTEnabled()) {
		if (!mqttClient.connected()) {
			switch (mqttConfigs[conf_->getMqttConfigNr()].inet) {
			case true:	// inet for broker needed and available
				if (getInternetAvailable())
					mqttClient.connect();
				break;
			case false:	// localnet for broker needed and available
				if (getLocalnetAvailable())
					mqttClient.connect();
			default:
				break;
			}
		}
	}
	else {
		if (mqttClient.connected())
			mqttClient.disconnect();
	}
}

void SPBOX_COM::changeMqttBroker()
{
	bool wasConnected = mqttClient.connected();

	mqttClient.disconnect();
	mqttClient.setServer(mqttConfigs[conf.getMqttConfigNr()].name, mqttConfigs[conf.getMqttConfigNr()].port);
	mqttClient.setKeepAlive(15).setCleanSession(false).setCredentials(mqttConfigs[conf.getMqttConfigNr()].uid, mqttConfigs[conf.getMqttConfigNr()].key);
	if (wasConnected)
		checkMqttConnection(true);
}

void SPBOX_COM::updatePingCB()
{
	doPing_ = true;
}

void SPBOX_COM::checkPing()
{
	if (wlan_initialized_ && doPing_) {
		doPing_ = false;
		pingInet_.begin(pingAddressInet_, 1, 300);
		pingLocal_.begin(pingAddressLocal_, 1, 300);
	}
}

void SPBOX_COM::setInternetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time)
{
	bool internetAvail = total_sent == total_recv;

	//DEBUGLOG("ping inet send %i, recv %i, %i time\n", total_sent, total_recv, total_time);

	if (internetAvail == internetAvailable_)
		return;

	if (internetAvail) {
		DEBUGLOG("Inet available, time %d\n", total_time);
		NTP.begin("pool.ntp.org", 1, true);
	}
	else {
		DEBUGLOG("Inet not available\n");
		NTP.stop(); // NTP sync can be disabled to avoid sync errors
		if (mqttConfigs[conf_->getMqttConfigNr()].inet)
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

void SPBOX_COM::setLocalnetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time)
{
	bool localnetAvail = total_sent == total_recv;
	if (localnetAvail == localnetAvailable_)
		return;

	if (localnetAvail) {
		DEBUGLOG("Localnet available, time %d\n", total_time);
	}
	else {
		DEBUGLOG("Localnet not available\n");
		mqttClient.disconnect();
	}

	localnetAvailable_ = localnetAvail;
	localnetChanged_ = true;
}

bool SPBOX_COM::getLocalnetAvailable()
{
	return localnetAvailable_;
}

bool SPBOX_COM::getAndClearLocalnetChanged()
{
	if (!localnetChanged_)
		return false;

	localnetChanged_ = false;
	return true;
}

void SPBOX_COM::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
	DEBUGLOG("Got IP: %s\r\n", ipInfo.ip.toString().c_str());

	os_timer_disarm(&timerPing);
	os_timer_arm(&timerPing, DELAY_MS_TWOSEC, true);
}
void SPBOX_COM::onSTADisconnected(WiFiEventStationModeDisconnected event_info)
{
	DEBUGLOG("Disconnected from SSID: %s, Reason: %d\n", event_info.ssid.c_str(), event_info.reason);

	os_timer_disarm(&timerPing);
	pingInet_.cancel();
	pingLocal_.cancel();
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
void SPBOX_COM::onMqttConnect(bool sessionPresent)
{
	DEBUGLOG("MQTT, connected to broker, session present %i\r\n", sessionPresent);
	setMqttAvailable(true);
	cntMqttSubscribed_ = 0;
	mqttConnSince_ = NTP.getTimeDateString();

	uint16_t packetIdSub = mqttClient.subscribe("andreaserd/feeds/battery", 0);
	Serial.print("Subscribing at QoS 0, packetId: ");
	Serial.println(packetIdSub);

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
void SPBOX_COM::onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
	DEBUGLOG("MQTT, disconnected from broker, reason %i\r\n", (int)reason);
	setMqttAvailable(false);
	cntMqttSubscribed_ = 0;
	mqttConnSince_ = "---";
}
void SPBOX_COM::onMqttSubscribe(uint16_t packetId, uint8_t qos) {
	DEBUGLOG("MQTT, Subscribe acknowledged, packetId: %i, qos: %i\n", packetId, qos);
	cntMqttSubscribed_++;
}
void SPBOX_COM::onMqttUnsubscribe(uint16_t packetId) {
	DEBUGLOG("MQTT, Unsubscribe acknowledged, packetId: %i\n", packetId);
	cntMqttSubscribed_--;
}
void SPBOX_COM::onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
	DEBUGLOG("MQTT, Publish received, topic %i, qos %i, dup %i, retain %i, len %i, index %i, total %i, payload %s\n",
		topic, properties.qos, properties.dup, properties.retain, len, index, total, payload);

	cntMqttRecv_++;
}
void SPBOX_COM::onMqttPublish(uint16_t packetId) {
	DEBUGLOG("MQTT, Publish acknowledged, packetId: %i\n", packetId);
	cntMqttSent_++;
}
bool SPBOX_COM::onPingInternet(const AsyncPingResponse& response) {
	//DEBUGLOG("ping inet,  send %i, recv %i, time %i\n", response.total_sent, response.total_recv, response.total_time);
	setInternetAvailable(response.total_sent, response.total_recv, response.total_time);
}
bool SPBOX_COM::onPingLocalnet(const AsyncPingResponse& response) {
	//DEBUGLOG("ping local, send %i, recv %i, time %i\n", response.total_sent, response.total_recv, response.total_time);
	setLocalnetAvailable(response.total_sent, response.total_recv, response.total_time);
}

void SPBOX_COM::onOtaStart()
{
	display.ssd1306_command(SSD1306_DISPLAYON);
	DEBUGLOG("OTA, onOtaStart()\n");

	otaUpdate_.otaUpdateStarted_ = true;
	otaUpdate_.otaUpdateEnd_ = false;
	otaUpdate_.otaUpdateError_ = false;
	otaUpdate_.otaUpdateProgress_ = 0;
	display.updateDisplayScr15(com.getOtaUpdate(), true);
}

void SPBOX_COM::onOtaEnd()
{
	DEBUGLOG("OTA, onOtaEnd()\n");
	otaUpdate_.otaUpdateEnd_ = true;
	display.updateDisplayScr15(com.getOtaUpdate(), true);
}

void SPBOX_COM::onOtaProgress(unsigned int progress, unsigned int total)
{
	unsigned int length = (progress / (total / 127));		// width of progress bar = 128 px

	otaUpdate_.otaUpdateProgress_ = length;

	DEBUGLOG("Progress: progress: %u, total: %u, length: %u\n", progress, total, otaUpdate_.otaUpdateProgress_);

	display.updateDisplayScr15(com.getOtaUpdate(), false);
}

void SPBOX_COM::onOtaError(ota_error_t error)
{
	DEBUGLOG("Arduino OTA Error[%u]: ", otaErrorNames[error]);
	otaUpdate_.otaUpdateError_ = false;
	otaUpdate_.otaUpdateErrorNr_ = error;
	display.updateDisplayScr15(com.getOtaUpdate(), true);
}

SPBOX_COM com;