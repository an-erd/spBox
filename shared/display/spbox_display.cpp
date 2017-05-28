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

#include <ESP8266WiFi.h>
#include <NtpClientLib.h>
#include "missing_str_util.h"
#include "spbox_display.h"
#include "spbox_sensors.h"
#include "glcdfont.c"

SPBOX_DISPLAY display;

void SPBOX_DISPLAY::initializeDisplay() {
	begin(SSD1306_SWITCHCAPVCC, 0x3C);
	//display();	// display buffer from Adafruit_SSD1306.cpp = Adafruit splash screen
	//delay(2000);
	clearDisplay();
	setTextSize(1);
	setTextColor(WHITE);
	display();

	manageInternetAvailable_ = false;
}

void SPBOX_DISPLAY::updatePrintBufferScr1() {
	float temp_x, temp_y, temp_z, temp_f;

	sensors.getAccel(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 4, 2, tempbuffer_[0]);
	dtostrf_sign(temp_y, 4, 2, tempbuffer_[1]);
	dtostrf_sign(temp_z, 4, 2, tempbuffer_[2]);
	snprintf(displaybuffer_[0], 21, "A %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	sensors.getGyro(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 4, 2, tempbuffer_[0]);
	dtostrf_sign(temp_y, 4, 2, tempbuffer_[1]);
	dtostrf_sign(temp_z, 4, 2, tempbuffer_[2]);
	snprintf(displaybuffer_[1], 21, "G %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	sensors.getHeading(&temp_f); dtostrf(temp_f, 3, 0, tempbuffer_[0]);
	sensors.getTemperature(&temp_f); dtostrf(temp_f, 5, 2, tempbuffer_[1]);
	snprintf(displaybuffer_[2], 21, "H %s T %s", tempbuffer_[0], tempbuffer_[1]);

	sensors.getAltitude(&temp_f); dtostrf(temp_f, 4, 0, tempbuffer_[0]);
	sensors.getPressure(&temp_f); dtostrf(temp_f / 100.0, 4, 0, tempbuffer_[1]);
	dtostrf(WiFi.status(), 1, 0, tempbuffer_[2]);
	snprintf(displaybuffer_[3], 21, "Alt %s P %s   W%s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);
}

void SPBOX_DISPLAY::updatePrintBufferScr2() {
	float temp_x, temp_y, temp_z, temp_f;

	sensors.getMaxAccel(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 5, 1, tempbuffer_[0]);
	dtostrf_sign(temp_y, 5, 1, tempbuffer_[1]);
	dtostrf_sign(temp_z, 5, 1, tempbuffer_[2]);
	snprintf(displaybuffer_[0], 21, "A/ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	sensors.getMinAccel(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 5, 1, tempbuffer_[0]);
	dtostrf_sign(temp_y, 5, 1, tempbuffer_[1]);
	dtostrf_sign(temp_z, 5, 1, tempbuffer_[2]);
	snprintf(displaybuffer_[1], 21, "A\\ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	sensors.getMaxGyro(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 5, 1, tempbuffer_[0]);
	dtostrf_sign(temp_y, 5, 1, tempbuffer_[1]);
	dtostrf_sign(temp_z, 5, 1, tempbuffer_[2]);
	snprintf(displaybuffer_[2], 21, "G/ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	sensors.getMinGyro(&temp_x, &temp_y, &temp_z);
	dtostrf_sign(temp_x, 5, 1, tempbuffer_[0]);
	dtostrf_sign(temp_y, 5, 1, tempbuffer_[1]);
	dtostrf_sign(temp_z, 5, 1, tempbuffer_[2]);
	snprintf(displaybuffer_[3], 21, "G\\ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);
}

void SPBOX_DISPLAY::updatePrintBufferScr3()
{
	float temp_x, temp_y, temp_z, temp_f;

	sensors.getTemperature(&temp_f); dtostrf(temp_f, 5, 2, tempbuffer_[0]);
	snprintf(displaybuffer_[0], 21, "Temperatur   \011C %s", tempbuffer_[0]);
	sensors.getPressure(&temp_f); dtostrf(temp_f / 100.0, 4, 0, tempbuffer_[0]);
	snprintf(displaybuffer_[1], 21, "Druck       hPa %s", tempbuffer_[0]);
	sensors.getAltitude(&temp_f); dtostrf(temp_f, 4, 0, tempbuffer_[0]);
	snprintf(displaybuffer_[2], 21, "H\224he          m %s", tempbuffer_[0]);
	snprintf(displaybuffer_[3], 21, "");
}

void SPBOX_DISPLAY::updateDisplayScr4()
{
	// code to draw compass inspired by http://cassiopeia.hk/arduinocompass/
	float heading_deg, heading_deg_round, angle_rad;

	sensors.getHeading(&heading_deg);

	heading_deg_round = round(heading_deg);
	if (heading_deg_round == 360)
		heading_deg_round = 0;

	dtostrf(heading_deg_round, 3, 0, tempbuffer_[0]);
	if (heading_deg > 337 || heading_deg < 23) snprintf(tempbuffer_[1], 3, "N ");
	else if (heading_deg > 22 && heading_deg < 68)snprintf(tempbuffer_[1], 3, "NO");
	else if (heading_deg > 67 && heading_deg < 113)snprintf(tempbuffer_[1], 3, "O ");
	else if (heading_deg > 112 && heading_deg < 158)snprintf(tempbuffer_[1], 3, "SO");
	else if (heading_deg > 157 && heading_deg < 203)snprintf(tempbuffer_[1], 3, "S ");
	else if (heading_deg > 202 && heading_deg < 248)snprintf(tempbuffer_[1], 3, "SW");
	else if (heading_deg > 247 && heading_deg < 293)snprintf(tempbuffer_[1], 3, "W ");
	else if (heading_deg > 292 && heading_deg < 338)snprintf(tempbuffer_[1], 3, "NW");

	snprintf(displaybuffer_[0], 20, "%s\011 %s", tempbuffer_[0], tempbuffer_[1]);

	angle_rad = PI * (float)heading_deg / 180.0;
	clearDisplay();
	setCursor(43, 8);
	setTextSize(2);
	println(displaybuffer_[0]);
	setTextSize(1);

	int x0 = 15 - sin(angle_rad) * 15; // tip of the arrow on circle
	int y0 = 15 - cos(angle_rad) * 15;
	int x1 = 15 - sin(angle_rad + 0.2) * 10; // triangle point
	int y1 = 15 - cos(angle_rad + 0.2) * 10;
	int x2 = 15 - sin(angle_rad - 0.2) * 10; // triangle point
	int y2 = 15 - cos(angle_rad - 0.2) * 10;

	drawCircle(15, 15, 15, WHITE);
	fillCircle(15, 15, 2, WHITE);
	//drawLine(15, 0, 15, 30, WHITE);
	drawLine(15, 15, x0, y0, WHITE);
	fillTriangle(x0, y0, x1, y1, x2, y2, WHITE);
	//drawLine(127, 0, 127, 31, WHITE);
	display();
}

void SPBOX_DISPLAY::updateDisplayScr5()
{
	snprintf(displaybuffer_[0], 21, "%s", NTP.getTimeDateString().c_str());
	snprintf(displaybuffer_[1], 21, "Up %s, since %s", NTP.getUptimeString().c_str(), NTP.getTimeDateString(NTP.getFirstSync()).c_str());
	snprintf(displaybuffer_[2], 21, "Since %s", NTP.getTimeDateString(NTP.getFirstSync()).c_str());
	snprintf(displaybuffer_[3], 21, "Last %s", NTP.getTimeDateString(NTP.getLastNTPSync()).c_str());

	updateDisplayWithPrintBuffer();
}

void SPBOX_DISPLAY::updateDisplayScr6() {
	bool is_connected = WiFi.status() == WL_CONNECTED;
	int8_t rssi = WiFi.RSSI();
	uint32_t ipAddress = WiFi.localIP();

	setConnected(is_connected);
	if (is_connected) {
		setRSSI(rssi);
		setIPAddress(ipAddress);
	}
	sensors.updateVBat();
	setBattery(sensors.getVBat());

	fillRect(0, 8, 128, 16, BLACK);
	setCursor(0, 8);
	switch (WiFi.status()) {
	case WL_IDLE_STATUS:
		print("Idle Status");
		break;
	case WL_NO_SSID_AVAIL:
		print("");
		break;
	case WL_SCAN_COMPLETED:
		print("Scan Completed");
		break;
	case WL_CONNECTED:
		print(WiFi.SSID());
		if (manageInternetAvailable_) {
			if (internetAvailable_) {
				print(", Inet");
				if (setMqttAvailable_)
					print(", MQTT");
			}
			else { print(", no Inet"); }
		}
		break;
	case WL_CONNECT_FAILED:
		print("Connect Failed");
		break;
	case WL_CONNECTION_LOST:
		print("Connection Lost");
		break;
	case WL_DISCONNECTED:
		print("Disconnected");
		break;
	case WL_NO_SHIELD:
		print("No Shield");
		break;
	}

	refreshIcons();
	display();
}

// Configure Altitude screen:
// smallerVal and largerVal are the smaller and larger(or equal) configured values,
// step is the scrolled text animation step (see drawScrolledText())
// bool increase true: animate from smallerVal -> largerVal
// position is the position of the number to configure (pos 0 is 10^0, pos 1 is 10^1, ...)
// Line 1/2 -> altitude input with "-" above/below to mark position
// Line 3 -> Abbruch, Line 4 -> OK
void SPBOX_DISPLAY::updateDisplayScr7(int16_t smallerVal, int16_t largerVal, uint8_t step, bool increase, uint8_t position, inputAltiModes_t altiMode)
{
	unsigned char textA[6];
	unsigned char textB[6];
	bool skip[] = { false, false, false, false, false, false, };
	uint8_t tmpStep = (step > 8) ? 8 : step;	// i.e. only update position marker

	snprintf((char*)textA, 5, "%4d", smallerVal);
	snprintf((char*)textB, 5, "%4d", largerVal);
	for (int i = 0; i < 4; i++)
		skip[i] = textA[i] == textB[i];

	clearDisplay();

	// " Neue Höhe: xxxx m"	-> 18 chars
	setCursor(6, 4); print("Neue H\224he:      m");
	drawScrolledText(12 * 6, 4, 5, textA, textB, skip, (increase ? step : 8 - step));

	// Position marker
	if (altiMode == INPUT_ALTITUDE) {
		drawLine((12 + position) * 6, 2, (12 + position) * 6 + 4, 2, WHITE);
		drawLine((12 + position) * 6, 12, (12 + position) * 6 + 4, 12, WHITE);
	}

	setCursor(6, 16); print("OK");
	setCursor(6, 24); print("Abbruch");

	// draw cursor to show active item
	switch (altiMode) {
	case BUTTON_ALTITUDE:
		setCursor(0, 4);
		write(0x10);
		break;
	case BUTTON_OK:
		setCursor(0, 16);
		write(0x10);
		break;
	case BUTTON_CANCEL:
		setCursor(0, 24);
		write(0x10);
		break;
	default:
		break;
	}

	display();
}

void SPBOX_DISPLAY::updateDisplayScr8(int16_t altitude)
{
	snprintf(displaybuffer_[0], 20, "%4dm", altitude);

	clearDisplay();
	drawBitmap(0, 0, myMountains, 32, 32, WHITE);

	setCursor(40, 8);
	setTextSize(2);
	print(displaybuffer_[0]);
	setTextSize(1);
	display();
}

void SPBOX_DISPLAY::updateDisplayScr9(float absaccel, float maxabsaccel)
{
	dtostrf(absaccel, 4, 1, tempbuffer_[0]);
	dtostrf(maxabsaccel, 4, 1, tempbuffer_[1]);
	snprintf(displaybuffer_[0], 20, "%sg", tempbuffer_[0]);
	snprintf(displaybuffer_[1], 20, "max = %sg", tempbuffer_[1]);

	clearDisplay();
	drawBitmap(0, 0, myForce, 32, 32, WHITE);

	setCursor(40, 0);
	print("\262a\262=");
	setCursor(40, 8);
	setTextSize(2);
	print(displaybuffer_[0]);
	setTextSize(1);

	setCursor(40, 24);
	print(displaybuffer_[1]);

	display();
}

void SPBOX_DISPLAY::updateDisplayScr10()
{
	// Display: x-axis -> NED y-axis, y-axis -> NED x-axis, (slowly) tilt to ground in direction of positive axis gives positive values
	float accel_x, accel_y, accel_z;
	int16_t delta_x, delta_y;
	int8_t bubble_x, bubble_y;

	sensors.getAccel(&accel_x, &accel_y, &accel_z);

	// caution, x/y axis flipped
	accel_x *= 100; delta_y = -(int16_t)accel_x;
	accel_y *= 100; delta_x = -(int16_t)accel_y;

	// stay inside/on border of circle
	float len = sqrt(delta_x*delta_x + delta_y * delta_y);
	if (len > 15.0) {
		float factor = 15.0 / len;
		delta_x = (int16_t)((delta_x + 0.5)*factor);
		delta_y = (int16_t)((delta_y + 0.5)*factor);
	}

	bubble_x = 15 + delta_x;
	bubble_y = 15 - delta_y;

	clearDisplay();

	drawCircle(15, 15, 15, WHITE);
	drawLine(0, 15, 12, 15, WHITE); drawLine(18, 15, 30, 15, WHITE);
	drawLine(15, 0, 15, 12, WHITE); drawLine(15, 18, 15, 30, WHITE);

	drawPixel(bubble_x - 2, bubble_y, WHITE); drawPixel(bubble_x - 1, bubble_y, WHITE); drawPixel(bubble_x + 1, bubble_y, WHITE); drawPixel(bubble_x + 2, bubble_y, WHITE);
	drawPixel(bubble_x, bubble_y - 2, WHITE); drawPixel(bubble_x, bubble_y - 1, WHITE); drawPixel(bubble_x, bubble_y + 1, WHITE); drawPixel(bubble_x, bubble_y + 2, WHITE);

	setCursor(40, 0); print("x: "); printf("% 3i", delta_x);
	setCursor(40, 8); print("y: "); printf("% 3i", delta_y);
	display();
}

void SPBOX_DISPLAY::updatePrintBufferScr4_speed(long val)
{
	snprintf(displaybuffer_[0], 21, "Alt %d", val);
	snprintf(displaybuffer_[1], 21, "");
	snprintf(displaybuffer_[2], 21, "");
	snprintf(displaybuffer_[3], 21, "");
}

void SPBOX_DISPLAY::updatePrintBufferScr4_charmap(uint8_t val)
{
	snprintf(displaybuffer_[0], 21, "Char %d", val);
	snprintf(displaybuffer_[1], 21, "%c", val);
	snprintf(displaybuffer_[2], 21, "");
	snprintf(displaybuffer_[3], 21, "");
}

void SPBOX_DISPLAY::updatePrintBufferScrTest()
{
	snprintf(displaybuffer_[0], 21, "123456789012345678901");
	snprintf(displaybuffer_[1], 21, "abcdefghijklmnopqrstu");
	snprintf(displaybuffer_[2], 21, "ABCDEFGHIJKLMNOPQRSTU");
	snprintf(displaybuffer_[3], 21, "öäü()[]/\?");
}
void SPBOX_DISPLAY::updatePrintBufferScrTest2()
{
	snprintf(displaybuffer_[0], 21, "Zur\81ck");
	snprintf(displaybuffer_[1], 21, "Zur\81 ck");
	snprintf(displaybuffer_[2], 21, "");
	snprintf(displaybuffer_[3], 21, "");
}

void SPBOX_DISPLAY::updateDisplayWithPrintBuffer() {
	clearDisplay();
	setCursor(0, 0);
	println(displaybuffer_[0]);
	println(displaybuffer_[1]);
	println(displaybuffer_[2]);
	println(displaybuffer_[3]);
	display();
}

// draw a vertically scrolled char "between" char A and char B. This can be
// used to show nice smoth scolled chars when e.g. input a value with rotary
// encoder.
//
// x and y are the top left coordinates of the resulting char.
// Char A is top char, char B is the bottom char,
// step defines how much the window to show is moved down.
// Example:
// - step = 0: char A is shown
// - step = 8: char B is shown
// - step = 4: bottom half of char A and top half of char B is show
//
// Remark: only 'Classic' built-in font is available, only size = 1, only color WHITE
//
void SPBOX_DISPLAY::drawScrolledChar(int16_t x, int16_t y, unsigned char cA, unsigned char cB, uint8_t step)
{
	// only 'Classic' built-in font is available
	if (gfxFont)
		return;

	// clip (right, bottom, left, top)
	if ((x >= _width) || (y >= _height) || ((x + 6 - 1) < 0) || ((y + 8 - 1) < 0))
		return;
	// Handle 'classic' charset behavior
	if (!_cp437 && (cA >= 176)) cA++;
	if (!_cp437 && (cB >= 176)) cB++;

	startWrite();
	for (int8_t i = 0; i < 6; i++) {
		uint8_t line;

		// line = (( cA (bit 0..7) | cB (bit 8..15) ) >> step ) & 0xFF;
		if (i < 5) {
			line = (pgm_read_byte(font + (cA * 5) + i) >> step) | (pgm_read_byte(font + (cB * 5) + i) << 8 - step);
		}
		else
			line = 0x0;

		for (int8_t j = 0; j < 8; j++, line >>= 1) {
			if (line & 0x1) {
				writePixel(x + i, y + j, WHITE);
			}
		}
	}
	endWrite();
}

// draw a vertically scrolled text "between" textA and textB, see also drawScrolledChar()
//
// x and y are the top left coordinates of the resulting text.
// len the length, textA and textB the text arrays
// the skip array can be used to not scroll and keep just textA[...] fixed, e.g. for a decimal point.
// step defines how much the window to show is moved down.
//
void SPBOX_DISPLAY::drawScrolledText(int16_t x, int16_t y, uint8_t len, unsigned char * textA, unsigned char * textB, bool * skip, uint8_t step)
{
	for (int8_t u = 0; u < len; u++) {
		if (skip[u])
			drawChar(x + 6 * u, y, textA[u], WHITE, BLACK, 1);
		else
			drawScrolledChar(x + 6 * u, y, textA[u], textB[u], step);
	}
}