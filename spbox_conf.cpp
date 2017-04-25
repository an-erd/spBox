//
//
//

#include "spbox_conf.h"

void initialize_basic_config()
{
	gConfig.wlan_initialized = false;
	gConfig.wlan_enabled = true;
	gConfig.ota_mode = OTA_IDE;
	gConfig.ntp_enabled = true;
	gConfig.aio_enabled = true;
	gConfig.accel_range_scale = MPU6050_ACCEL_FS_16;
	gConfig.gyro_range_scale = MPU6050_GYRO_FS_2000;
	gConfig.accel_gyro_orientation = 0;	// TODO
	gConfig.mag_orientation = 0;		// TODO
	gConfig.use_configured_sea_level = false;
	gConfig.sea_level_pressure = 101325;
}