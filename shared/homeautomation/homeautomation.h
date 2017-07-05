// homeautomation.h

#ifndef _HOMEAUTOMATION_h
#define _HOMEAUTOMATION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class HOMEAUTOMATION
{
 public:
	void init();

 private:

};

extern HOMEAUTOMATION Homeautomation;

#endif

