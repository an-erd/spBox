#ifndef _MYCONFIG_h
#define _MYCONFIG_h

// WLAN
//#define WLAN_SSID		"..."
//#define WLAN_PASSWORD	"..."

// ADAFRUIT IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
//#define AIO_USERNAME  "..."
//#define AIO_KEY		"..."

// Timer delay constants in milliseconds(MS)
#define DELAY_MS_1HZ	1000
#define DELAY_MS_2HZ	500
#define DELAY_MS_5HZ	200
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000

// typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;

// Output and debug
#define DBG_PORT Serial
#define DEBUG_COM
#define DEBUG_CONF
#define	SERIAL_STATUS_OUTPUT

typedef enum OTAMode { OTA_OFF = 0, OTA_IDE = 1, OTA_HTTP_SERVER = 2, } OTAModes_t;

#endif
