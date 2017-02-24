# spBox

I2C devices
===========
address 0x3C 	Adafruit Featherwing OLED
address 0x1E	GY-87/HMC5883L 	magnetometer
address 0x68	GY-87/MPU6050 	3-axis gyroscope and a 3-axis accelerometer
address 0x77	GY-87/BMP180 	barometric pressure/temperature/altitude

PIN assignment
==============
#4		SDA
#5		SCL
#12		Rotary Encoder Pin A
#14		Rotary Encoder Pin B
GND		Rotary Encoder Pin C/GND

Software TIMER usage
====================
name	

Read sensors
============
  barometer.setControl(BMP085_MODE_TEMPERATURE);
  wait barometer.getMeasureDelayMicroseconds();
  temperature = barometer.getTemperatureC();

  barometer.setControl(BMP085_MODE_PRESSURE_3);
  wait barometer.getMeasureDelayMicroseconds();
  pressure = barometer.getPressure();



ARDUINO Libraries used
======================
i2cdevlib (see https://www.i2cdevlib.com/usage)
Adafruit GFX
Adafruit SSD1306

Serial Output
=============

a/g:	0.40	0.52	0.79	-233	141	6	mag:	149	-347	-532	heading:	0.00	T/P/A	23.00	95865.00	464.83Rot.enc	0
performance: 29829 2651
(first performance value: get sensor data
second value: output)

Sample (debug) output line
==========================
display.println("abcdefghijklmnopqrstu");
display.println("123456789012345678901");
display.println("a:  +0.70 +0.46 +0.59");
display.println("H: 215 T: 21.3");