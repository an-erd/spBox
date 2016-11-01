    #include <SPI.h>
    #include <Wire.h>
    #include <Adafruit_GFX.h>
    #include <Adafruit_SSD1306.h>


    Adafruit_SSD1306 display = Adafruit_SSD1306();

    #ifdef ARDUINO_SAMD_ZERO
    #define Serial SerialUSB
    #endif

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

      Serial.println("OLED test");
      // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
      display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
      // init done
      Serial.println("OLED begun");
     
      // Show image buffer on the display hardware.
      // Since the buffer is intialized with an Adafruit splashscreen
      // internally, this will display the splashscreen.
      display.display();
      delay(1000);

      // Clear the buffer.
      display.clearDisplay();
      display.display();
     
      Serial.println("IO test");

      pinMode(BUTTON_A, INPUT_PULLUP);
      pinMode(BUTTON_B, INPUT_PULLUP);
      pinMode(BUTTON_C, INPUT_PULLUP);

      // text display tests
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      display.println("abcdefghijklmnopqrstu");
      display.println("ABCDEFGHIJKLMNOPQRSTU");
      display.println("123456789012345678901");
      display.println("/-\|/-\|/-\|/-\|/-\|/");
    }


    void loop() {
//      if (! digitalRead(BUTTON_A)) display.print("A");
//      if (! digitalRead(BUTTON_B)) display.print("B");
//      if (! digitalRead(BUTTON_C)) display.print("C");
      if (! digitalRead(BUTTON_A)) {
        Serial.print("A");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("A-A-A");
      }
      if (! digitalRead(BUTTON_B)) {
        Serial.print("B");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("B-B-B");
      }
      if (! digitalRead(BUTTON_C)) {
        Serial.print("C");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("C-C-C");
      }

      delay(10);
      yield();
      display.display();
    }

