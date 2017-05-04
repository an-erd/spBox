#include <ESP8266WiFi.h>
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

void SPBOX_DISPLAY::updateDisplayScr3() {
	int8_t rssi = WiFi.RSSI();
	uint32_t ipAddress = WiFi.localIP();

	//int level = analogRead(VBAT_PIN);
	// analog read level is 10 bit 0-1023 (0V-1V).
	// resistors of the voltage divider 271K and 1M -> 271 / 1271 =
	// lipo values:
	// 3.14V -> 0.669V -> analog value 684
	// 4.20V -> 0.895V -> analog value 915
	//int perc_level = map(level, 684, 915, 0, 100); // level in percentage
	//float temp_volt = level * 0.97751 * 4.69;
	//setBattery(temp_volt);
	//Serial.print("Battery: "); Serial.print(temp_volt); Serial.print(", level "); Serial.println(perc_level);
	//Serial.println(level);
	//Adafruit_IO_Feed battery = aio.getFeed("battery");
	//battery.send(level);

	//updateVbat();
	//bool is_connected;
	//is_connected = WiFi.status() == WL_CONNECTED;
	//setConnected(is_connected);
	//if (is_connected) {
	//	setRSSI(rssi);
	//	setIPAddress(ipAddress);
	//}
	//refreshIcons();

	//clearMsgArea();
	//print(level);
	//switch (WiFi.status()) {
	//case WL_IDLE_STATUS:
	//	print("Idle Status");
	//	break;
	//case WL_NO_SSID_AVAIL:
	//	print("");
	//	break;
	//case WL_SCAN_COMPLETED:
	//	print("Scan Completed");
	//	break;
	//case WL_CONNECTED:
	//	print(WiFi.SSID());
	//	break;
	//case WL_CONNECT_FAILED:
	//	print("Connect Failed");
	//	break;
	//case WL_CONNECTION_LOST:
	//	print("Connection Lost");
	//	break;
	//case WL_DISCONNECTED:
	//	print("Disconnected");
	//	break;
	//case WL_NO_SHIELD:
	//	print("No Shield");
	//	break;
	//}

	display();
}

void SPBOX_DISPLAY::updatePrintBufferScr4_speed(long val)
{
	snprintf(displaybuffer_[0], 21, "Alt %d", val);
	snprintf(displaybuffer_[1], 21, "");
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

void SPBOX_DISPLAY::updateDisplayWithPrintBuffer() {
	clearDisplay();
	setCursor(0, 0);
	println(displaybuffer_[0]);
	println(displaybuffer_[1]);
	println(displaybuffer_[2]);
	println(displaybuffer_[3]);
	display();
}