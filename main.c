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
    // startTimer1(10, (uint32) onTimerFire); // every 1 second
    //init_irq(1000,00);
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