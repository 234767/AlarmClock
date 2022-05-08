#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include "util.h"

/*
// If needed struct can also contain other values

typedef struct
{
	uint8 seconds;    // 0 - 59
	uint8 minutes;    // 0 - 59  
	uint8 hours;      // 0 - 23
	uint8 dayOfMonth; // 1 - (28, 29, 30, 31 depending on month)
	uint8 dayOfWeek;  // 0 - 6
	uint16 dayOfYear; // 1 - 365 (365 for leap year)
	uint8 month;      // 1 - 12
	uint16 year;      // 0 - 4095
}RTC_Time;
*/

typedef struct
{
	uint8 seconds;
	uint8 minutes;
	uint8 hours;
} RTC_Time;

void RTC_SetTime(RTC_Time Time)
{
	SEC = Time.seconds;
	MIN = Time.minutes;
	HOUR = Time.hours;
/*
	// setting time for other values if extended struct needed
	DOM = Time.dayOfMonth;
	DOW = Time.day_of_weedayOfWeekk;
	DOY = Time.dayOfYear;
	MONTH = Time.month;
	YEAR = Time.year;
*/
}

/*
	Alarm register group

	If all alarm registers match time registers, interrupt is generated

*/

void RTC_Set_AlarmTime(RTC_Time AlarmTime)
{
	ALSEC = AlarmTime.seconds;
	ALMIN = AlarmTime.minutes;
	ALHOUR = AlarmTime.hours;
	/*
	// setting alarm time for other values if extended struct needed
	ALDOM = Time.dayOfMonth;
	ALDOW = Time.day_of_weedayOfWeekk;
	ALDOY = Time.dayOfYear;
	ALMONTH = Time.month;
	ALYEAR = Time.year;
*/
}

RTC_Time RTC_GetTime(void)
{
	RTC_Time time;

	time.seconds = SEC;
	time.minutes = MIN;
	time.hours = HOUR;
	/*
	// returning time for other values if extended struct needed
	time.dayOfMonth = DOM;
	time.day_of_weedayOfWeekk = DOW;
	time.dayOfYear = DOY;
	time.month = MONTH;
	time.year = YEAR;
	*/

	return time;
}

RTC_Time RTC_GetAlarmTime(void)
{
	RTC_Time AlarmTime;

	AlarmTime.seconds = ALSEC;
	AlarmTime.minutes = ALMIN;
	AlarmTime.hours = ALHOUR;
	/*
	// returning alarm time for other values if extended struct needed
	AlarmTime.dayOfMonth = ALDOM;
	AlarmTime.day_of_weedayOfWeekk = ALDOW;
	AlarmTime.dayOfYear = ALDOY;
	AlarmTime.month = ALMONTH;
	AlarmTime.year = ALYEAR;
	*/

	return AlarmTime;
}

RTC_Time RTC_Init(void)
{
	RTC_Time AlarmTime;

	AlarmTime.seconds = ALSEC;
	AlarmTime.minutes = ALMIN;
	AlarmTime.hours = ALHOUR;
	/*
	// returning alarm time for other values if extended struct needed
	AlarmTime.dayOfMonth = ALDOM;
	AlarmTime.dayOfWeek = ALDOW;
	AlarmTime.dayOfYear = ALDOY;
	AlarmTime.month = ALMONTH;
	AlarmTime.year = ALYEAR;
	*/

	return AlarmTime;
}

// compares seconds, minutes and hours of both times
bool RTC_Compare(RTC_Time t1, RTC_Time t2)
{
	return (t1.seconds == t2.seconds
		&& t1.minutes == t2.minutes
		&& t1.hours == t2.hours);
}