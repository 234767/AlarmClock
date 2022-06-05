#include "lpc2xxx.h"

static void sdelay (tU32 seconds)
{
    T0TCR = TIMER_RESET;                    //timer disable and reset
    T0PR  = 12000000-1;                     //prescaler
    T0MR0 = seconds;
    T0IR  = TIMER_ALL_INT;                  //interrupt flags
    T0MCR = MR0_S;                          //stop when reached MR0
    T0TCR = TIMER_RUN;                      //start timer

    while (T0TCR & TIMER_RUN)               // stop while timer is running
    {
    }
}