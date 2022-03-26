#include <lpc214x.h>
#include "joystick.h"
#include "util.h"

/// Infinite loop untill joystick has input. Returns char:
/// u - up; d - down; l - left; r - right; c - center
char getJoyInput(void){
	while (1) {
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
		Delay();
	}
}