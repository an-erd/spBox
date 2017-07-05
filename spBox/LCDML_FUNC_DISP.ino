#include "myconfig.h"

void LCDML_DISP_returnFromInitScreen(int func) {
	if (LCDML.getFunction() != _LCDML_NO_FUNC) {
		bitSet(LCDML.control, _LCDML_control_funcend);
		g_lcdml_jump_func = func;
	}
	else {
		if (func == 255) {
			LCDML.goRoot();
		}
		else {
			LCDML.jumpToElement(func);
		}
		LCDML_DISP_update_menu();
	}
	LCDML_BUTTON_resetAll();
}

uint32_t gMillisTimer;	// used for diverse timer countdown
bool gSensorReset;

initScreen_t gInitScreen;
uint8_t gInitScreenPrevID;	// ID of function or element, type defined in gInitScreen (!= OFF)

int8_t gSelectedMenu;
int8_t gPrevSelectedEntry;
int8_t gSelectedEntry;
int8_t gMenuConfirm;

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_mqtt_toggle)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_mqtt_toggle)
{
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_mqtt_toggle)
{
	char text1[30];
	char text2[30];

	// Serial.printf("mqtt toggle: %i\n", LCDML.getFunction());
	
	snprintf(text1, 30, "%s", "home/2og/l1");
	snprintf(text2, 30, "%s", "t");
	
	mqttClient.publish(text1, 0, true, text2);

	LCDML_DISP_resetIsTimer();
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_mqtt_dimmer)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_mqtt_dimmer)
{
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_mqtt_dimmer)
{
	LCDML_DISP_resetIsTimer();
}
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
	LCDML_DISP_resetIsTimer();
	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_overview)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_min_max)
{
	gSensorReset = false;
	display.updatePrintBufferScr2();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_min_max)
{
	int16_t timeToGo;
	static bool ledOn;

	if (LCDML_BUTTON_checkLeft()) {
		gMillisTimer = millis();
		gSensorReset = true;
		ledOn = true;  digitalWrite(LED_R, !ledOn);
		LCDML_BUTTON_resetAll();
	}

	if (gSensorReset) {
		timeToGo = RESET_TIMER - (millis() - gMillisTimer);
		if (timeToGo <= 0) {
			sensors.resetMinMaxAccelGyro();
			gSensorReset = false;
			digitalWrite(LED_R, HIGH); // off
		}
		else {
			display.updateDisplayScr11(timeToGo);
			if (timeToGo < 1000) {
				ledOn = !ledOn; digitalWrite(LED_R, !ledOn);
			}
		}
	}
	else {
		display.updatePrintBufferScr2();
		display.updateDisplayWithPrintBuffer();

		LCDML_DISP_resetIsTimer();

		if (LCDML_BUTTON_checkEnter()) {
			LCDML_BUTTON_resetAll();
			LCDML_DISP_funcend();
		}
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_sensor_min_max)
{
	digitalWrite(LED_R, HIGH); // off
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_sensor_temp_press_alt)
{
	display.updatePrintBufferScr3();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_sensor_temp_press_alt)
{
	display.updatePrintBufferScr3();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
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

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
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
	gSensorReset = false;

	display.updateDisplayScr9(absAccel, maxAbsAccel);

	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_max_accel)
{
	float absAccel, maxAbsAccel;
	int16_t timeToGo;
	static bool ledOn;

	if (LCDML_BUTTON_checkLeft()) {
		gMillisTimer = millis();
		gSensorReset = true;
		ledOn = true;  digitalWrite(LED_R, !ledOn);
		LCDML_BUTTON_resetAll();
	}

	if (gSensorReset) {
		timeToGo = RESET_TIMER - (millis() - gMillisTimer);
		if (timeToGo <= 0) {
			sensors.resetMaxAbsAccel();
			gSensorReset = false;
			digitalWrite(LED_R, HIGH); // off
		}
		else {
			display.updateDisplayScr11(timeToGo);
			if (timeToGo < 1000) {
				ledOn = !ledOn;
				digitalWrite(LED_R, !ledOn);
			}
		}
	}
	else {
		sensors.getAbsAccel(&absAccel);
		sensors.getMaxAbsAccel(&maxAbsAccel);

		display.updateDisplayScr9(absAccel, maxAbsAccel);

		LCDML_DISP_resetIsTimer();

		if (LCDML_BUTTON_checkEnter()) {
			LCDML_BUTTON_resetAll();
			LCDML_DISP_funcend();
		}
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_max_accel)
{
	digitalWrite(LED_R, HIGH); // off
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_waterbubble)
{
	display.updateDisplayScr10();

	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_waterbubble)
{
	display.updateDisplayScr10();

	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
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
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_status_wlan)
{
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_status_mqtt)
{
	display.updatePrintBufferScr5();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_triggerMenu(DELAY_MS_5HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_status_mqtt)
{
	if (LCDML_BUTTON_checkLeft()) {
		LCDML_BUTTON_resetAll();
		if (conf.getMQTTEnabled())
			com.disableMqtt();
		else
			com.enableMqtt();
	}

	display.updatePrintBufferScr5();
	display.updateDisplayWithPrintBuffer();
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_status_mqtt)
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
				// no wraparound
				break;
			case INPUT_ALTITUDE:
				// input numer -> increase number
				gConfigAltitudeLarger += pow10(3 - gConfigAltitudePosition);
				gConfigAltitudeIncrease = true;

				if (gConfigAltitudeLarger > 9999)
					gConfigAltitudeLarger = 9999;

				if (gConfigAltitudeSmaller < gConfigAltitudeLarger)
					if (gConfigAltitudeCurrStep > 6)
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
				// no wraparound
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
bool gEEPROMResetDone;

void LCDML_DISP_setup(LCDML_FUNC_reset)
{
	gMillisTimer = millis();
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
	time_diff = millis() - gMillisTimer;
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
	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_resetIsTimer();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_ownerinformation)
{
}

// ############################################################################
clockModes_t gConfigClockMode;
void LCDML_DISP_setup(LCDML_FUNC_clock)
{
	gConfigClockMode = CLOCK_WALL;
	display.updateDisplayScr5(gConfigClockMode);
	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_clock)
{
	int tmp_mode = (int)gConfigClockMode;

	if (LCDML_BUTTON_checkUp()) {
		LCDML_BUTTON_resetUp();

		tmp_mode -= 1;
		if (tmp_mode < 0)
			tmp_mode = CLOCK_LAST - 1;
	}
	if (LCDML_BUTTON_checkDown()) {
		LCDML_BUTTON_resetDown();
		tmp_mode += 1;
		tmp_mode %= CLOCK_LAST;
	}
	gConfigClockMode = (clockModes_t)tmp_mode;

	display.updateDisplayScr5(gConfigClockMode);
	LCDML_DISP_resetIsTimer();

	if (LCDML_BUTTON_checkEnter()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_clock)
{
}

// ############################################################################
int gIdleLedBlink;		// bckw. cnt
bool gIdleLedBlinkOn;
#define IDLE_LED_BLINK_COUNTER	49
void LCDML_DISP_setup(LCDML_FUNC_initscreen)
{
	bool tmp = sensors.checkMotionIndicators();
	display.ssd1306_command(SSD1306_DISPLAYOFF);

	gIdleLedBlinkOn = false;
	gIdleLedBlink = IDLE_LED_BLINK_COUNTER;

	LCDML_DISP_triggerMenu(100);
}

void LCDML_DISP_loop(LCDML_FUNC_initscreen)
{
	if (gIdleLedBlinkOn) {
		digitalWrite((LCDML_DISP_isGroupEnabled(_LCDML_G1)) ? LED_G : LED_R, HIGH); // off
		gIdleLedBlinkOn = false;
		gIdleLedBlink = IDLE_LED_BLINK_COUNTER;
	}
	else {
		if (gIdleLedBlink == 0) {
			digitalWrite((LCDML_DISP_isGroupEnabled(_LCDML_G1)) ? LED_G : LED_R, LOW);	// on
			gIdleLedBlinkOn = true;
			gIdleLedBlink = 0;
		}
		else {
			gIdleLedBlink--;
		}
	}

	LCDML_DISP_resetIsTimer();

	if (sensors.checkMotionIndicators() || LCDML_BUTTON_checkAny()) {
		LCDML_BUTTON_resetAll();
		LCDML_DISP_funcend();
	}
}

void LCDML_DISP_loop_end(LCDML_FUNC_initscreen)
{
	display.ssd1306_command(SSD1306_DISPLAYON);
	digitalWrite((LCDML_DISP_isGroupEnabled(_LCDML_G1)) ? LED_G : LED_R, HIGH); // off
	gIdleLedBlinkOn = false;

	//Serial.printf("return from initscreen: PrevID = %i\n", gInitScreenPrevID);
	if (gInitScreenPrevID == 255)
		LCDML.goRoot();
	else
		switch (gInitScreen) {
		case INITSCREEN_FUNCTION:
			//LCDML_DISP_jumpToFunc(LCDML_FUNC_clock);
			LCDML_DISP_returnFromInitScreen(gInitScreenPrevID);
			break;
		case INITSCREEN_MENU:
			LCDML.jumpToElement(gInitScreenPrevID);
			break;
		default:
			break;
		}

	gInitScreen = INITSCREEN_OFF;
}

// ############################################################################
int16_t	gInputPinSmaller;
int16_t gInputPinLarger;
uint8_t gInputPinCurrStep;
bool gInputPinIncrease;
int8_t gInputPinPosition;
inputPin_t gInputPinMode;
bool gInputPinUnlock;

void LCDML_DISP_setup(LCDML_FUNC_unlock)
{
	gInputPinSmaller = gInputPinLarger = 0;
	gInputPinCurrStep = 10; // animation paused
	gInputPinPosition = 0;	// value of pos = 10^(3-pos)
	gInputPinMode = PIN_BUTTON_INPUT;
	gInputPinUnlock = false;

	display.updateDisplayScr12(gInputPinSmaller, gInputPinLarger, 0, true, 0, gInputPinMode);

	LCDML_DISP_triggerMenu(DELAY_MS_20HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_unlock)
{
	if (LCDML_BUTTON_checkEnter()) {
		switch (gInputPinMode) {
		case PIN_BUTTON_INPUT:
			gInputPinMode = PIN_INPUT;
			break;
		case PIN_INPUT:
			gInputPinPosition++;
			if (gInputPinPosition > 3) {
				gInputPinPosition = 0;
				gInputPinMode = PIN_BUTTON_OK;
			}
			break;
		case PIN_BUTTON_OK:
			if (gInputPinLarger == SPBOX_PIN)
				gInputPinUnlock = true;

			if (gInputPinUnlock) {
				LCDML_BUTTON_resetAll();
				LCDML_DISP_resetIsTimer();
				LCDML_DISP_funcend();
			}

			//Serial.printf("PIN entered: %i, unlock: %i\n", gInputPinLarger, gInputPinUnlock);
			break;
		case PIN_BUTTON_CANCEL:
			LCDML_BUTTON_resetAll();
			LCDML_DISP_resetIsTimer();
			LCDML_DISP_funcend();
			break;
		default:
			break;
		}

		display.updateDisplayScr12(gInputPinSmaller, gInputPinLarger, gInputPinCurrStep, gInputPinIncrease, gInputPinPosition, gInputPinMode);
	}
	else
	{
		if (LCDML_BUTTON_checkUp()) {
			switch (gInputPinMode) {
			case PIN_BUTTON_INPUT:
				// no wraparound
				break;
			case PIN_INPUT:
				gInputPinLarger = changeSingleDigit(gInputPinLarger, gInputPinPosition, +1, false);
				gInputPinIncrease = true;

				if (gInputPinSmaller < gInputPinLarger)
					if (gInputPinCurrStep > 6)
						gInputPinCurrStep = 0;
				//Serial.printf("pin, checkUp:\t<%i >%i, pos %i\n", gInputPinSmaller, gInputPinLarger, gInputPinPosition);
				break;
			case PIN_BUTTON_OK:
				gInputPinMode = PIN_BUTTON_INPUT;
				break;
			case PIN_BUTTON_CANCEL:
				gInputPinMode = PIN_BUTTON_OK;
				break;
			}
		}

		if (LCDML_BUTTON_checkDown()) {
			switch (gInputPinMode) {
			case PIN_BUTTON_INPUT:
				gInputPinMode = PIN_BUTTON_OK;
				break;
			case PIN_INPUT:
				gInputPinSmaller = changeSingleDigit(gInputPinSmaller, gInputPinPosition, -1);
				gInputPinIncrease = false;

				if (gInputPinSmaller < gInputPinLarger)
					if (gInputPinCurrStep > 6)
						gInputPinCurrStep = 0;
				//Serial.printf("pin, checkDown:\t<%i >%i, pos %i\n", gInputPinSmaller, gInputPinLarger, gInputPinPosition);
				break;
			case PIN_BUTTON_OK:
				gInputPinMode = PIN_BUTTON_CANCEL;
				break;
			case PIN_BUTTON_CANCEL:
				// no wraparound
				break;
			}
		}

		switch (gInputPinCurrStep) {
		case 9: // reinitialize
			if (gInputPinIncrease)
				gInputPinSmaller = gInputPinLarger;
			else
				gInputPinLarger = gInputPinSmaller;
			gInputPinCurrStep++;
			break;
		case 10: // pause
			display.updateDisplayScr12(gInputPinSmaller, gInputPinLarger, gInputPinCurrStep, gInputPinIncrease, gInputPinPosition, gInputPinMode);
			break;
		default: // scrolled text steps 0..8
			display.updateDisplayScr12(gInputPinSmaller, gInputPinLarger, gInputPinCurrStep, gInputPinIncrease, gInputPinPosition, gInputPinMode);
			gInputPinCurrStep++;
			break;
		}
	}
	LCDML_BUTTON_resetAll();
}

void LCDML_DISP_loop_end(LCDML_FUNC_unlock)
{
	if (gInputPinUnlock) {
		LCDML_DISP_groupEnable(_LCDML_G1);
		LCDML_DISP_groupDisable(_LCDML_G2);

		LCDML_DISP_resetIsTimer();
		LCDML.goRoot();
	}
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_lock)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_lock)
{
	display.clearDisplay();
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_lock)
{
	LCDML_DISP_resetIsTimer();
	LCDML_DISP_groupEnable(_LCDML_G2);
	LCDML_DISP_groupDisable(_LCDML_G1);
	LCDML.goRoot();
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_toggle_conf_wlan)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_toggle_conf_wlan)
{
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_toggle_conf_wlan)
{
	bool result;
	conf.setWlanEnabled(!conf.getWlanEnabled());
	result = conf.writeConfToEEPROM();

	LCDML_DISP_resetIsTimer();
}

// todo
// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_select_wlan_profile)
{
	gSelectedMenu = gPrevSelectedEntry = gSelectedEntry = conf.getWifiMode();
	gMenuConfirm = false;

	display.updateDisplayScr14_wifi(wifiProfiles, gPrevSelectedEntry, gSelectedMenu, gMenuConfirm, wifiProfiles[gSelectedMenu].name);

	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_select_wlan_profile)
{
	if (!gMenuConfirm) {
		if (LCDML_BUTTON_checkEnter())
		{
			if (gSelectedMenu == NUM_WIFI_PROFILES) {		// "back" button
				LCDML_BUTTON_resetAll();
				LCDML_DISP_resetIsTimer();
				LCDML_DISP_funcend();
			}
			else {
				LCDML_BUTTON_resetEnter();
				LCDML_DISP_resetIsTimer();
				gSelectedEntry = gSelectedMenu;
				gSelectedMenu = 0;
				gMenuConfirm = true;
			}
		}
		if (LCDML_BUTTON_checkUp()) {
			LCDML_BUTTON_resetUp();
			if (gSelectedMenu)
				gSelectedMenu--;
		}
		if (LCDML_BUTTON_checkDown()) {
			LCDML_BUTTON_resetDown();
			if (gSelectedMenu < NUM_WIFI_PROFILES)
				gSelectedMenu++;
		}
	}
	else {
		if (LCDML_BUTTON_checkEnter())
		{
			if (gSelectedMenu == 0) {
				// new entry confirmed
				LCDML_BUTTON_resetAll();
				LCDML_DISP_funcend();
			}
			else
			{
				LCDML_BUTTON_resetAll();
				gPrevSelectedEntry = gSelectedEntry = conf.getWifiMode();
				gSelectedMenu = 0;
				gMenuConfirm = false;
			}
		}
		if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
			LCDML_BUTTON_resetAll();
			if (gSelectedMenu != 0)
				gSelectedMenu = 0;
			else
				gSelectedMenu = 1;
		}
	}
	display.updateDisplayScr14_wifi(wifiProfiles, gPrevSelectedEntry, gSelectedMenu, gMenuConfirm, wifiProfiles[gSelectedEntry].name);

	LCDML_BUTTON_resetAll();
	LCDML_DISP_resetIsTimer();
}

void LCDML_DISP_loop_end(LCDML_FUNC_select_wlan_profile)
{
	bool result;
	Serial.printf("select wifi: confirmed %i, prev: %i, new: %i, name: %s\n", gMenuConfirm, gPrevSelectedEntry, gSelectedEntry, wifiProfiles[gSelectedEntry].name);

	if (gMenuConfirm && (gPrevSelectedEntry != gSelectedEntry)) {
		conf.setWifiMode((WifiAPProfile_t) gSelectedEntry);
		result = conf.writeConfToEEPROM();
		Serial.printf("wifi: new entry: %i, eeprom write result: %i\n", gSelectedEntry, result);
		com.changeWifiProfile();
	}
	else
	{
		Serial.print("wifi: no new wifi selected, config unchanged\n");
	}
	LCDML_DISP_resetIsTimer();
	//LCDML.goBack();
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_toggle_mqtt)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_toggle_mqtt)
{
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_toggle_mqtt)
{
	bool result;
	conf.setMQTTEnabled(!conf.getMQTTEnabled());
	result = conf.writeConfToEEPROM();

	LCDML_DISP_resetIsTimer();
}

// ############################################################################
void LCDML_DISP_setup(LCDML_FUNC_toggle_mqtthealth)
{
	LCDML_DISP_triggerMenu(DELAY_MS_10HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_toggle_mqtthealth)
{
	LCDML_BUTTON_resetAll();
	LCDML_DISP_funcend();
}

void LCDML_DISP_loop_end(LCDML_FUNC_toggle_mqtthealth)
{
	bool result;
	conf.setMQTTHealthdata(!conf.getMQTTHealthdata());
	result = conf.writeConfToEEPROM();

	LCDML_DISP_resetIsTimer();
}
// ############################################################################

void LCDML_DISP_setup(LCDML_FUNC_select_mqtt)
{
	gSelectedMenu = gPrevSelectedEntry = gSelectedEntry = conf.getMqttConfigNr();
	gMenuConfirm = false;

	display.updateDisplayScr14_mqtt(mqttConfigs, gPrevSelectedEntry, gSelectedMenu, gMenuConfirm, mqttConfigs[gSelectedMenu].name);

	LCDML_DISP_triggerMenu(DELAY_MS_1HZ);
}

void LCDML_DISP_loop(LCDML_FUNC_select_mqtt)
{
	if (!gMenuConfirm) {
		if (LCDML_BUTTON_checkEnter())
		{
			if (gSelectedMenu == NUM_MQTT_CONFIG) {		// "back" button
				LCDML_BUTTON_resetAll();
				LCDML_DISP_resetIsTimer();
				LCDML_DISP_funcend();
			}
			else {
				LCDML_BUTTON_resetEnter();
				LCDML_DISP_resetIsTimer();
				gSelectedEntry = gSelectedMenu;
				gSelectedMenu = 0;
				gMenuConfirm = true;
			}
		}
		if (LCDML_BUTTON_checkUp()) {
			LCDML_BUTTON_resetUp();
			if (gSelectedMenu)
				gSelectedMenu--;
		}
		if (LCDML_BUTTON_checkDown()) {
			LCDML_BUTTON_resetDown();
			if (gSelectedMenu < NUM_MQTT_CONFIG)
				gSelectedMenu++;
		}
	}
	else {
		if (LCDML_BUTTON_checkEnter())
		{
			if (gSelectedMenu == 0) {
				// new entry confirmed
				LCDML_BUTTON_resetAll();
				LCDML_DISP_funcend();
			}
			else
			{
				LCDML_BUTTON_resetAll();
				gPrevSelectedEntry = gSelectedEntry = conf.getMqttConfigNr();
				gSelectedMenu = 0;
				gMenuConfirm = false;
			}
		}
		if (LCDML_BUTTON_checkUp() || LCDML_BUTTON_checkDown()) {
			LCDML_BUTTON_resetAll();
			if (gSelectedMenu != 0)
				gSelectedMenu = 0;
			else
				gSelectedMenu = 1;
		}
	}
	display.updateDisplayScr14_mqtt(mqttConfigs, gPrevSelectedEntry, gSelectedMenu, gMenuConfirm, mqttConfigs[gSelectedEntry].name);

	LCDML_BUTTON_resetAll();
	LCDML_DISP_resetIsTimer();
}

void LCDML_DISP_loop_end(LCDML_FUNC_select_mqtt)
{
	bool result;
	Serial.printf("select mqtt: confirmed %i, prev: %i, new: %i, name: %s\n", gMenuConfirm, gPrevSelectedEntry, gSelectedEntry, mqttConfigs[gSelectedEntry].name);

	if (gMenuConfirm && (gPrevSelectedEntry != gSelectedEntry)) {
		conf.setMqttConfigNr(gSelectedEntry);
		result = conf.writeConfToEEPROM();
		Serial.printf("mqtt: new entry: %i, eeprom write result: %i\n", gSelectedEntry, result);
		com.changeMqttBroker();
	}
	else
	{
		Serial.print("mqtt: no new broker selected, config unchanged\n");
	}
	LCDML_DISP_resetIsTimer();
	//LCDML.goBack();
}

// ############################################################################
// ############################################################################
// ############################################################################
// ############################################################################
// Test functions
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
	g_lcdml_initscreen;

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