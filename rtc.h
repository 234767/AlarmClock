#include <lpc214x.h>
#include <stdint.h>
#include <stdio.h>
#include "util.h"

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
}

void RTC_Set_AlarmTime(RTC_Time AlarmTime)
{
	ALSEC = AlarmTime.seconds;
	ALMIN = AlarmTime.minutes;
	ALHOUR = AlarmTime.hours;
}

RTC_Time RTC_GetTime(void)
{
	RTC_Time time;

	time.seconds = SEC;
	time.minutes = MIN;
	time.hours = HOUR;

	return time;
}

RTC_Time RTC_GetAlarmTime(void)
{
	RTC_Time AlarmTime;

	AlarmTime.seconds = ALSEC;
	AlarmTime.minutes = ALMIN;
	AlarmTime.hours = ALHOUR;

	return AlarmTime;
}

// compares seconds, minutes and hours of both times
bool RTC_Compare(RTC_Time t1, RTC_Time t2)
{
	return (t1.seconds == t2.seconds
		&& t1.minutes == t2.minutes
		&& t1.hours == t2.hours);
}