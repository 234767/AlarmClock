#include "lpc2xxx.h"
#include "util.h"

#define ALARM_PIN 15

static void init_buzzer()
{
    PINSEL0 &= ~(3 << (2* ALARM_PIN)); // clear bits 15:14

    IODIR0 |= (1 << ALARM_PIN); // set P0.7 as output

    IOSET0 = (1 << ALARM_PIN); // set P0.7 high (buzzer off)
}

static void buzzer_on()
{
    IOCLR0 = (1 << ALARM_PIN); // bring P0.7 low
}

static void buzzer_off()
{
    IOSET0 = (1 << ALARM_PIN); // set P0.7 high
}
