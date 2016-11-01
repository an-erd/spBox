// The code makes use of I2Cdevlib (http://www.i2cdevlib.com/). All the thanks to the team for providing that great work!

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

   Serial.println("... initializing done.");
}

void loop() {
   
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
   Serial.print(gz);Serial.print("\t");
   
   Serial.print("mag:\t");
   Serial.print(mx); Serial.print("\t");
   Serial.print(my); Serial.print("\t");
   Serial.print(mz); Serial.print("\t");

  // To calculate heading in degrees. 0 degree indicates North
   float heading = atan2(my, mz);
   if(heading < 0)
     heading += 2 * M_PI;
   Serial.print("heading:\t");
   Serial.print(heading * 180/M_PI); Serial.print("\t");

   // display measured values if appropriate
   Serial.print("T/P/A\t");
   Serial.print(temperature); Serial.print("\t");
   Serial.print(pressure); Serial.print("\t");
   Serial.print(altitude);
   Serial.println("");
}
