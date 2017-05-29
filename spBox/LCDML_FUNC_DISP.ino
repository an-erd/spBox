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
compassModes_t gConfigCompassMode;
void LCDML_DISP_setup(LCDML_FUNC_kompass)
{
	gConfigCompassMode = COMPASS_ROSE;

	display.updateDisplayScr4(gConfigCompassMode);
	LCDML_DISP_triggerMenu(DELAY_MS_5HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_kompass)
{
	int tmp_mode = (int)gConfigCompassMode;
	if (LCDML_BUTTON_checkUp()) {
		LCDML_BUTTON_resetUp();

		tmp_mode -= 1;
		if (tmp_mode < 0)
			tmp_mode = COMPASS_LAST - 1;
	}
	if (LCDML_BUTTON_checkDown()) {
		LCDML_BUTTON_resetDown();
		tmp_mode += 1;
		tmp_mode %= COMPASS_LAST;
	}
	gConfigCompassMode = (compassModes_t)tmp_mode;

	display.updateDisplayScr4(gConfigCompassMode);

	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_kompass)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_altitude)
{
	float tmp;

	sensors.getAltitude(&tmp);
	display.updateDisplayScr8((long)tmp);

	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_altitude)
{
	float tmp;

	sensors.getAltitude(&tmp);
	display.updateDisplayScr8((long)tmp);
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_altitude)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_max_accel)
{
	float absAccel, maxAbsAccel;
	sensors.getAbsAccel(&absAccel);
	sensors.getMaxAbsAccel(&maxAbsAccel);

	display.updateDisplayScr9(absAccel, maxAbsAccel);

	LCDML_DISP_triggerMenu(DELAY_MS_5HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_max_accel)
{
	float absAccel, maxAbsAccel;

	if (LCDML_BUTTON_checkLeft()) {
		LCDML_BUTTON_resetAll();
		sensors.resetMaxAbsAccel();
	}

	sensors.getAbsAccel(&absAccel);
	sensors.getMaxAbsAccel(&maxAbsAccel);

	display.updateDisplayScr9(absAccel, maxAbsAccel);

	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_max_accel)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_waterbubble)
{
	display.updateDisplayScr10();

	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_waterbubble)
{
	//if (LCDML_BUTTON_checkLeft()) {
	//	LCDML_BUTTON_resetAll();
	//	sensors.resetMaxAbsAccel();
	//}

	display.updateDisplayScr10();

	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_waterbubble)
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
inputAltiModes_t gConfigAltitudeMode;

