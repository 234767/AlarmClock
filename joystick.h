#define KEYPIN_NOTHING 0x001F0000
#define KEYPIN_CENTER  0x00010000 // P0.16
#define KEYPIN_UP      0x00020000 // P0.17
#define KEYPIN_RIGHT   0x00040000 // P0.18
#define KEYPIN_LEFT    0x00080000 // P0.19
#define KEYPIN_DOWN    0x00100000 // P0.20
#define PIN_BUTTON     0x00004000 // P0.14


char getYoyInput(void);
char waitForJoyInput(void);
unsigned char isButtonPressed(void);