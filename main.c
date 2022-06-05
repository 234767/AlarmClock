#include "lpc2xxx.h"
#include "timer.h"
#include "display.h"
#include "joytick.h"
#include "timer2.h"
#include "rtc.h"
#include <lpc2xxx.h>
#include <printf_P.h>
#include <printf_init.h>
#include <consol.h>
#include <config.h>
#include "irq/irq_handler.h"
#include "timer.h"
#include "VIC.h"
#include "eeprom.h"
#include "i2c.h"
#include "buzzer.h"

uint8 led12;
uint8 mode;
/*
0 - normal
1 - setting alarm hour
2 - setting alarm minute
3 - setting alarm second
4 - alarm
5 ,6, 7- setting current time
*/

void save(tU8 string[]){
	eepromWrite(0x0000, string, 2);
	//eepromPoll() == I2C_CODE_OK;
}

void read(tU8 string[]){
	eepromPageRead(0x0000, string, 2);
}


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

static void displaySetTime() {
	SetCursor(0,0);
	LcdData((setTime.hours / 10) + 0x30);
	LcdData((setTime.hours % 10) + 0x30);
	LcdData(':');
	LcdData((setTime.minutes / 10) + 0x30);
	LcdData((setTime.minutes % 10) + 0x30);
	LcdData(':');
	LcdData((setTime.seconds / 10) + 0x30);
	LcdData((setTime.seconds % 10) + 0x30);
}

void handleModeO(){
    if(isButtonPressed()){
        IOSET0 = (1 << 13);
        clearDisplay();
        displayTime();
        SetCursor(1,0);
        sdelay(1); // half second delay

        if (isButtonPressed()) {
            LcdPrint("Alarm set");
            cursor_enable(1);
            mode = 1;
            setTime.hours = currentTime.hours;
            setTime.minutes = currentTime.minutes;
            while(isButtonPressed())
                ;
        }
        else {
        	LcdPrint("Time set");
			mode = 5;
			setTime.hours = currentTime.hours;
			setTime.minutes = currentTime.minutes;
			return;
        }
        delay2ms();
        IOCLR0 = (1 << 13);
    }
}

void increment() {
    switch (mode)
    {
    case 1:
    case 5:
    	setTime.hours += 1;
    	setTime.hours %= 24;
        break;
    case 2:
    case 6:
    	setTime.minutes += 1;
    	setTime.minutes %= 60;
        break;
    case 3:
    case 7:
    	setTime.seconds += 1;
    	setTime.seconds %= 60;
    default:
        break;
    }
}

void decrement() {
    switch (mode)
    {
    case 1:
    case 5:
        if (setTime.hours == 0)
        	setTime.hours = 23;
        else
        	setTime.hours -= 1;
        break;
    case 2:
    case 6:
        if (setTime.minutes == 0)
        	setTime.minutes = 59;
       else
    	   setTime.minutes -= 1;
        break;
    case 3:
    case 7:
        if (setTime.seconds == 0)
        	setTime.seconds = 59;
        else
        	setTime.seconds -= 1;
        break;
    default:
        break;
    }
}

void handleTimeSet() {
    if (isButtonPressed()) {

        cursor_enable(0);
        clearDisplay();
        displaySetTime();
        while(isButtonPressed())
            ;
        if (mode >= 1 && mode <= 3){
        	// save to eeprom
        	tU8 zapis[2];
			zapis[0] = setTime.hours;
			zapis[1] = setTime.minutes;
			save(zapis);

			// set alarm
			alarmTime.hours = setTime.hours;
			alarmTime.minutes = setTime.minutes;
			alarmTime.seconds = 0;
        }
        else if (mode >= 5){
        	RTC_SetTime(setTime);
        }
        mode = 0;
        return;
    }
    char input = getJoyInput();
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
        if (mode < 3 || (mode >= 5 && mode < 7)) mode++;
        break;
    case 'l':
        if (mode > 1 || mode > 5) mode--;
        break;
    default:
        break;
    }
    displaySetTime();
    sdelay(1);
}

static void handleAlarm(){
	clearDisplay();
	LcdPrint("ALARM!");
	while(!isButtonPressed()){
		buzzer_on();
		sdelay(1);
		buzzer_off();
		sdelay(1);
	}
	while(isButtonPressed())//debounce
		;
	currentTime = RTC_GetTime();
	mode = 0;
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
	PREINT = 2000;
	PREFRAC = 50;

//	setTime.seconds = 0;
//	setTime.minutes = 00;
//	setTime.hours = 14;
//	RTC_SetTime(setTime);
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


    mode = 0;
    DisplayInit();
    lcdBacklight(1);
    sdelay(1);
    LcdPrint("Startujemy");
    sdelay(5);
    clearDisplay();
    tU8 odczyt[2];
    i2cInit();
    read(odczyt);

    alarmTime.seconds = 0;
	alarmTime.minutes = odczyt[1];
	alarmTime.hours = odczyt[0];

    init_buzzer();
    while(1){
    	if (mode == 0){
    		handleModeO();
			displayTime();
			currentTime = RTC_GetTime();
			delay2ms();

    	}
    	else if (mode == 4){
    		handleAlarm();
    	}
    	else {
    		handleTimeSet();
    	}
    	if (RTC_Compare(alarmTime, currentTime)) {
			mode = 4;
			continue;
		}

    }
    return 0;
}
