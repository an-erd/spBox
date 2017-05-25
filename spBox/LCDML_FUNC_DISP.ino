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
int16_t	gConfigAltitudeSmaller;
int16_t gConfigAltitudeLarger;
uint8_t gConfigAltitudeCurrStep;
bool gConfigAltitudeIncrease;
int8_t gConfigAltitudePosition;

void LCDML_DISP_setup(LCDML_FUNC_config_altitude)
{
	float tmpAlt, tmpPressure;
	sensors.getAltitude(&tmpAlt);
	tmpAlt = 9999.0;
	gConfigAltitudeSmaller = gConfigAltitudeLarger = (int16_t)tmpAlt;
	gConfigAltitudeCurrStep = 10; // paused
	gConfigAltitudePosition = 0; // X... -> Value of pos = 10^(3-pos)

	display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, 0, true, 0);
	LCDML_DISP_triggerMenu(DELAY_MS_40HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_config_altitude)
{
	if (LCDML_BUTTON_checkEnter()) {
		gConfigAltitudePosition++;
		gConfigAltitudePosition %= 4;
		display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, gConfigAltitudeCurrStep, gConfigAltitudeIncrease, gConfigAltitudePosition);
	}
	else
	{
		if (LCDML_BUTTON_checkUp()) {
			// increase number
			gConfigAltitudeLarger += pow10(3 - gConfigAltitudePosition);
			gConfigAltitudeIncrease = true;

			if (gConfigAltitudeLarger > 9999)
				gConfigAltitudeLarger = 9999;

			if (gConfigAltitudeSmaller < gConfigAltitudeLarger)
				gConfigAltitudeCurrStep = 0;
		}

		if (LCDML_BUTTON_checkDown()) {
			// decrease number
			gConfigAltitudeSmaller -= pow10(3 - gConfigAltitudePosition);
			gConfigAltitudeIncrease = false;

			if (gConfigAltitudeSmaller < 0)
				gConfigAltitudeSmaller = 0;

			if (gConfigAltitudeSmaller < gConfigAltitudeLarger)
				gConfigAltitudeCurrStep = 0;
		}
	}

	switch (gConfigAltitudeCurrStep) {
	case 9: // reinitialize
		if (gConfigAltitudeIncrease)
			gConfigAltitudeSmaller = gConfigAltitudeLarger;
		else
			gConfigAltitudeLarger = gConfigAltitudeSmaller;
		gConfigAltitudeCurrStep++;
		break;
	case 10: // pause
		break;
	default: // scrolled text steps 0..8
		display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, gConfigAltitudeCurrStep, gConfigAltitudeIncrease, gConfigAltitudePosition);
		gConfigAltitudeCurrStep++;
		break;
	}

	LCDML_BUTTON_resetAll();
	LCDML_DISP_resetIsTimer();
}

void LCDML_DISP_loop_end(LCDML_FUNC_config_altitude)
{
	//tmpPressure = sensors.calcPressureAtSealevel(468.0);	// W12, L.-E. -> 102306
	//Serial.print("pressure@sealevel: "); Serial.println((long)tmpPressure);
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

// ############################################################################
int gScrollTestStep = 0;

void LCDML_DISP_setup(LCDML_FUNC_scrolltest)
{
	display.clearDisplay();
	display.display();
	gScrollTestStep = 0;
	LCDML_DISP_triggerMenu(DELAY_MS_20HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_scrolltest)
{
	uint8_t len = 5;
	unsigned char textA[] = { '1', '2', '3', '.', '5', };
	unsigned char textB[] = { '2', '3', '4', '.', '6', };
	bool skip[] = { false, false, false, true, false, };

	display.clearDisplay();

	display.drawScrolledChar(0, 0, 'A', 'B', gScrollTestStep);
	for (int i = 0; i < 9; i++) {
		display.drawScrolledChar(6 * i, 8, 'A', 'B', i);
	}

	display.drawScrolledText(0, 16, len, textA, textB, skip, gScrollTestStep);

	display.display();

	gScrollTestStep++;
	gScrollTestStep %= 9;
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_scrolltest)
{
}