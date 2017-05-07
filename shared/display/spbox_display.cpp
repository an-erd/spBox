#include <ESP8266WiFi.h>
#include <NtpClientLib.h>
#include "missing_str_util.h"
#include "spbox_display.h"
#include "spbox_sensors.h"

SPBOX_DISPLAY display;

void SPBOX_DISPLAY::initializeDisplay() {
	begin(SSD1306_SWITCHCAPVCC, 0x3C);
	clearDisplay();
	setTextSize(1);
	setTextColor(WHITE);
	display();
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
	float heading_deg, angle_rad;

	sensors.getHeading(&heading_deg);
	angle_rad = PI * (float)heading_deg / 180.0;

	dtostrf(heading_deg, 3, 0, tempbuffer_[0]);
	if (heading_deg > 337 || heading_deg < 23) snprintf(tempbuffer_[1], 3, "N ");
	else if (heading_deg > 22 && heading_deg < 68)snprintf(tempbuffer_[1], 3, "NO");
	else if (heading_deg > 67 && heading_deg < 113)snprintf(tempbuffer_[1], 3, "O ");
	else if (heading_deg > 112 && heading_deg < 158)snprintf(tempbuffer_[1], 3, "SO");
	else if (heading_deg > 157 && heading_deg < 203)snprintf(tempbuffer_[1], 3, "S ");
	else if (heading_deg > 202 && heading_deg < 248)snprintf(tempbuffer_[1], 3, "SW");
	else if (heading_deg > 247 && heading_deg < 293)snprintf(tempbuffer_[1], 3, "W ");
	else if (heading_deg > 292 && heading_deg < 338)snprintf(tempbuffer_[1], 3, "NW");

	snprintf(displaybuffer_[0], 20, "%s\011 %s", tempbuffer_[0], tempbuffer_[1]);

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
	snprintf(displaybuffer_[3], 21, "");

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