#include "TM4C123GH6PM.h"
void delayMs(int n);
unsigned int LED = 0;
unsigned int masks[] = {0x08, 0x06};
int main(void)
{
 SYSCTL->RCGCGPIO |= 0x20; /* enable clock to PORTF */
 /* PORTF0 has special function, need to unlock to modify */
 GPIOF->LOCK = 0x4C4F434B; /* unlock commit register */
 GPIOF->CR = 0x01; /* make PORTF0 configurable */
 GPIOF->LOCK = 0; /* lock commit register */
 /* configure PORTF for switch input and LED output */
 GPIOF->DIR &= ~0x11; /* make PORTF4,0 input for switch */
 GPIOF->DIR |= 0x0E; /* make PORTF3, 2, 1 output for LEDs */
 GPIOF->DEN |= 0x1F; /* make PORTF4-0 digital pins */
 GPIOF->PUR |= 0x11; /* enable pull up for PORTF4,0 */

 /* configure PORTF4, 0 for falling edge trigger interrupt */
 GPIOF->IS &= ~0x11; /* make bit 4, 0 edge sensitive */
 GPIOF->IBE &= ~0x11; /* trigger is controlled by IEV */
 GPIOF->IEV &= ~0x11; /* falling edge trigger */
 GPIOF->ICR |= 0x11; /* clear any prior interrupt */
 GPIOF->IM |= 0x11; /* unmask interrupt for PF4,PF0 */
/* enable interrupt in NVIC and set priority to 3 */
 NVIC->IP[30] = 3 << 5; /* set interrupt priority to 3 */
 NVIC->ISER[0] |= 0x40000000; /* enable IRQ30 (D30 of ISER[0]) */
 __enable_irq(); /* global enable IRQs */

 /* toggle the green LED (PF3) continuously */
 while(1)
 {
 GPIOF->DATA |= masks[LED];
 delayMs(500);
 GPIOF->DATA &= ~ 0x0e; /* turn all LEDs off */
 delayMs(500);
 }
}
/* SW1 is connected to PF4 pin, SW2 is connected to PF0. */
/* Both of them trigger PORTF interrupt */
void GPIOF_Handler(void)
{
 volatile int readback;
 LED = ! LED;
 GPIOF->ICR |= 0x11; /* clear the interrupt flag before return */
 readback = GPIOF->ICR; /* a read to force clearing of interrupt flag */
}
/* delay n milliseconds (16 MHz CPU clock) */
void delayMs(int n)
{
SysTick->LOAD = 15999*n;
SysTick->CTRL = 0x5; /*Enable the timer and choose sysclk */
while((SysTick->CTRL & 0x10000) == 0) /*wait till Count flag is set */
{ }
SysTick->CTRL = 0; /*Stop the timer (Enable = 0) */
}

	