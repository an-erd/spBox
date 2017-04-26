//
//
//

#include "spbox_display.h"
#include <ESP8266WiFi.h>

SPBOX_DISPLAY display;

void SPBOX_DISPLAY::updatePrintBufferScr1() {
	dtostrf_sign(sensors.ax_f_, 4, 2, tempbuffer_[0]);
	//dtostrf_sign(sensors.ay_f, 4, 2, tempbuffer_[1]);
	//dtostrf_sign(sensors.az_f, 4, 2, tempbuffer_[2]);
	//snprintf(displaybuffer_[0], 21, "A %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	//dtostrf_sign(sensors.gx_f, 4, 2, tempbuffer_[0]);
	//dtostrf_sign(sensors.gy_f, 4, 2, tempbuffer_[1]);
	//dtostrf_sign(sensors.gz_f, 4, 2, tempbuffer_[2]);
	//snprintf(displaybuffer_[1], 21, "G %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	//dtostrf(sensors.heading, 3, 0, tempbuffer_[0]);
	//dtostrf(sensors.temperature, 5, 2, tempbuffer_[1]);
	//snprintf(displaybuffer_[2], 21, "H %s T %s", tempbuffer_[0], tempbuffer_[1]);

	//dtostrf(sensors.altitude, 4, 0, tempbuffer_[0]);
	//dtostrf(sensors.pressure / 100.0, 4, 0, tempbuffer_[1]);
	//dtostrf(WiFi.status(), 1, 0, tempbuffer_[2]);
	//snprintf(displaybuffer_[3], 21, "Alt %s P %s   W%s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);
}

void SPBOX_DISPLAY::updatePrintBufferScr2() {
	//dtostrf_sign(sensors.max_ax_f, 5, 1, tempbuffer_[0]);
	//dtostrf_sign(sensors.max_ay_f, 5, 1, tempbuffer_[1]);
	//dtostrf_sign(sensors.max_az_f, 5, 1, tempbuffer_[2]);
	//snprintf(displaybuffer_[0], 21, "A/ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	//dtostrf_sign(sensors.min_ax_f, 5, 1, tempbuffer_[0]);
	//dtostrf_sign(sensors.min_ay_f, 5, 1, tempbuffer_[1]);
	//dtostrf_sign(sensors.min_az_f, 5, 1, tempbuffer_[2]);
	//snprintf(displaybuffer_[1], 21, "A\\ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	//dtostrf_sign(sensors.max_gx_f, 5, 1, tempbuffer_[0]);
	//dtostrf_sign(sensors.max_gy_f, 5, 1, tempbuffer_[1]);
	//dtostrf_sign(sensors.max_gz_f, 5, 1, tempbuffer_[2]);
	//snprintf(displaybuffer_[2], 21, "G/ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);

	//dtostrf_sign(sensors.min_gx_f, 5, 1, tempbuffer_[0]);
	//dtostrf_sign(sensors.min_gy_f, 5, 1, tempbuffer_[1]);
	//dtostrf_sign(sensors.min_gz_f, 5, 1, tempbuffer_[2]);
	//snprintf(displaybuffer_[3], 21, "G\\ %s %s %s", tempbuffer_[0], tempbuffer_[1], tempbuffer_[2]);
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

void SPBOX_DISPLAY::updateDisplayWithPrintBuffer() {
	clearDisplay();
	setCursor(0, 0);
	println(displaybuffer_[0]);
	println(displaybuffer_[1]);
	println(displaybuffer_[2]);
	println(displaybuffer_[3]);
	display();
}

void initialize_display() {
	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	display.clearDisplay();
	display.setTextSize(1);
	display.setTextColor(WHITE);
	display.display();
	//display_struct.update_display = true;	// TODO
}