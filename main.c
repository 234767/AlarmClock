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

int main(void) {
    led12 = 0;
    IODIR0 |= ((1 << 12) | (1<<13));
    currentTime.hour = 0;
    currentTime.minute = 0;
    currentTime.second = 0;
    DisplayInit();
    startTimer1(10, (uint32) onTimerFire); // every 1 second
    while(1){
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
        }
        delay2ms();
        IO0CLR = (1 << 13);
    }
    return 0;
}