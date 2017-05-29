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

#include "arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_FeatherOLED_WiFi.h>
#include "myconfig.h"

class SPBOX_DISPLAY : public Adafruit_FeatherOLED_WiFi
{
public:
	SPBOX_DISPLAY(int reset = -1) : Adafruit_FeatherOLED_WiFi(reset) { };
	void initializeDisplay();

	void updatePrintBufferScr1();	// Sensor overview
	void updatePrintBufferScr2();	// Sensor Accel/Gyro min/max
	void updatePrintBufferScr3();	// Sensor Temp/Press/Alti
	void updateDisplayScr4(compassModes_t mode);		// Sensor Kompass
	void updateDisplayScr5();		// Uhrzeit anzeigen
	void updateDisplayScr6();		// WLAN/Batterie Status anzeigen
	void updateDisplayScr7(int16_t smallerVal, int16_t largerVal, uint8_t step, bool increase, uint8_t position, inputAltiModes_t altiMode);		// Config Altitude
	void updateDisplayScr8(int16_t altitude); // show Altitude
	void updateDisplayScr9(float absaccel, float maxabsaccel);
	void updateDisplayScr10();
	void updatePrintBufferScr4_speed(long val);
	void updatePrintBufferScr4_charmap(uint8_t val);
	void updatePrintBufferScrTest();
	void updatePrintBufferScrTest2();

	void updateDisplayWithPrintBuffer();

	void setInternetAvailable(bool avail) { manageInternetAvailable_ = true; internetAvailable_ = avail; }
	void setMqttAvailable(bool avail) { setMqttAvailable_ = avail; }

	// new helper functions for a nice UI
	void drawScrolledChar(int16_t x, int16_t y, unsigned char cA, unsigned char cB, uint8_t step);
	void drawScrolledText(int16_t x, int16_t y, uint8_t len, unsigned char *textA, unsigned char *textB, bool *skip, uint8_t step);
private:
	char displaybuffer_[4][22];  // 4 lines with 22 chars each
	char tempbuffer_[3][15];     // temp for float to str conversion
	bool update_display_;
	bool manageInternetAvailable_;	// will internet tracked in Display, too?
	bool internetAvailable_;	// not nice, needs rework
	bool setMqttAvailable_;
protected:
};

extern SPBOX_DISPLAY display;