void LCDML_DISP_setup(LCDML_FUNC_config_altitude)
{
	float tmpAlt;

	sensors.getAltitude(&tmpAlt);

	gConfigAltitudeSmaller = gConfigAltitudeLarger = (int16_t)tmpAlt;

	gConfigAltitudeMode = BUTTON_ALTITUDE;	// initial fokus on Button
	gConfigAltitudeCurrStep = 10;			// scroll animation paused
	gConfigAltitudePosition = 0;			// Value of pos = 10^(3-pos)

	display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, 0, true, 0, gConfigAltitudeMode);

	LCDML_DISP_triggerMenu(DELAY_MS_40HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_config_altitude)
{
	if (LCDML_BUTTON_checkEnter()) {
		switch (gConfigAltitudeMode) {
		case BUTTON_ALTITUDE:
			gConfigAltitudeMode = INPUT_ALTITUDE;
			break;
		case INPUT_ALTITUDE:
			gConfigAltitudePosition++;
			if (gConfigAltitudePosition > 3) {
				gConfigAltitudePosition = 0;
				gConfigAltitudeMode = BUTTON_OK;
			}
			break;
		case BUTTON_OK:
			LCDML_BUTTON_resetAll();
			LCDML_DISP_resetIsTimer();
			LCDML_DISP_funcend();
			break;
		case BUTTON_CANCEL:
			LCDML_BUTTON_resetAll();
			LCDML_DISP_resetIsTimer();
			LCDML_DISP_funcend();
			break;
		default:
			break;
		}

		display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, gConfigAltitudeCurrStep, gConfigAltitudeIncrease, gConfigAltitudePosition, gConfigAltitudeMode);
	}
	else
	{
		if (LCDML_BUTTON_checkUp()) {
			switch (gConfigAltitudeMode) {
			case BUTTON_ALTITUDE:
				gConfigAltitudeMode = BUTTON_CANCEL;
				break;
			case INPUT_ALTITUDE:
				// input numer -> increase number
				gConfigAltitudeLarger += pow10(3 - gConfigAltitudePosition);
				gConfigAltitudeIncrease = true;

				if (gConfigAltitudeLarger > 9999)
					gConfigAltitudeLarger = 9999;

				if (gConfigAltitudeSmaller < gConfigAltitudeLarger)
					gConfigAltitudeCurrStep = 0;
				break;
			case BUTTON_OK:
				gConfigAltitudeMode = BUTTON_ALTITUDE;
				break;
			case BUTTON_CANCEL:
				gConfigAltitudeMode = BUTTON_OK;
				break;
			}
		}

		if (LCDML_BUTTON_checkDown()) {
			switch (gConfigAltitudeMode) {
			case BUTTON_ALTITUDE:
				gConfigAltitudeMode = BUTTON_OK;
				break;
			case INPUT_ALTITUDE:
				// in input numer -> decrease number
				gConfigAltitudeSmaller -= pow10(3 - gConfigAltitudePosition);
				gConfigAltitudeIncrease = false;

				if (gConfigAltitudeSmaller < 0)
					gConfigAltitudeSmaller = 0;

				if (gConfigAltitudeSmaller < gConfigAltitudeLarger)
					gConfigAltitudeCurrStep = 0;
				break;
			case BUTTON_OK:
				gConfigAltitudeMode = BUTTON_CANCEL;
				break;
			case BUTTON_CANCEL:
				gConfigAltitudeMode = BUTTON_ALTITUDE;
				break;
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
			display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, gConfigAltitudeCurrStep, gConfigAltitudeIncrease, gConfigAltitudePosition, gConfigAltitudeMode);
			break;
		default: // scrolled text steps 0..8
			display.updateDisplayScr7(gConfigAltitudeSmaller, gConfigAltitudeLarger, gConfigAltitudeCurrStep, gConfigAltitudeIncrease, gConfigAltitudePosition, gConfigAltitudeMode);
			gConfigAltitudeCurrStep++;
			break;
		}
	}
	LCDML_BUTTON_resetAll();
	LCDML_DISP_resetIsTimer();
}

void LCDML_DISP_loop_end(LCDML_FUNC_config_altitude)
{
	float tmpPressure;
	float tmpAlt;

	if (gConfigAltitudeMode == BUTTON_OK) {
		tmpPressure = sensors.calcPressureAtSealevel(gConfigAltitudeLarger);	// W12, L.-E. -> 102306@468.0m

		conf.setSeaLevelPressure(tmpPressure);
		conf.writeConfToEEPROM();

		sensors.calcAltitude();
		sensors.getAltitude(&tmpAlt);
		Serial.print("pressure@sealevel: "); Serial.print((long)tmpPressure); ; Serial.print(", alt: "); Serial.println((long)tmpAlt);
	}
}

// ############################################################################
uint32_t gMillisReset;
bool gEEPROMResetDone;

void LCDML_DISP_setup(LCDML_FUNC_reset)
{
	gMillisReset = millis();
	gEEPROMResetDone = false;

	display.clearDisplay();
	display.setCursor(5 * 5, 8); display.print("Ger\204t wird");
	display.setCursor(5 * 3, 16); display.print("zur\201ckgesetzt!");
	display.display();
	LCDML_DISP_triggerMenu(200);
}

void LCDML_DISP_loop(LCDML_FUNC_reset)
{
	if (LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		return;
	}

	if (!gEEPROMResetDone) {
		gEEPROMResetDone = true;

		bool result = conf.clearEEPROM();
		Serial.printf("reset, clear EERPOM %d\n", result);
		conf.initialize(false);
		result = conf.writeConfToEEPROM();
		Serial.printf("reset, write EERPOM %d\n", result);

		conf.printEEPROM(20);
	}
	uint32_t time_diff;
	time_diff = millis() - gMillisReset;
	if (time_diff < 3000)
		return;

	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_reset)
{
	ESP.restart();
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
		//LCDML.goBack();
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
		//LCDML.goBack();
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