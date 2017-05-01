#pragma once

// Rotary Encoder and switch
#define ENCODER_PIN_A	12
#define ENCODER_PIN_B	14
#define ENCODER_SW		13

// Threshold
#define THRESHOLD		7		// rot enc and button debounce threshold (milliseconds)

// Timer delay constants in milliseconds(MS)
#define DELAY_MS_1HZ	1000
#define DELAY_MS_2HZ	500
#define DELAY_MS_5HZ	200
#define DELAY_MS_10HZ	100
#define DELAY_MS_TWOSEC	2000
#define DELAY_MS_TENSEC	10000
#define DELAY_MS_1MIN	60000

#define SERIAL_STATUS_OUTPUT
#define DBG_PORT Serial
#define DEBUG_ROTENC
#define DEBUG_BUTTON
