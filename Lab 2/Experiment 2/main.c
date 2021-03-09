#include "TM4C123GH6PM.h"
int main (void)
{
 /* enable clock to GPIOF at clock gating control register */
 SYSCTL->RCGCGPIO |= 0x20;
 /* enable the GPIO pins for the LED (PF3, 2, 1) as output */
 GPIOF->DIR = 0x0e;
 /* enable the GPIO pins for digital function */
 GPIOF->DEN = 0x0e;

 /* Configure SysTick */
 SysTick->LOAD = 16000000-1; /* reload with number of clocks per second */
 SysTick->CTRL = 7; /* enable SysTick interrupt, use system clock */
 __enable_irq(); /* global enable interrupt */

 while (1)
 {
 }
}
void SysTick_Handler(void)
{
 GPIOF->DATA ^= 2; /* toggle the red LED */
}