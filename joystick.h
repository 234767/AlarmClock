#define KEYPIN_NOTHING 0x001F0000
#define KEYPIN_CENTER  0x00010000 // P0.16
#define KEYPIN_UP      0x00020000 // P0.17
#define KEYPIN_RIGHT   0x00040000 // P0.18
#define KEYPIN_LEFT    0x00080000 // P0.19
#define KEYPIN_DOWN    0x00100000 // P0.20
#define PIN_BUTTON     0x00004000 // P0.14

#include <lpc2xxx.h>
#include "util.h"

char getJoyInput(void) {
    if (((IOPIN0 & KEYPIN_UP) == 0)) {
		return 'u';
	}
	if (((IOPIN0 & KEYPIN_DOWN) == 0)) {
		return 'd';
	}
	if (((IOPIN0 & KEYPIN_RIGHT) == 0)) {
		return 'r';
	}
	if (((IOPIN0 & KEYPIN_LEFT) == 0)) {
		return 'l';
	}
	if (((IOPIN0 & KEYPIN_CENTER) == 0)) {
		return 'c';
	}
    return 0;
}

/// Infinite loop untill joystick has input. Returns char:
/// u - up; d - down; l - left; r - right; c - center
char waitForJoyInput(void){
	while (1) {
		char result = getJoyInput();
		if (result != 0)
			return result;
		delay37us();
	}
}

uint8 isButtonPressed(void) {
	if ((IOPIN0 & PIN_BUTTON) == 0) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}
