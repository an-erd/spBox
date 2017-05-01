// missing_str_util.h

#ifndef _MISSING_STR_UTIL_h
#define _MISSING_STR_UTIL_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

char * dtostrf_sign(double number, signed char width, unsigned char prec, char *s);

#endif
