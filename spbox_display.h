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
	void update_print_buffer_scr1();
	void update_print_buffer_scr2();
	void update_display_scr3();
	void update_display_with_print_buffer();
private:
	char displaybuffer_[4][21];  // 4 lines with 21 chars each
	char tempbuffer_[3][15];     // temp for float to str conversion
	bool update_display_;
protected:
};

extern SPBOX_DISPLAY display;

#endif
