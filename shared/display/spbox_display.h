// spbox_display.h

#ifndef _SPBOX_DISPLAY_h
#define _SPBOX_DISPLAY_h

#include "arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED_WiFi.h>

class SPBOX_DISPLAY : public Adafruit_FeatherOLED_WiFi
{
public:
	SPBOX_DISPLAY(int reset = -1) : Adafruit_FeatherOLED_WiFi(reset) { };
	void initializeDisplay();

	void updatePrintBufferScr1();	// Sensor overview
	void updatePrintBufferScr2();	// Sensor Accel/Gyro min/max
	void updatePrintBufferScr3();	// Sensor Temp/Press/Alti
	void updateDisplayScr4();		// Sensor Kompass
	void updateDisplayScr5();		// Uhrzeit anzeigen
	void updateDisplayScr6();		// WLAN/Batterie Status anzeigen
	void updatePrintBufferScr4_speed(long val);
	void updatePrintBufferScr4_charmap(uint8_t val);
	void updatePrintBufferScrTest();
	void updatePrintBufferScrTest2();

	void updateDisplayWithPrintBuffer();
private:
	char displaybuffer_[4][22];  // 4 lines with 22 chars each
	char tempbuffer_[3][15];     // temp for float to str conversion
	bool update_display_;
protected:
};

extern SPBOX_DISPLAY display;

#endif
