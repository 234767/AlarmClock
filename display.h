#include "lpc214x.h"

#define ENABLE_PIN      (1 << 24)
#define REGISTER_SELECT (1 << 25)

void DisplayInit(void);
void LcdCommand(unsigned char byte);
void LcdData(unsigned char byte);
void SetCursor(unsigned int line, unsigned int column);
void Print(char* str);
void Delay(void) { for(int i=0; i<100; i++); }

void DisplayInit(void){
    IODIR1 = ENABLE_PIN | REGISTER_SELECT; 
    IOCLR1 = ENABLE_PIN | REGISTER_SELECT;

    // Set pins P1.16 - P1.23 to output and clear
    IODIR1 = 0x00FF0000;
    IOCLR1 = 0x00FF0000;

    Delay();
    LcdCommand(0x01); // clear display
    Delay();
    LcdCommand(0x02); // return home
    Delay();
    LcdCommand(0x01); // clear again
    Delay();

    LcdCommand(0b00111000); // Set 8-bit 2-line mode
}
