#include "spbox_com.h"

#ifdef DEBUG_COM
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

LOCAL os_timer_t timerPing;					// regular ping
void updatePing_CB(void *arc) {
	com.updatePingCB();
}

//Adafruit_MQTT_Client	mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
//Adafruit_MQTT_Publish	battery = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/battery");
//LOCAL os_timer_t timer_update_mqtt;

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
}

void SPBOX_COM::initializeWlan()
{
	const char *ips[] = { "8.8.8.8" };

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

		//ping_.on(true, [](const AsyncPingResponse& response) {
		//	IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
		//	if (response.answer)
		//		Serial.printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%d ms\n", response.size, addr.toString().c_str(), response.icmp_seq, response.ttl, response.time);
		//	else
		//		Serial.printf("no answer yet for %s icmp_seq=%d\n", addr.toString().c_str(), response.icmp_seq);
		//	return false; //do not stop
		//});
		ping_.on(false, [](const AsyncPingResponse& response) {
			//IPAddress addr(response.addr); //to prevent with no const toString() in 2.3.0
			//Serial.printf("total answer from %s sent %d recevied %d time %d ms\n", addr.toString().c_str(), response.total_sent, response.total_recv, response.total_time);
			//if (response.mac)
			//	Serial.printf("detected eth address " MACSTR "\n", MAC2STR(response.mac->addr));
			com.setInternetAvailable(response.total_sent, response.total_recv, response.total_time);
			Serial.print("Internet available:"); Serial.println(com.getInternetAvailalbe() ? "Y" : "N");

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

	//mqtt_ = new Adafruit_MQTT_Client(&client_, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
	//battery_ = new Adafruit_MQTT_Publish(&mqtt_, AIO_USERNAME "/feeds/battery");

	//int8_t ret;

	//while ((ret = mqtt_->connect()) != 0) {
	//	switch (ret) {
	//	case 1: Serial.println(F("Wrong protocol")); break;
	//	case 2: Serial.println(F("ID rejected")); break;
	//	case 3: Serial.println(F("Server unavail")); break;
	//	case 4: Serial.println(F("Bad user/pass")); break;
	//	case 5: Serial.println(F("Not authed")); break;
	//	case 6: Serial.println(F("Failed to subscribe")); break;
	//	default: Serial.println(F("Connection failed")); break;
	//	}

	//	if (ret >= 0)
	//		mqtt_->disconnect();

	//	DEBUGLOG("MQTT: Retrying connection... \r\n");
	//}
	//DEBUGLOG("MQTT: Connected!\r\n");
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
		NTP.begin("pool.ntp.org", 1, true);
	}
	else {
		NTP.stop(); // NTP sync can be disabled to avoid sync errors
	}

	internetAvailable_ = internetAvail;
	internetChanged_ = true;
}

bool SPBOX_COM::getInternetAvailalbe()
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

void SPBOX_COM::onSTAGotIP(WiFiEventStationModeGotIP ipInfo)
{
	DEBUGLOG("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
#ifdef DEBUG_COM
	Serial.printf("Got IP: %s\r\n", ipInfo.ip.toString().c_str());
#endif
	NTP.begin("pool.ntp.org", 1, true);
	NTP.setInterval(5, 30);

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

//volatile bool	do_update_mqtt;
//
//void check_mqtt()
//{
//	if (!do_update_mqtt)
//		return;
//
//	do_update_mqtt = false;
//
//	updateVbat();
//
//	if (!battery.publish(g_vbatADC)) {
//#ifdef SERIAL_STATUS_OUTPUT
//		Serial.println(F("Update vbat Failed."));
//#endif
//	}
//	else {
//#ifdef SERIAL_STATUS_OUTPUT
//		Serial.println(F("Update vbat Success!"));
//#endif
//	}
//}