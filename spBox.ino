// I2Cdevlib:
// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!
//
// Rotary Encoder:
// The code for the rotary encoder has been copied from http://playground.arduino.cc/Main/RotaryEncoders, 
// Int0 & Int1 example using bitRead() with debounce handling and true Rotary Encoder pulse tracking, J.Carter(of Earth)
// 

#include <arduino.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1306.h"

// Buttons on the ADAFRUIT FeatherWing OLED, PRODUCT ID: 2900
#define BUTTON_A		0
#define BUTTON_B		16
#define BUTTON_C		2
#define LED				0

// rotary encoder 
#define encoderPinA		12 
#define encoderPinB		14

#define THRESHOLD		7	// debounce threshold in milliseconds

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

typedef struct 
{
	MPU6050		accelgyro;
	HMC5883L	mag;
	BMP085		barometer;

	int16_t		ax, ay, az;			// accel values (sensor)
	float		ax_f, ay_f, az_f;	// accel float values (calculated)
	
	int16_t		gx, gy, gz;			// gyro values (sensor)
	
	int16_t		mx, my, mz;			// magnetometer values (sensor)
	float		heading;			// calculated heading (calculated)

	float		temperature;		// temperature (sensor)
	float		pressure;			// pressure (sensor)
	float		altitude;			// altitude (sensor)
  
	bool		changed_AGM;		// flag -> accel/gyro/magnetometer changed
	bool		changed_TPA;		// flag -> temperature/pressure/altitude changed
} sGlobalSensors;

typedef struct 
{
	uint32_t	int0time;			// ISR threshold 
	uint32_t	int1time;			// ISR threshold
	uint8_t		int0signal;
	uint8_t		int0history;
	uint8_t		int1signal;
	uint8_t		int1history;
	long		rotaryHalfSteps;	// internal counter used for rot enc position

	long		actualRotaryTicks;	// rot enc position
	bool		changed_rotEnc;		// flag -> rotary encoder position changed 
} sGlobalRotEnc;

typedef struct 
{
  char displaybuffer[4][21];  // 4 lines with 21 chars each
	


} sGlobalDisplay;

sGlobalSensors	sensors;
sGlobalRotEnc	rotenc;
Adafruit_SSD1306  display = Adafruit_SSD1306();
sGlobalDisplay	display_struct;

// store accel and gyro values from MPU6050, and accel using unit "g"(float)
int32_t lastMicros;		// TODO
Adafruit_SSD1306  display = Adafruit_SSD1306();	// TODO


// timer functions
// os_timer_t read_BT;		// read barometer and temperature

// = dtostre() function experimental ============================
char * dtostrf_sign(double number, signed char width, unsigned char prec, char *s) {
    bool negative = false;

    if (isnan(number)) {
        strcpy(s, "nan");
        return s;
    }
    if (isinf(number)) {
        strcpy(s, "inf");
        return s;
    }

    char* out = s;

    int fillme = width; // how many cells to fill for the integer part
    if (prec > 0) {
        fillme -= (prec+1);
    }

    // Handle negative numbers
    if (number < 0.0) {
        negative = true;
        fillme--;
        number = -number;
    } else {
      fillme--;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    // I optimized out most of the divisions
    double rounding = 2.0;
    for (uint8_t i = 0; i < prec; ++i)
        rounding *= 10.0;
    rounding = 1.0 / rounding;

    number += rounding;

    // Figure out how big our number really is
    double tenpow = 1.0;
    int digitcount = 1;
    while (number >= 10.0 * tenpow) {
        tenpow *= 10.0;
        digitcount++;
    }

    number /= tenpow;
    fillme -= digitcount;

    // Pad unused cells with spaces
    while (fillme-- > 0) {
        *out++ = ' ';
    }

    // Handle negative sign
    if (negative) {
      *out++ = '-';
    } else {
      *out++ = ' ';
    }

    // Print the digits, and if necessary, the decimal point
    digitcount += prec;
    int8_t digit = 0;
    while (digitcount-- > 0) {
        digit = (int8_t)number;
        if (digit > 9) digit = 9; // insurance
        *out++ = (char)('0' | digit);
        if ((digitcount == prec) && (prec > 0)) {
            *out++ = '.';
        }
        number -= digit;
        number *= 10.0;
    }

    // make sure the string is terminated
    *out = 0;
    return s;
}
// ====================================================================

// rotary encoder interrupt routines
void int0() {
	if (millis() - int0time < THRESHOLD)
		return;
	int0history = int0signal;
	int0signal = digitalRead(encoderPinA);
	if (int0history == int0signal)
		return;
	int0time = millis();
	if (int0signal == int1signal)
		rotaryHalfSteps--;
	else
		rotaryHalfSteps++;
}

void int1() {
	if (millis() - int1time < THRESHOLD)
		return;
	int1history = int1signal;
	int1signal = digitalRead(encoderPinB);
	if (int1history == int1signal)
		return;
	int1time = millis();
}

void initialize_GPIO() {
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

}

void initialize_display() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void initialize_accelgyro() {
  accelgyro.setI2CMasterModeEnabled(false);
  accelgyro.setI2CBypassEnabled(true) ;
  accelgyro.setSleepEnabled(false);

  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
}

void initialize_mag() {
  mag.initialize();
  Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
}

void initialize_barometer() {
	barometer.initialize();
	Serial.println(barometer.testConnection() ? "BMP180 connection successful" : "BMP180 connection failed");
}

void initialize_rotary_encoder() {
  attachInterrupt(digitalPinToInterrupt(encoderPinA), int0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), int1, CHANGE);

  int0signal = digitalRead(encoderPinA);
  int0history = int0signal;
  
  int1signal = digitalRead(encoderPinB);
  int1history = int1signal;
}

