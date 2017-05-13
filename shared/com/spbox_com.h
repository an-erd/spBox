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
#include "AsyncPing.h"
#include "spbox_conf.h"
#include "credentials.h"

class SPBOX_CONF; // forward decl

class SPBOX_COM
{
public:
	SPBOX_COM();

	void setConf(SPBOX_CONF *conf);
	void initialize();
	void initializeWlan();
	void disableWlan();
	void enableWlan();
	void initializeOta(OTAModes_t ota_mode = OTA_IDE);
	void checkOta();
	void initializeMQTT();
	void updatePingCB();
	void checkPing();
	void setInternetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time);
	bool getInternetAvailalbe();
	bool getAndClearInternetChanged();
private:
	WiFiClient				client_;
	WiFiEventHandler		gotIpEventHandler_;
	WiFiEventHandler		disconnectedEventHandler_;
	AsyncPing				ping_;
	IPAddress				pingAddress_;

	SPBOX_CONF				*conf_;
	Adafruit_MQTT_Client	*mqtt_;
	Adafruit_MQTT_Publish	*battery_;
	bool					wlan_initialized_;
	bool					ota_initialized_;
	bool					mqtt_initialized_;
	bool					doPing_;
	bool					internetAvailable_;
	bool					internetChanged_;

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
