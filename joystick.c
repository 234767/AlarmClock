#include <lpc214x.h>
#include "joystick.h"
#include "util.h"

char getYoyInput(void) {
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