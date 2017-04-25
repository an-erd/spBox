// spbox_conf.h

#ifndef _SPBOX_CONF_h
#define _SPBOX_CONF_h

#include "arduino.h"
#include "spbox_com.h"
#include "MPU6050.h"

class SPBOX_CONF
{
public:
	SPBOX_CONF();
	void initialize(bool from_eeprom = false);

	void setWlanInitialized(bool wlan_initialized = true);
	bool getWlanInitialized();

	void setWlanEnabled(bool wlan_enabled = true);
	bool getWlanEnabled();

	void setOtaMode(OTAModes ota_mode_ = OTA_IDE);
	OTAModes getOtaMode();

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
	// WLAN
	bool		wlan_initialized_;
	bool		wlan_enabled_;

	// OTA
	OTAModes	ota_mode_;

	// NTP time
	bool		ntp_enabled_;

	// Adafruit IO and Home Automation (W12)
	bool		aio_enabled_;

	// Accel/Gyro
	uint8_t		accel_range_scale_;
	float		gyro_range_scale_;
	//	uint8_t		accel_gyro_orientation_;

	// Magnetometer
	//	uint8_t		mag_orientation_;

	// Barometer/Altitude
	//bool		use_configured_sea_level_;
	//float		sea_level_pressure_;
protected:
};

extern SPBOX_CONF conf;

#endif
