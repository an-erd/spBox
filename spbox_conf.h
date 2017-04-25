// spbox_conf.h

#ifndef _SPBOX_CONF_h
#define _SPBOX_CONF_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

typedef struct {
	// WLAN
	bool		wlan_initialized;
	bool		wlan_enabled;

	// OTA
	OTAModes	ota_mode;

	// NTP time
	bool		ntp_enabled;

	// Adafruit IO and Home Automation (W12)
	bool		aio_enabled;

	// Accel/Gyro
	uint8_t		accel_range_scale;
	uint8_t		gyro_range_scale;
	uint8_t		accel_gyro_orientation;

	// Magnetometer
	uint8_t		mag_orientation;

	// Barometer/Altitude
	bool		use_configured_sea_level;
	float		sea_level_pressure;
} sGlobalConfig;
volatile sGlobalConfig gConfig;

#endif
