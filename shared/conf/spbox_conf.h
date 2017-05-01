// spbox_conf.h

#ifndef _SPBOX_CONF_h
#define _SPBOX_CONF_h

#include "arduino.h"
#include "user_config.h"
#include "spbox_conf.h"
#include "spbox_com.h"
#include "MPU6050.h"

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

	void setAioEnabled(bool aio_enabled = true);
	bool getAioEnabled();

	void setAccelRangeScale(uint8_t accel_range_scale = MPU6050_ACCEL_FS_16);
	void setGyroRangeScale(float gyro_range_scale = MPU6050_GYRO_FS_2000);
	//	void setAccelGyroOrientation(uint8_t accel_gyro_orientation = 0);

	bool writeConfToEEPROM();
	void readConfFromEEPROM();
	bool clearEEPROM(char seq = 0);

private:
	bool		wlan_enabled_;
	OTAModes_t	ota_mode_;
	bool		ntp_enabled_;
	bool		aio_enabled_;
	uint8_t		accel_range_scale_;
	float		gyro_range_scale_;
	//	uint8_t		accel_gyro_orientation_;
	//	uint8_t		mag_orientation_;
	//bool		use_configured_sea_level_;
	//float		sea_level_pressure_;
protected:
};

extern SPBOX_CONF conf;

#endif
