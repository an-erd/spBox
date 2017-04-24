// rotenc.h

#ifndef _ROTENC_h
#define _ROTENC_h

#ifdef ESP8266
extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#endif

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

typedef enum {
	ticksChanged,
	switchChanged
} ROTENCChangeEvent_t;

#ifdef ARDUINO_ARCH_ESP8266
#include <functional>
typedef std::function<void(ROTENCChangeEvent_t)> onChangeEvent_t;
#else
typedef void(*onChangeEvent_t)(ROTENCChangeEvent_t);
#endif

class ROTENC
{
public:
	ROTENC();
	void		initialize();
	void		checkRotaryEncoder();
	void		checkButton();

	void		onROTENCChangeEvent(onChangeEvent_t handler);
	void		isrInt0();
	void		isrInt1();
	void		isrInt2();
private:
	// rot enc
	volatile uint32_t	int0time;			// ISR threshold
	uint32_t	int1time;
	uint8_t		int0signal;
	uint8_t		int0history;
	uint8_t		int1signal;
	uint8_t		int1history;

	// switch
	uint32_t	int_time;
	uint8_t		int_signal;
	uint8_t		int_history;

	// rot enc
	long		rotaryHalfSteps;
	bool		changed_halfSteps;
	long		actualRotaryTicks;
	bool		changed_rotEnc;
	long		LCDML_rotenc_value;
	long		LCDML_rotenc_value_history;

	// switch
	bool		changed;
	bool		long_diff_change;		// long time gone since change
	bool		very_long_diff_change;	// very long time gone since change
	bool		LCDML_button_pressed;
protected:
	onChangeEvent_t onChangeEvent;
};

extern ROTENC rotenc;
#endif
