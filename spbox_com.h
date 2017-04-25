// spbox_com.h

#ifndef _SPBOX_COM_h
#define _SPBOX_COM_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>
#include <Time.h>
#include <TimeLib.h>
#include <NtpClientLib.h>

enum OTAModes { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, };

#endif
