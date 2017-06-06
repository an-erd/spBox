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

#include <EEPROM.h>
#include "spbox_conf.h"

#ifdef DEBUG_CONF
#define DEBUGLOG(...) DBG_PORT.printf(__VA_ARGS__)
#else
#define DEBUGLOG(...)
#endif

typedef union
{
	float floatvalue;
	char bytes[4];
} float_char_conversion;

SPBOX_CONF conf;

SPBOX_CONF::SPBOX_CONF()
{
}

void SPBOX_CONF::initialize(bool from_eeprom)
{
	bool result = false;

	DEBUGLOG("SPBOX_CONF::Initialize");
	if (from_eeprom) {
		result = readConfFromEEPROM();
	}
	if ((!from_eeprom) || (!result)) {
		//wlan_initialized_ = false;
		wlan_enabled_ = true;
		ota_mode_ = OTA_IDE;
		ntp_enabled_ = true;
		mqtt_enabled_ = true;
		accel_range_scale_ = MPU6050_ACCEL_FS_16;
		gyro_range_scale_ = MPU6050_GYRO_FS_2000;
		sea_level_pressure_ = 101325;
		mqtt_config_nr_ = 0;
		mqtt_send_healthdata_ = true;
		//accel_gyro_orientation_ = 0;	// TODO
		//mag_orientation_ = 0;		// TODO
	}
	writeConfToEEPROM();
}

void SPBOX_CONF::setWlanEnabled(bool wlan_enabled)
{
	wlan_enabled_ = wlan_enabled;
}

bool SPBOX_CONF::getWlanEnabled()
{
	return wlan_enabled_;
}

void SPBOX_CONF::setOtaMode(OTAModes_t ota_mode)
{
	ota_mode_ = ota_mode;
}

OTAModes_t SPBOX_CONF::getOtaMode()
{
	return ota_mode_;
}

void SPBOX_CONF::setNtpEnabled(bool ntp_enabled)
{
	ntp_enabled_ = ntp_enabled;
}

bool SPBOX_CONF::getNtpEnabled()
{
	return ntp_enabled_;
}

void SPBOX_CONF::setMQTTEnabled(bool mqtt_enabled)
{
	mqtt_enabled_ = mqtt_enabled;
}

bool SPBOX_CONF::getMQTTEnabled()
{
	return mqtt_enabled_;
}

void SPBOX_CONF::setMQTTHealthdata(bool mqtt_healthdata)
{
	mqtt_send_healthdata_ = mqtt_healthdata;
}

bool SPBOX_CONF::getMQTTHealthdata()
{
	return mqtt_send_healthdata_;
}

void SPBOX_CONF::setAccelRangeScale(uint8_t accel_range_scale)
{
	accel_range_scale_ = accel_range_scale;
}

void SPBOX_CONF::setGyroRangeScale(float gyro_range_scale)
{
	gyro_range_scale_ = gyro_range_scale;
}

uint8_t SPBOX_CONF::getAccelRangeScale()
{
	return accel_range_scale_;
}

uint8_t SPBOX_CONF::getGyroRangeScale()
{
	return gyro_range_scale_;
}

void SPBOX_CONF::setSeaLevelPressure(float seaLevelPressure)
{
	sea_level_pressure_ = seaLevelPressure;
}

float SPBOX_CONF::getSeaLevelPressure()
{
	return sea_level_pressure_;
}

void SPBOX_CONF::setMqttConfigNr(uint8_t nr)
{
	mqtt_config_nr_ = nr;
}

uint8_t SPBOX_CONF::getMqttConfigNr()
{
	return mqtt_config_nr_;
}

bool SPBOX_CONF::writeConfToEEPROM()
{
	DEBUGLOG("SPBOX_CONF::writeConfToEEPROM()\n");

	int i = 0;
	char seq;
	float_char_conversion temp;

	seq = EEPROM.read(i) + 1;
	EEPROM.write(i, seq); i++;

	DEBUGLOG("SPBOX_CONF::writeConfToEEPROM(), EEPROM seq %i written\n", seq);

	EEPROM.write(i, wlan_enabled_); i++;
	EEPROM.write(i, ota_mode_); i++;
	EEPROM.write(i, ntp_enabled_); i++;
	EEPROM.write(i, mqtt_enabled_); i++;
	EEPROM.write(i, accel_range_scale_); i++;
	EEPROM.write(i, gyro_range_scale_); i++;

	temp.floatvalue = sea_level_pressure_;
	EEPROM.write(i, temp.bytes[0]); i++;
	EEPROM.write(i, temp.bytes[1]); i++;
	EEPROM.write(i, temp.bytes[2]); i++;
	EEPROM.write(i, temp.bytes[3]); i++;

	EEPROM.write(i, mqtt_config_nr_); i++;
	EEPROM.write(i, mqtt_send_healthdata_); i++;

	return EEPROM.commit();
}

bool SPBOX_CONF::readConfFromEEPROM()
{
	DEBUGLOG("SPBOX_CONF::readConfFromEEPROM()\n");

	int i = 0;
	int seq;
	float_char_conversion temp;

	seq = EEPROM.read(i++);
	DEBUGLOG("SPBOX_CONF::readConfFromEEPROM(), EEPROM seq %i read\n", seq);
	if (seq == 0)
		return false;

	wlan_enabled_ = EEPROM.read(i++);
	ota_mode_ = (OTAModes_t)EEPROM.read(i++);
	ntp_enabled_ = EEPROM.read(i++);
	mqtt_enabled_ = EEPROM.read(i++);
	accel_range_scale_ = EEPROM.read(i++);
	gyro_range_scale_ = EEPROM.read(i++);

	temp.bytes[0] = EEPROM.read(i++);
	temp.bytes[1] = EEPROM.read(i++);
	temp.bytes[2] = EEPROM.read(i++);
	temp.bytes[3] = EEPROM.read(i++);
	sea_level_pressure_ = temp.floatvalue;

	mqtt_config_nr_ = EEPROM.read(i++);
	mqtt_send_healthdata_ = EEPROM.read(i++);

	return true;
}

bool SPBOX_CONF::clearEEPROM(char seq)
{
	DEBUGLOG("SPBOX_CONF::clearEEPROM()\n");
	for (int i = 0; i < 20; i++) { // 11 values + x free space
		EEPROM.write(i, 0);
	}
	return EEPROM.commit();
}

void SPBOX_CONF::printEEPROM(uint8_t len)
{
	DEBUGLOG("SPBOX_CONF::printEEPROM()\n");

	int i = 0;
	int seq;
	float_char_conversion temp;
	float result;
	uint8_t readInt;

	for (int i = 0; i < 20; i++) {
		readInt = EEPROM.read(i);
		DEBUGLOG("SPBOX_CONF::printEEPROM(), %i: %i\n", i, readInt);
	}

	//temp.bytes[0] = 128;
	//temp.bytes[1] = 230;
	//temp.bytes[2] = 197;
	//temp.bytes[3] = 71;
	//result = temp.floatvalue;
	//DEBUGLOG("SPBOX_CONF::printEEPROM(), sealevel %d\n", (long)result);
}