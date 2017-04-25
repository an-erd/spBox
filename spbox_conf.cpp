//
//
//

#include <EEPROM.h>
#include "spbox_conf.h"

typedef union
{
	float floatvalue;
	char bytes[4];
} float_char_conversion;

SPBOX_CONF::SPBOX_CONF()
{
}

void SPBOX_CONF::initialize(bool from_eeprom)
{
	if (from_eeprom) {
		readConfFromEEPROM();
		return;
	}

	wlan_initialized_ = false;
	wlan_enabled_ = true;
	ota_mode_ = OTA_IDE;
	ntp_enabled_ = true;
	aio_enabled_ = true;
	accel_range_scale_ = MPU6050_ACCEL_FS_16;
	gyro_range_scale_ = MPU6050_GYRO_FS_2000;
	//accel_gyro_orientation_ = 0;	// TODO
	//mag_orientation_ = 0;		// TODO
	//use_configured_sea_level_ = false;
	//sea_level_pressure_ = 101325;
}

void SPBOX_CONF::setWlanInitialized(bool wlan_initialized)
{
	wlan_initialized_ = wlan_initialized;
}

bool SPBOX_CONF::getWlanInitialized()
{
	return wlan_initialized_;
}

void SPBOX_CONF::setWlanEnabled(bool wlan_enabled)
{
	wlan_enabled_ = wlan_enabled;
}

bool SPBOX_CONF::getWlanEnabled()
{
	return wlan_enabled_;
}

void SPBOX_CONF::setOtaMode(OTAModes ota_mode_)
{
	ota_mode_ = ota_mode;
}

OTAModes SPBOX_CONF::getOtaMode()
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

void SPBOX_CONF::setAioEnabled(bool aio_enabled)
{
	aio_enabled_ = aio_enabled;
}

bool SPBOX_CONF::getAioEnabled()
{
	return aio_enabled_;
}

void SPBOX_CONF::setAccelRangeScale(uint8_t accel_range_scale)
{
	accel_range_scale_ = accel_range_scale;
}

void SPBOX_CONF::setGyroRangeScale(float gyro_range_scale)
{
	gyro_range_scale_ = gyro_range_scale;
}

int SPBOX_CONF::writeConfToEEPROM()
{
	int i = 0;
	char seq;
	float_char_conversion temp;

	seq = EEPROM.read(i) + 1;
	EEPROM.write(i, seq); i++;

	EEPROM.write(i, wlan_initialized_); i++;
	EEPROM.write(i, wlan_enabled_); i++;
	EEPROM.write(i, ota_mode_); i++;
	EEPROM.write(i, ntp_enabled_); i++;
	EEPROM.write(i, aio_enabled_); i++;
	EEPROM.write(i, accel_range_scale_); i++;

	temp.floatvalue = gyro_range_scale_;
	EEPROM.write(i, temp.bytes[0]); i++;
	EEPROM.write(i, temp.bytes[0]); i++;
	EEPROM.write(i, temp.bytes[0]); i++;
	EEPROM.write(i, temp.bytes[0]); i++;

	return EEPROM.commit();
}

void SPBOX_CONF::readConfFromEEPROM()
{
	int i = 0;
	int seq;
	float_char_conversion temp;

	seq = EEPROM.read(i++);
	wlan_initialized_ = EEPROM.read(i++);
	wlan_enabled_ = EEPROM.read(i++);
	ota_mode_ = EEPROM.read(i++);
	ntp_enabled_ = EEPROM.read(i++);
	aio_enabled_ = EEPROM.read(i++);
	accel_range_scale_ = EEPROM.read(i++);

	temp.bytes[0] = EEPROM.read(i++);
	temp.bytes[1] = EEPROM.read(i++);
	temp.bytes[2] = EEPROM.read(i++);
	temp.bytes[3] = EEPROM.read(i++);
	gyro_range_scale_ = temp.floatvalue;
}

bool SPBOX_CONF::clearEEPROM(char seq)
{
	for (int i = 0; i < 13; i++) { // 12 values + 1 free space
		EEPROM.write(i, 0);
	}
	return EEPROM.commit();
}

extern SPBOX_CONF conf;