// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!
#include <stdlib.h>
#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "HMC5883L.h"
#include "BMP085.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

MPU6050           accelgyro;
HMC5883L          mag;
BMP085            barometer;
Adafruit_SSD1306  display = Adafruit_SSD1306();

// store accel and gyro values from MPU6050, and accel using unit "g"(float)
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t mx, my, mz;
float   ax_f, ay_f, az_f;

float   temperature;
float   pressure;
float   altitude;
int32_t lastMicros;

#if defined(ESP8266)
#define BUTTON_A 0
#define BUTTON_B 16
#define BUTTON_C 2
#define LED      0
#elif defined(ARDUINO_STM32F2_FEATHER)
#define BUTTON_A PA15
#define BUTTON_B PC7
#define BUTTON_C PC5
#define LED PB5
#else
#define BUTTON_A 9
#define BUTTON_B 6
#define BUTTON_C 5
#define LED      13
#endif

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// rotary encoder PINs
#define encoder0PinA	12 
#define encoder0PinB	14

// store encoder values
volatile unsigned long threshold = 10000;
volatile long rotaryHalfSteps = 0;
// Working variables for the interrupt routines
volatile unsigned long int0time = 0;
volatile unsigned long int1time = 0;
volatile uint8_t int0signal = 0;
volatile uint8_t int1signal = 0;
volatile uint8_t int0history = 0;
volatile uint8_t int1history = 0;


// example 2 -------------
volatile unsigned int encoder0Pos = 0;
unsigned int tmp = 0;
unsigned int Aold = 0;
unsigned int Bnew = 0; 




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


void setup() {
#if !defined(ESP8266)
  while (!Serial) delay(1);
#endif

  Serial.begin(9600);
  Wire.begin();

  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();

  accelgyro.setI2CMasterModeEnabled(false);
  accelgyro.setI2CBypassEnabled(true) ;
  accelgyro.setSleepEnabled(false);

  // initialize device
  Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  mag.initialize();
  Serial.println(mag.testConnection() ? "HMC5883L connection successful" : "HMC5883L connection failed");
  barometer.initialize();
  Serial.println(barometer.testConnection() ? "BMP180 connection successful" : "BMP180 connection failed");

  // initialize rotary encoder
  Serial.println("Initializing Rotary Encoder...");
  pinMode(encoder0PinA, INPUT_PULLUP);
  pinMode(encoder0PinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoder0PinA), int0, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoder0PinB), int1, CHANGE);
  Serial.println("... initializing rotary encoder done.");
}


/* interrupt rot enc example 1
void int0() {
	if (micros() - int0time < threshold)
		return;
	int0history = int0signal;
	int0signal = digitalRead(encoder0PinA);
	if (int0history == int0signal)
		return;
	int0time = micros();
	if (int0signal == int1signal)
		rotaryHalfSteps++;
	else
		rotaryHalfSteps--;
}

void int1() {
	if (micros() - int1time < threshold)
		return;
	int1history = int1signal;
	int1signal = digitalRead(encoder0PinB);
	if (int1history == int1signal)
		return;
	int1time = micros();
}
*/

/* interrupt rot enc example 2 */
void int0() {
	Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
	Aold = digitalRead(encoder0PinA);
}
// Interrupt on B changing state
void int1() {
	Bnew = digitalRead(encoder0PinB);
	Bnew^Aold ? encoder0Pos++ : encoder0Pos--;
}




void loop() {
  char displaybuffer[4][21];  // 4 lines with 21 chars each
  char tempbuffer[3][15];     // temp for float to str conversion
  long actualRotaryTicks; 

  /*
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  mag.getHeading(&mx, &my, &mz);

  // request temperature
  barometer.setControl(BMP085_MODE_TEMPERATURE);

  // wait appropriate time for conversion (4.5ms delay)
  lastMicros = micros();
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

  // read calibrated temperature value in degrees Celsius
  temperature = barometer.getTemperatureC();

  // request pressure (3x oversampling mode, high detail, 23.5ms delay)
  barometer.setControl(BMP085_MODE_PRESSURE_3);
  while (micros() - lastMicros < barometer.getMeasureDelayMicroseconds());

  // read calibrated pressure value in Pascals (Pa)
  pressure = barometer.getPressure();

  // calculate absolute altitude in meters based on known pressure
  // (may pass a second "sea level pressure" parameter here,
  // otherwise uses the standard value of 101325 Pa)
  altitude = barometer.getAltitude(pressure);

  ax_f = ax / 16384.0;
  ay_f = ay / 16384.0;
  az_f = az / 16384.0;

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

  // To calculate heading in degrees. 0 degree indicates North
  float heading = atan2(my, mz);
  if (heading < 0)
    heading += 2 * M_PI;
  heading *= 180 / M_PI;
  Serial.print("heading:\t");
  Serial.print(heading); Serial.print("\t");

  // display measured values if appropriate
  Serial.print("T/P/A\t");
  Serial.print(temperature); Serial.print("\t");
  Serial.print(pressure); Serial.print("\t");
  Serial.print(altitude);
  */ 

  Serial.print("Rot.enc\t");
  //actualRotaryTicks = (rotaryHalfSteps / 2);
  //Serial.print(actualRotaryTicks);
  Serial.print(encoder0Pos);
  Serial.println("");

  /*
  dtostrf_sign(ax_f, 4, 2, tempbuffer[0]);   // -x.x
  dtostrf_sign(ay_f, 4, 2, tempbuffer[1]);   // -x.x
  dtostrf_sign(az_f, 4, 2, tempbuffer[2]);   // -x.x
  snprintf(displaybuffer[0], 21, "A %s %s %s", tempbuffer[0], tempbuffer[1], tempbuffer[2]);

  snprintf(displaybuffer[1], 21, "G: %+4d %+4d %+4d", gx, gy, gz);

  dtostrf(heading, 3, 0, tempbuffer[0]);   // xxx
  snprintf(displaybuffer[2], 21, "H: %s", tempbuffer[0]);

  dtostrf(temperature, 5, 2, tempbuffer[0]);   // -xx.x
  dtostrf(pressure, 5, 2, tempbuffer[1]);   // -xx.x
  snprintf(displaybuffer[3], 21, "T: %s, P: %s", tempbuffer[0], tempbuffer[1]);

  //   Serial.println(displaybuffer[0]);
  //   Serial.println(displaybuffer[1]);
  //   Serial.println(displaybuffer[2]);
  //   Serial.println(displaybuffer[3]);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(displaybuffer[0]);
  //  display.println(displaybuffer[1]);
  display.println(displaybuffer[2]);
  //  display.println(displaybuffer[3]);
  delay(10);
  yield();
  display.display();
  */
  delay(100);
}
