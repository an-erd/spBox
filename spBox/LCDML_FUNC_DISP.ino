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
bool	gConfigValueChanged;
int16_t	gConfigAltitude;

void LCDML_DISP_setup(LCDML_FUNC_config_altitude)
{
	//display.clearDisplay();
	//display.setCursor(0, 0);
	//display.println("Durch Drehen eingeben");
	//display.println("und Knopf drücken.");
	//display.println(" ");
	//display.print("Neue Höhe:");
	//gConfigAltitudeCursorX = display.getCursorX();
	//gConfigAltitudeCursorY = display.getCursorY();
	//display.print(gConfigAltitude);
	//display.display();
	gConfigAltitude = 0;
	display.updatePrintBufferScr4_speed(gConfigAltitude);
	display.updateDisplayWithPrintBuffer();
	gConfigValueChanged = false;

	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_config_altitude)
{
	//Serial.println("enter LCDML_FUNC_config_altitude");
	if (LCDML_BUTTON_checkEnter()) {
		//Serial.println("checkEnter()");
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
	else
	{
		if (LCDML_BUTTON_checkUp()) {
			//Serial.println("checkUp()");
			gConfigAltitude++;
			gConfigValueChanged = true;
		}

		if (LCDML_BUTTON_checkDown()) {
			//Serial.println("checkDown()");
			gConfigAltitude--;
			gConfigValueChanged = true;
		}
		if (gConfigValueChanged) {
			//Serial.printf("LCDML_DISP_loop(LCDML_FUNC_config_altitude): %d\n", gConfigAltitude);
			gConfigValueChanged = false;
		}

		display.updatePrintBufferScr4_speed(gConfigAltitude);
		display.updateDisplayWithPrintBuffer();
	}

	LCDML_BUTTON_resetAll();
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

// ############################################################################
long speedcnt;
void LCDML_DISP_setup(LCDML_FUNC_speed)
{
	speedcnt = 0;
	display.updatePrintBufferScr4_speed(speedcnt);
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_speed)
{
	speedcnt++;
	display.updatePrintBufferScr4_speed(speedcnt);
	display.updateDisplayWithPrintBuffer();
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_speed)
{
}