#include "lpc2xxx.h"
#include "timer.h"
#include "display.h"
#include "joytick.h"
#include "timer2.h"
#include "rtc.h"
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
//
//struct time {
//    uint8 hour;
//    uint8 minute;
//    uint8 second;
//};
//typedef struct time Time;
//
//Time currentTime;
uint8 led12;
uint8 mode;
/*
0 - normal
1 - setting hour
2 - setting minute
3 - setting second
*/
 
 
RTC_Time setTime, alarmTime, currentTime;
 
static void displayTime(){
    SetCursor(0,0);
    LcdData((currentTime.hours / 10) + 0x30);
    LcdData((currentTime.hours % 10) + 0x30);
    LcdData(':');
    LcdData((currentTime.minutes / 10) + 0x30);
    LcdData((currentTime.minutes % 10) + 0x30);
    LcdData(':');
    LcdData((currentTime.seconds / 10) + 0x30);
    LcdData((currentTime.seconds % 10) + 0x30);
}
#if 0
static void onTimerFire(void) {
    if (led12 == 0) {
        IOCLR0 = (1 << 12);
        led12 = 1;
    }
    else {
        IOSET0 = (1 << 12);
        led12 = 0;
    }
 
    if (mode != 0) {
        return;
    }
    //increment time by 1 second
    currentTime.second++;
    if (currentTime.second >= 60){
        currentTime.second = 0;
        currentTime.minute++;
        if (currentTime.minute >= 60){
            currentTime.minute = 0;
            currentTime.hour++;
            if (currentTime.hour >= 24){
                currentTime.hour = 0;
            }
        }
    }
 
    //display current time
    displayTime();
}
 
static void init_irq (tU32 period, tU8 duty_cycle)
{
    clearDisplay();
    LcdPrint("InitIRQ");
    //Zainicjuj VIC dla przerwań od Timera #1
    VICIntSelect &= ~TIMER_1_IRQ;           //Przerwanie od Timera #1 przypisane do IRQ (nie do FIQ)
    VICVectAddr5  = (tU32)onTimerFire;         //adres procedury przerwania
    VICVectCntl5  = VIC_ENABLE_SLOT | TIMER_1_IRQ_NO;
    VICIntEnable  = TIMER_1_IRQ;            // Przypisanie i odblokowanie slotu w VIC od Timera #1
 
    T1TCR = TIMER_RESET;                    //Zatrzymaj i zresetuj
    T1PR  = 0;                              //Preskaler nieużywany
    T1MR0 = ((tU64)period)*((tU64)PERIPHERAL_CLOCK)/1000;
    T1MR1 = (tU64)T1MR0 * duty_cycle / 100; //Wypełnienie
    T1IR  = TIMER_ALL_INT;                  //Resetowanie flag przerwań
    T1MCR = MR0_I | MR1_I | MR0_R;          //Generuj okresowe przerwania dla MR0 i dodatkowo dla MR1
    T1TCR = TIMER_RUN;                      //Uruchom timer
    clearDisplay();
    LcdPrint("pomocyaaaaa");
    return;
}
#endif
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
 
//void increment() {
//    switch (mode)
//    {
//    case 1:
//        currentTime.hour += 1;
//        currentTime.hour %= 24;
//        break;
//    case 2:
//        currentTime.minute += 1;
//        currentTime.minute %= 60;
//        break;
//    case 3:
//        currentTime.second += 1;
//        currentTime.second %= 60;
//    default:
//        break;
//    }
//}
//
//void decrement() {
//    switch (mode)
//    {
//    case 1:
//        if (currentTime.hour == 0)
//            currentTime.hour = 23;
//        else
//            currentTime.hour -= 1;
//        break;
//    case 2:
//        if (currentTime.minute == 0)
//            currentTime.minute = 59;
//        else
//            currentTime.minute -= 1;
//        break;
//    case 3:
//        if (currentTime.second == 0)
//            currentTime.second = 59;
//        else
//            currentTime.second -= 1;
//        break;
//    default:
//        break;
//    }
//}
 
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
//    case 'u':
//        increment();
//        break;
//    case 'd':
//        decrement();
//        break;
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
	ILR = 0x0;    // disable RTC interrupts
	CCR = 0x02;   // reset Clock Tick Counter
	CCR = 0x00;
	CIIR = 0x00;  // disable Counter Increment Interrupt (seconds increment)
	AMR = 0x00;   // disable Alarm Mask Register
	// prescaler - enables getting integers from high frequency signal such as RTC
	// PCLK for LPC2148 - 15MHz
	// Prescaler Integer  = (PCLK/32768) – 1              => in our case 456   = 0x01C8
	// Prescaler Fraction = PCLK – ((PREINT + 1) * 32768) => in our case 25024 = 0x61C0
	PREINT = 1;
	PREFRAC = 2;
	// set starting time to 10:20:30
	setTime.seconds = 30;
	setTime.minutes = 20;
	setTime.hours = 10;
	RTC_SetTime(setTime);
    CIIR = 0x00; // enable Counter Increment Interrupt (seconds increment)
	// set alarm time to 10:20:50
	alarmTime.seconds = 50;
	alarmTime.minutes = 20;
	alarmTime.hours = 10;
	//RTC_AlarmTime(alarmTime);
    CCR = 0x01; // enable Cock Control Register
	IOCLR0 = 0x00000010;
    // ILR = 0x03; // enable RTC interrupts but idk if we want to use it
	// end of RTC init
	// RTC stores time even if device is disabled, so we can comment lines above
	// after setting the time or wrap this into a function
	// to retreive time
	currentTime = RTC_GetTime();
 
 
    led12 = 0;
    IODIR0 |= ((1 << 12) | (1<<13));
    mode = 0;
    DisplayInit();
    lcdBacklight(1);
    sdelay(1);
    LcdPrint("Siema2");
    sdelay(3);
    // startTimer1(10, (uint32) onTimerFire); // every 1 second
    //init_irq(1000,00);
    clearDisplay();
    LcdPrint("Startujemy3");
    while(1){
    	displayTime();
    	currentTime = RTC_GetTime();
        delay2ms();
    }
    return 0;
}