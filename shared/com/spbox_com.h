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

#pragma once

#include "arduino.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Time.h>
#include <TimeLib.h>
#include <NtpClientLib.h>
#include <AsyncPing.h>
#include <AsyncMqttClient.h>
#include "spbox_conf.h"
#include "credentials.h"

class SPBOX_CONF; // forward decl
extern AsyncMqttClient mqttClient;

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
	void updateMqttCB();
	void checkMqtt();
	void setMqttAvailable(bool avail);
	bool getMqttAvailable();
	void disableMqtt();
	void enableMqtt();
	bool changeMqttBroker();

	void updatePingCB();
	void checkPing();

	void setInternetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time);
	bool getInternetAvailable();
	bool getAndClearInternetChanged();

	void setLocalnetAvailable(u16_t total_sent, u16_t total_recv, u32_t total_time);
	bool getLocalnetAvailable();
	bool getAndClearLocalnetChanged();

private:
	WiFiClient				client_;
	WiFiEventHandler		gotIpEventHandler_;
	WiFiEventHandler		disconnectedEventHandler_;
	AsyncPing				pingInet_, pingLocal_;
	IPAddress				pingAddressInet_, pingAddressLocal_;

	SPBOX_CONF				*conf_;
	bool					wlan_initialized_;
	bool					ota_initialized_;
	bool					doPing_;
	bool					internetAvailable_;
	bool					internetChanged_;
	bool					localnetAvailable_;
	bool					localnetChanged_;
	bool					doUpdateMqtt_;
	bool					mqttAvailable_;

	void onSTAGotIP(WiFiEventStationModeGotIP ipInfo);
	void onSTADisconnected(WiFiEventStationModeDisconnected event_info);
	void onNTPSyncEvent(NTPSyncEvent_t event);
	void onMqttConnect(bool sessionPresent);
	void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
	void onMqttSubscribe(uint16_t packetId, uint8_t qos);
	void onMqttUnsubscribe(uint16_t packetId);
	void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
	void onMqttPublish(uint16_t packetId);
	bool onPingInternet(const AsyncPingResponse& response);
	bool onPingLocalnet(const AsyncPingResponse& response);

protected:
};

extern SPBOX_COM com;
