#include "lpc214x.h"
#include "util.h"

static void init_buzzer()
{
    PINSEL0 &= ~(3 << 14); // clear bits 15:14

    IO0DIR |= (1 << 7); // set P0.7 as output

    IO0SET = (1 << 7); // set P0.7 high (buzzer off)
}

static void buzzer_on()
{
    IO0CLR = (1 << 7); // bring P0.7 low
}

static void buzzer_off()
{
    IO0SET = (1 << 7); // set P0.7 high
}