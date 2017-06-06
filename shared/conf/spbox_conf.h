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

#include <MPU6050.h>
#include "myconfig.h"
#include "spbox_com.h"

class SPBOX_CONF
{
public:
	SPBOX_CONF();
	void initialize(bool from_eeprom = false);

	void setWlanEnabled(bool wlan_enabled = true);
	bool getWlanEnabled();

	void setOtaMode(OTAModes_t ota_mode = OTA_IDE);
	OTAModes_t getOtaMode();

	void setNtpEnabled(bool ntp_enabled = true);
	bool getNtpEnabled();

	void setMQTTEnabled(bool mqtt_enabled = true);
	bool getMQTTEnabled();

	void setMQTTHealthdata(bool mqtt_healthdata = true);
	bool getMQTTHealthdata();

	void setAccelRangeScale(uint8_t accel_range_scale = MPU6050_ACCEL_FS_16);
	void setGyroRangeScale(float gyro_range_scale = MPU6050_GYRO_FS_2000);
	uint8_t getAccelRangeScale();
	uint8_t getGyroRangeScale();

	void setSeaLevelPressure(float seaLevelPressure);
	float getSeaLevelPressure();
	void setMqttConfigNr(uint8_t nr);
	uint8_t getMqttConfigNr();

	//	void setAccelGyroOrientation(uint8_t accel_gyro_orientation = 0);

	bool writeConfToEEPROM();
	bool readConfFromEEPROM();
	bool clearEEPROM(char seq = 0);
	void printEEPROM(uint8_t len);

private:
	bool		wlan_enabled_;
	OTAModes_t	ota_mode_;
	bool		ntp_enabled_;
	bool		mqtt_enabled_;
	bool		mqtt_send_healthdata_;
	uint8_t		accel_range_scale_;
	uint8_t		gyro_range_scale_;
	float		sea_level_pressure_;
	uint8_t		mqtt_config_nr_;
	//	uint8_t		accel_gyro_orientation_;
	//	uint8_t		mag_orientation_;
protected:
};

extern SPBOX_CONF conf;