void get_accelgyro()
{
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
}

void get_mag()
{
  mag.getHeading(&mx, &my, &mz);
}

void get_temperature()
{
  // request temperature
  barometer.setControl(BMP085_MODE_TEMPERATURE);

  // wait appropriate time for conversion (4.5ms delay)
  lastMicros = micros();
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

  // read calibrated temperature value in degrees Celsius
  temperature = barometer.getTemperatureC();
}

void get_pressure()
{
  // request pressure (3x oversampling mode, high detail, 23.5ms delay)
  barometer.setControl(BMP085_MODE_PRESSURE_3);
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

  // read calibrated pressure value in Pascals (Pa)
  pressure = barometer.getPressure();
}

void calc_accelgyro()
{
  ax_f = ax / 16384.0;
  ay_f = ay / 16384.0;
  az_f = az / 16384.0;
}

void calc_mag()
{
  // To calculate heading in degrees. 0 degree indicates North
  float heading = atan2(my, mz);
  if (heading < 0)
    heading += M_TWOPI;
  heading *= 180 / M_PI;

}

void calc_altitude()
{
  // calculate absolute altitude in meters based on known pressure
  // (may pass a second "sea level pressure" parameter here,
  // otherwise uses the standard value of 101325 Pa)
  altitude = barometer.getAltitude(pressure);
}

void setup() {
#if !defined(ESP8266)
  while (!Serial) delay(1);
#endif

  Serial.begin(115200);
  Wire.begin();

  // initialize devices
  initialize_GPIO();
  initialize_display();
  initialize_accelgyro();
  initialize_mag();
  initialize_barometer();
  initialize_rotary_encoder();
}


void loop() {
  char tempbuffer[3][15];     // temp for float to str conversion

  int32_t perfStopWatch;
  perfStopWatch = micros();
  
/*
  // display tab-separated accel/gyro x/y/z values
  Serial.print("a/g:\t");
  Serial.print(ax_f); Serial.print("\t");
  Serial.print(ay_f); Serial.print("\t");
  Serial.print(az_f); Serial.print("\t");
  Serial.print(gx); Serial.print("\t");
  Serial.print(gy); Serial.print("\t");
  Serial.print(gz); Serial.print("\t");

  Serial.print("mag:\t");
  Serial.print(mx); Serial.print("\t");
  Serial.print(my); Serial.print("\t");
  Serial.print(mz); Serial.print("\t");
*/
/*
  Serial.print("heading:\t");
  Serial.print(heading); Serial.print("\t");

  // display measured values if appropriate
  Serial.print("T/P/A\t");
  Serial.print(temperature); Serial.print("\t");
  Serial.print(pressure); Serial.print("\t");
  Serial.print(altitude);
 
  Serial.print("Rot.enc\t");
*/
  actualRotaryTicks = (rotaryHalfSteps / 2);
/*
  Serial.print(actualRotaryTicks);
  Serial.println("");

*/
  dtostrf_sign(ax_f, 4, 2, tempbuffer[0]);   // -x.x
  dtostrf_sign(ay_f, 4, 2, tempbuffer[1]);   // -x.x
  dtostrf_sign(az_f, 4, 2, tempbuffer[2]);   // -x.x
  snprintf(displaybuffer[0], 21, "A %s %s %s", tempbuffer[0], tempbuffer[1], tempbuffer[2]);
  
  snprintf(displaybuffer[1], 21, "G: %+4d %+4d %+4d", gx, gy, gz);

  dtostrf(heading, 3, 0, tempbuffer[0]);   // xxx
  dtostrf(temperature, 5, 2, tempbuffer[1]);   // -xx.x
  snprintf(displaybuffer[2], 21, "H: %s, T: %s", tempbuffer[0], tempbuffer[1]);

  dtostrf(altitude, 4, 0, tempbuffer[0]);   // xxxx
  dtostrf(pressure/100.0, 4, 0, tempbuffer[1]);   // xxxx
  snprintf(displaybuffer[3], 21, "Alt: %s, P: %s", tempbuffer[0], tempbuffer[1]);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(displaybuffer[0]);
  //  display.println(displaybuffer[1]);
  display.println(displaybuffer[2]);
  display.println(displaybuffer[3]);
  delay(10);
  yield();
  display.display();
 
  Serial.print("performance: ");
  Serial.println(micros() - perfStopWatch);

  delay(100);
}
