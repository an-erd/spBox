// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_back)
{
}

void LCDML_DISP_loop(LCDML_FUNC_back)
{
	LCDML_DISP_resetIsTimer();
	LCDML.goBack();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_back)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_overview)
{
	display.updatePrintBufferScr1();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_overview)
{
	display.updatePrintBufferScr1();
	display.updateDisplayWithPrintBuffer();
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_overview)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_min_max)
{
	display.updatePrintBufferScr2();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_2HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_min_max)
{
	display.updatePrintBufferScr2();
	display.updateDisplayWithPrintBuffer();
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_min_max)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_min_max_reset)
{
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_min_max_reset)
{
	//reset_min_max_accelgyro();

	LCDML_DISP_resetIsTimer();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_min_max_reset)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_status_wlan)
{
	display.setBatteryVisible(true);
	display.setBatteryIcon(true);
	display.setConnectedVisible(true);
	display.setRSSIVisible(true);
	display.setRSSIIcon(true);

	display.updateDisplayScr3();

	LCDML_DISP_triggerMenu(DELAY_MS_2HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_status_wlan)
{
	display.updateDisplayScr3();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_status_wlan)
{
}

// ############################################################################
int16_t	gConfigAltitude;
uint8	gConfigAltitudeCursorX;
uint8	gConfigAltitudeCursorY;
int32_t	gConfigAltitudeRotEncHist;

void LCDML_DISP_setup(LCDML_FUNC_config_altitude)
{
	//gConfigAltitude = sensors.altitude;
	//gConfigAltitudeRotEncHist = rotenc.LCDML_rotenc_value;

	display.clearDisplay();
	display.setCursor(0, 0);
	display.println("Durch Drehen eingeben");
	display.println("und Knopf drücken.");
	display.println(" ");
	display.print("Neue Höhe:");
	gConfigAltitudeCursorX = display.getCursorX();
	gConfigAltitudeCursorY = display.getCursorY();
	display.print(gConfigAltitude);
	display.display();

	LCDML_DISP_triggerMenu(500);
}

void LCDML_DISP_loop(LCDML_FUNC_config_altitude)
{
	if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
		int aDiff = 0; // abs(gConfigAltitudeRotEncHist - rotenc.LCDML_rotenc_value);

		if (aDiff > 7) {
			gConfigAltitude += 1000 * (LCDML_BUTTON_checkUp() ? -1 : 1);
		}
		else if (aDiff > 4) {
			gConfigAltitude += 100 * (LCDML_BUTTON_checkUp() ? -1 : 1);
		}
		else if (aDiff > 2) {
			gConfigAltitude += 10 * (LCDML_BUTTON_checkUp() ? -1 : 1);
		}
		else {
			gConfigAltitude += 1 * (LCDML_BUTTON_checkUp() ? -1 : 1);
		}

		//Serial.print(gConfigAltitudeCursorX); Serial.print(" "); Serial.println(gConfigAltitudeCursorY);
		//gConfigAltitudeRotEncHist = rotenc.LCDML_rotenc_value;

		//display.clearDisplay();
		display.setCursor(0, 0);
		display.println("Durch Drehen eingeben");
		display.println("und Knopf drücken.");
		display.println();
		display.print("Neue Höhe:");
		display.print(gConfigAltitude);
		display.display();
		//display.fillRect(gConfigAltitudeCursorX, gConfigAltitudeCursorY, _LCDML_FONT_W * 5, _LCDML_FONT_H, BLACK);
		//display.setCursor(gConfigAltitudeCursorX, gConfigAltitudeCursorY);
		//display.print(gConfigAltitude);
		//display.display();
		LCDML_BUTTON_resetUp();
		LCDML_BUTTON_resetDown();
	}

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_config_altitude)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_ownerinformation)
{
	display.clearDisplay();
	display.setCursor(0, 0);
	display.println("Andreas Erdmann");
	display.display();
	LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_ownerinformation)
{
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML.goBack();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_ownerinformation)
{
}