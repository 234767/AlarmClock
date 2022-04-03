#ifndef __UTIL_H
#define __UTIL_H

#define TRUE    1
#define FALSE   0
#ifndef NULL
#define NULL (0)
#endif

#define CRYSTAL_FREQUENCY 12000000                 /* External clock input frequency (must be between 10 MHz and 25 MHz) */

#define USE_PLL 1                     /* 0 = do not use on-chip PLL,
                                         1 = use on-chip PLL) */
#define PLL_MUL 5                     /* PLL multiplication factor (1 to 32) */
#define PLL_DIV 2                     /* PLL division factor (1, 2, 4, or 8) */
#define PBSD    1 

typedef unsigned char   uint8;
typedef char            int8;
typedef unsigned short  uint16;
typedef short           int16;
typedef unsigned int    uint32;
typedef int             int32;

static void
delay37us(void)
{
	volatile uint8 i;

	for(i=0; i<2500; i++)
        asm volatile (" nop"); //delay 15 ns x 2500 = about 37 us delay
}

static void
delay2ms(void)
{
	volatile uint8 i;

	for(i=0; i<55; i++)
        delay37us(); //delay 37 us x 55 = about 2 ms delay
}
#endif // __UTIL_H