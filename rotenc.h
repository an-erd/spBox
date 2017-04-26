// rotenc.h

#ifndef _ROTENC_h
#define _ROTENC_h

extern "C" {
#include "user_interface.h"
#include <os_type.h>
}
#include <functional>
using namespace std;
using namespace placeholders;
#include "arduino.h"
#include "user_config.h"

typedef enum {
	CCW = -1,
	CW = 1
} rotencChangeEvent_t;

typedef struct {
	rotencChangeEvent_t event;
	int diff;
	long pos;
} rotencPosEvent_t;

typedef std::function<void(rotencChangeEvent_t)> onRotencChangeEvent_t;
typedef std::function<void(rotencPosEvent_t)>    onRotencPosEvent_t;

class ROTENC
{
public:
	ROTENC();
	void		initialize();
	void		start();			// attach interrupt and variable initialization
	void		stop();				// detach interrupt

	bool		check();			// call in loop() for events to create

	void		onRotencChangeEvent(onRotencChangeEvent_t handler);
	void		onRotencPosEvent(onRotencPosEvent_t    handler);

	void		isrInt0();				// only for isr wrapper
	void		isrInt1();
private:
	volatile uint32_t	int0_time_;			// ISR threshold
	volatile uint32_t	int1_time_;
	volatile uint8_t	int0_signal_;
	volatile uint8_t	int0_history_;
	volatile uint8_t	int1_signal_;
	volatile uint8_t	int1_history_;
	volatile long		rotary_half_steps_;

	volatile bool		changed_halfSteps_;
	volatile long		actualRotaryTicks;

	bool		changed_;

	long		LCDML_rotenc_value;
	long		LCDML_rotenc_value_history;

	volatile bool		changed__;
protected:
	onRotencChangeEvent_t onChangeEvent;
	onRotencPosEvent_t    onPosEvent;
};

extern ROTENC rotenc;
#endif
