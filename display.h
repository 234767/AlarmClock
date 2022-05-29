#include "lpc214x.h"
#include "util.h"

#define LCD_DATA      0x00ff0000  //P1.16-P1.23
#define LCD_E         0x02000000  //P1.25
#define LCD_RW        0x00400000  //P0.22
#define LCD_RS        0x01000000  //P1.24
#define LCD_BACKLIGHT 0x40000000  //P0.30

void DisplayInit(void);
void LcdCommand(unsigned char byte);
void LcdData(unsigned char byte);
void SetCursor(unsigned int line, unsigned int column);
void LcdPrint(char* str);

static void
writeLCD(uint8 reg, uint8 data)
{
	volatile uint8 i;

	if (reg == 0)
	  IOCLR1 = LCD_RS;
	else
	  IOSET1 = LCD_RS;

    IOCLR0 = LCD_RW;
	IOCLR1 = LCD_DATA;
	IOSET1 = ((uint32)data << 16) & LCD_DATA;
	
	IOSET1 = LCD_E;

	for(i=0; i<16; i++)
        asm volatile (" nop"); //15ns x 16 = about 250 ns
	IOCLR1 = LCD_E;

	for(i=0; i<16; i++)
        asm volatile (" nop"); //15ns x 16 = about 250 ns
    delay2ms();
}

void LcdCommand(unsigned char byte){
    writeLCD(0, byte);
}

void LcdData(unsigned char byte){
    writeLCD(1, byte);
}

void clearDisplay() {
    LcdCommand(0x01); 
    delay2ms();
}

static void
lcdBacklight(uint8 onOff)
{
	if (onOff == TRUE)
	  IOSET0 = LCD_BACKLIGHT;
	else
	  IOCLR0 = LCD_BACKLIGHT;
}

void DisplayInit(void) {
	IODIR1 |= (LCD_DATA | LCD_E | LCD_RS);
	IOCLR1  = (LCD_DATA | LCD_E | LCD_RS);

	IODIR0 |= LCD_RW;
	IOCLR0  = LCD_RW;
	
    IODIR0 |= LCD_BACKLIGHT;
    IOCLR0  = LCD_BACKLIGHT;

    LcdCommand(0x30);
    delay2ms();
    LcdCommand(0x30);
    delay37us();
    LcdCommand(0x30);
    delay37us();

    LcdCommand(0x38); // set 8-bit, 2 line mode
    delay37us();

    LcdCommand(0x08); // display off
    delay37us();

    clearDisplay();

    LcdCommand(0x06); // cursor direction - increment, no shift
    delay2ms();

    LcdCommand(0x0c); // display on, cursor off
    delay2ms();

    LcdCommand(0x02); // cursor to home position
    delay2ms();
}

void cursor_enable(unsigned char on) {
    if (on == FALSE) {
        LcdCommand(0x0c);
    }
    else {
        LcdCommand(0x0e);
    }
    delay37us();
} 

void SetCursor(unsigned int line, unsigned int column){
    if (column > 39)
        column = 39;

    if (line == 0)
        LcdCommand(0x80 + column);
    else 
        LcdCommand(0xc0 + column);
}

void LcdPrint(char* str){
    while(*str){
        LcdData(*str);
        delay37us();
        str++;
    }
}