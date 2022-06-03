#include "lpc2xxx.h"
#include "timer.h"
#include "display.h"
#include "joytick.h"
#include "timer2.h"
#include "general.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <printf_init.h>
#include <consol.h>
#include <config.h>
#include "irq/irq_handler.h"
#include "timer.h"
#include "VIC.h"
 
#include "Common_Def.h"
 
struct time {
    uint8 hour;
    uint8 minute;
    uint8 second;
};
typedef struct time Time;
 
Time currentTime;
uint8 led12;
uint8 mode;
/*
0 - normal
1 - setting hour
2 - setting minute
3 - setting second
4 - alarm
*/
 
static void displayTime(){
    SetCursor(0,0);
    LcdData((currentTime.hour / 10) + 0x30);
    LcdData((currentTime.hour % 10) + 0x30);
    LcdData(':');
    LcdData((currentTime.minute / 10) + 0x30);
    LcdData((currentTime.minute % 10) + 0x30);
    LcdData(':');
    LcdData((currentTime.second / 10) + 0x30);
    LcdData((currentTime.second % 10) + 0x30);
}

void handleModeO(){
    if(isButtonPressed()){
        IOSET0 = (1 << 13);
        clearDisplay();
        displayTime();
        SetCursor(1,0);
        delayMs(500,0); // half second delay
        if (isButtonPressed()) {
            LcdPrint("Time set");
            cursor_enable(1);
            mode = 1;
            while(isButtonPressed())
                ;
        }
        else {
            LcdPrint("Pres longer");
        }
        delay2ms();
        IOCLR0 = (1 << 13);
    }
}
 
void increment() {
    switch (mode)
    {
    case 1:
        currentTime.hour += 1;
        currentTime.hour %= 24;
        break;
    case 2:
        currentTime.minute += 1;
        currentTime.minute %= 60;
        break;
    case 3:
        currentTime.second += 1;
        currentTime.second %= 60;
    default:
        break;
    }
}
 
void decrement() {
    switch (mode)
    {
    case 1:
        if (currentTime.hour == 0)
            currentTime.hour = 23;
        else
            currentTime.hour -= 1;
        break;
    case 2:
        if (currentTime.minute == 0)
            currentTime.minute = 59;
        else
            currentTime.minute -= 1;
        break;
    case 3:
        if (currentTime.second == 0)
            currentTime.second = 59;
        else
            currentTime.second -= 1;
        break;
    default:
        break;
    }
}
 
void handleTimeSet() {
    if (isButtonPressed()) {
        mode = 0;
        cursor_enable(0);
        clearDisplay();
        displayTime();
        while(isButtonPressed())
            ;
        return;
    }
    char input = getYoyInput();
    switch ( input)
    {
    case 0:
        return;
    case 'u':
        increment();
        break;
    case 'd':
        decrement();
        break;
    case 'r':
        if (mode < 3) mode++;
        break;
    case 'l':
        if (mode > 1) mode--;
        break;
    default:
        break;
    }
    displayTime();
    delayMs(1000,0);
}
 
static void sdelay (tU32 seconds)
{
    T0TCR = TIMER_RESET;                    //Zatrzymaj i zresetuj
    T0PR  = 12000000-1;             //jednostka w preskalerze
    T0MR0 = seconds;
    T0IR  = TIMER_ALL_INT;                  //Resetowanie flag przerwaĹ„
    T0MCR = MR0_S;                          //Licz do wartości w MR0 i zatrzymaj się
    T0TCR = TIMER_RUN;                      //Uruchom timer
 
    // sprawdź czy timer działa
    // nie ma wpisanego ogranicznika liczby pętli, ze względu na charakter procedury
    // proszę jednak pamiętać, że to jest wyjątek od reguły!
    while (T0TCR & TIMER_RUN)
    {
    }
}
 
int main(void) {
    
	// RTC Init
	// currentTime is used to retreive time from the RTC
	RTC_Time setTime, alarmTime, currentTime;
	ILR = 0x0;    // disable RTC interrupts 
	CCR = 0x02;   // reset Clock Tick Counter
	CCR = 0x00;
	CIIR = 0x00;  // disable Counter Increment Interrupt (seconds increment)
	AMR = 0x00;   // disable Alarm Mask Register

	// prescaler - enables getting integers from high frequency signal such as RTC
	// PCLK for LPC2148 - 15MHz
	// Prescaler Integer  = (PCLK/32768) – 1              => in our case 456   = 0x01C8
	// Prescaler Fraction = PCLK – ((PREINT + 1) * 32768) => in our case 25024 = 0x61C0
	//PREINT = 0x01C8; 
	//PREFRAC = 0x61C0;

    // Try this:
    PREINT = 449;
    PREFRAC = 0;

	// set starting time to 10:20:30 
	setTime.seconds = 30;
	setTime.minutes = 20;
	setTime.hours = 10;
	RTC_SetTime(setTime);

    CIIR = 0x01; // enable Counter Increment Interrupt (seconds increment)

	// set alarm time to 10:20:50
	alarmTime.seconds = 50;
	alarmTime.minutes = 20;
	alarmTime.hours = 10;
	RTC_AlarmTime(alarmTime);

    CCR = 0x01; // enable Cock Control Register
    // ILR = 0x03; // enable RTC interrupts but idk if we want to use it

	// end of RTC init
	// RTC stores time even if device is disabled, so we can comment lines above
	// after setting the time or wrap this into a function

	// to retreive time
	currentTime = RTC_GetTime();

    led12 = 0;
    IODIR0 |= ((1 << 12) | (1<<13));
    currentTime.hour = 0;
    currentTime.minute = 0;
    currentTime.second = 0;
    mode = 0;
    DisplayInit();
    lcdBacklight(1);
    sdelay(1);
    LcdPrint("Siema2");
    sdelay(3);
    clearDisplay();
    LcdPrint("Startujemy");
    sdelay(1);
    while(1){
        if (mode == 0) {
            handleModeO();
        }
        else {
            handleTimeSet();
        }
        delay2ms();
    }
    return 0;
}