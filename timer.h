#include "lpc214x.h"
#include "util.h"

/**
 * @brief Waits for delay specified by \p delayLength * \p prescaler
 * 
 * @param delayLength
 * @param prescaler
 */
static void
delayMs(uint16 delayLength, uint32 prescaler) {
    //initialize and start Timer #0
  T0TCR = 0x00000002;                           //disable and reset Timer1
  T0PC  = prescaler;                            //set prescale
  T0MR0 = delayLength *                         //calculate no of timer ticks
         ((CRYSTAL_FREQUENCY * 5) / (1000 * 1));
  T0MCR = 0x00000006;                           //reset counter and stop timer on MR0 match
  T0TCR = 0x00000001;  
}

/**
 * @brief Assigns handler to timer1 interrupt
 * 
 * @param delayInDs delay for the timer prescaled by 100ms
 * @param handlerAddress address of the interrupt handler function
 */
static void
startTimer1(uint16 delayInDs, uint32 handlerAddress)
{
  //initialize VIC for Timer1 interrupts
  VICIntSelect &= ~0x20;           //Timer1 interrupt is assigned to IRQ (not FIQ)
  VICVectAddr5  = handlerAddress;  //register ISR address
  VICVectCntl5  = 0x25;            //enable vector interrupt for timer1 
  VICIntEnable  = 0x20;            //enable timer1 interrupt
  
  //initialize and start Timer #1
  T1TCR = 0x00000002;                           //disable and reset Timer1
  T1PC  = 100;                                  // prescale of 100
  T1MR0 = delayInDs *                           //calculate no of timer ticks
         ((CRYSTAL_FREQUENCY * 5) / (1000 * 1));
  T1IR  = 0x000000ff;                           //reset all flags before enable IRQs
  T1MCR = 0x00000003;                           //reset counter and generate IRQ on MR0 match
  T1TCR = 0x00000001;                           //start Timer1
}