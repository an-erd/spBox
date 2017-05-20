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
	LCDML_DISP_triggerMenu(DELAY_MS_5HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_overview)
{
	display.updatePrintBufferScr1();
	display.updateDisplayWithPrintBuffer();
	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
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
	if (LCDML_BUTTON_checkLeft()) {
		LCDML_BUTTON_resetAll();
		sensors.resetMinMaxAccelGyro();
	}

	display.updatePrintBufferScr2();
	display.updateDisplayWithPrintBuffer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_min_max)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_temp_press_alt)
{
	//display.updatePrintBufferScr3();
	//display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_temp_press_alt)
{
	display.updatePrintBufferScr3();
	display.updateDisplayWithPrintBuffer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_temp_press_alt)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_kompass)
{
	display.updateDisplayScr4();
	LCDML_DISP_triggerMenu(DELAY_MS_5HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_kompass)
{
	display.updateDisplayScr4();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_kompass)
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

	display.updateDisplayScr6();

	LCDML_DISP_triggerMenu(DELAY_MS_2HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_status_wlan)
{
	if (LCDML_BUTTON_checkLeft()) {
		LCDML_BUTTON_resetAll();
		if (conf.getWlanEnabled()) {
			com.disableWlan();
		}
		else {
			com.enableWlan();
		}
	}

	display.updateDisplayScr6();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
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
	gConfigAltitude = 0;
	//display.updatePrintBufferScr4_speed(gConfigAltitude);
	display.updatePrintBufferScr4_charmap(gConfigAltitude);
	display.updateDisplayWithPrintBuffer();
	gConfigValueChanged = false;

	//LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_config_altitude)
{
	//Serial.println("enter LCDML_FUNC_config_altitude");
	if (LCDML_BUTTON_checkEnter()) {
		//Serial.println("checkEnter()");
		Serial.printf("Layer: %d, Function: %d\n", LCDML.getLayer(), LCDML.getFunction());
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

		//display.updatePrintBufferScr4_speed(gConfigAltitude);
		display.updatePrintBufferScr4_charmap(gConfigAltitude);
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
void LCDML_DISP_setup(LCDML_FUNC_clock)
{
	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_clock)
{
	display.updateDisplayScr5();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML.goBack();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_clock)
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
// ############################################################################

void LCDML_DISP_setup(LCDML_FUNC_test2)
{
	display.updatePrintBufferScrTest2();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_test2)
{
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_test2)
{
}

// ############################################################################
int gIdleLedBlink;		// bckw. cnt
bool gIdleLedBlinkOn;
#define IDLE_LED_BLINK_COUNTER	99
// 1 cyle on, 9 cycle off
void LCDML_DISP_setup(LCDML_FUNC_initscreen)
{
	display.ssd1306_command(SSD1306_DISPLAYOFF);

	gIdleLedBlinkOn = false;
	gIdleLedBlink = IDLE_LED_BLINK_COUNTER;

	LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_initscreen)
{
	if (gIdleLedBlinkOn) {
		digitalWrite(LED_R, HIGH);	// off
		gIdleLedBlinkOn = false;
		gIdleLedBlink = IDLE_LED_BLINK_COUNTER;
	}
	else {
		if (gIdleLedBlink == 0) {
			digitalWrite(LED_R, LOW);	// on
			gIdleLedBlinkOn = true;
			gIdleLedBlink = 0;
		}
		else {
			gIdleLedBlink--;
		}
	}

	LCDML_DISP_resetIsTimer();
	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_initscreen)
{
	display.ssd1306_command(SSD1306_DISPLAYON);
	digitalWrite(LED_R, HIGH);	// off
	gIdleLedBlinkOn = false;

	LCDML.goRoot();	// LCDMenuLib_getActiveFuncId()
}