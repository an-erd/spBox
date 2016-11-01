# spBox

I2C devices
===========
address 0x3C 	Adafruit Featherwing OLED 
address 0x1E	GY-87/HMC5883L 	magnetometer
address 0x68	GY-87/MPU6050 	3-axis gyroscope and a 3-axis accelerometer
address 0x77	GY-87/BMP180 	barometric pressure/temperature/altitude

Sample (debug) output line
==========================
a/g:	0.70	0.46	0.59	-174	117	13	mag:	-108	-384	-530	heading:	215.92	T/P/A	21.30	96817.00	382.27
display.println("abcdefghijklmnopqrstu");
display.println("a:  +0.70 +0.46 +0.59");
display.println("H: 215 T: 21.3");