#ifndef _SPBOX_COM_h
#define _SPBOX_COM_h

#include "arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Time.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include "spbox_conf.h"
#include "credentials.h"

class SPBOX_CONF; // forward decl

class SPBOX_COM
{
public:
	SPBOX_COM();

	void setConf(SPBOX_CONF *conf);

	void initializeWlan();
	void disableWlan();
	void enableWlan();
	void initializeOta(OTAModes_t ota_mode = OTA_IDE);
	void checkOta();
	void initializeMQTT();
private:
	WiFiClient				client_;
	WiFiEventHandler		gotIpEventHandler_;
	WiFiEventHandler		disconnectedEventHandler_;

	SPBOX_CONF				*conf_;
	Adafruit_MQTT_Client	*mqtt_;
	Adafruit_MQTT_Publish	*battery_;
	bool					wlan_initialized_;
	bool					ota_initialized_;
	bool					mqtt_initialized_;

	void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
	void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
	void onNTPSyncEvent(NTPSyncEvent_t event);

protected:
};

//void update_mqtt_cb(void *arg) {
//	do_update_mqtt = true;
//}
//void setup_update_mqtt_timer()
//{
//	os_timer_disarm(&timer_update_mqtt);
//	os_timer_setfn(&timer_update_mqtt, (os_timer_func_t *)update_mqtt_cb, (void *)0);
//	os_timer_arm(&timer_update_mqtt, 10000, true);	// DELAY_MS_1MIN
//}

extern SPBOX_COM com;
#endif
