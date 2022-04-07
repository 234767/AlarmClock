#include "timer.h"
#include "display.h"
#include "joystick.h"

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

void init_irq (int32 period, int8 duty_cycle) {
    VICIntSelect &= ~0x20;
    VICVectAddr5 = (int32)onTimerFire;
    VICVectCntl5  = 0x25;            //enable vector interrupt for timer1 
    VICIntEnable  = 0x20;            //enable timer1 interrupt
  
    //initialize and start Timer #1
    T1TCR = 0x00000002;                           //disable and reset Timer1
    T1PC  = 0;                                    // no prescale
    T1MR0 = ((unsigned long long) period ) * ((unsigned long long) CRYSTAL_FREQUENCY )/1000;
    T1MR1 = (unsigned long long)T1MR0 * duty_cycle / 100;
    T1IR  = 0x000000ff;                           //reset all flags before enable IRQs
    T1MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
    T1TCR = 0x00000001;   
}

void handleModeO(){
    if(isButtonPressed()){
        IO0SET = (1 << 13);
        clearDisplay();
        displayTime();
        SetCursor(1,1);
        delayMs(500,0); // half second delay
        if (isButtonPressed()) {
            LcdPrint("looong");
        }
        else {
            LcdPrint("shrt");
        }
        delay2ms();
        IO0CLR = (1 << 13);
    }
}

int main(void) {
    led12 = 0;
    IODIR0 |= ((1 << 12) | (1<<13));
    currentTime.hour = 0;
    currentTime.minute = 0;
    currentTime.second = 0;
    DisplayInit();
    // startTimer1(10, (uint32) onTimerFire); // every 1 second
    init_irq(1000,500);
    while(1){
        
        
    }
    return 0;
}