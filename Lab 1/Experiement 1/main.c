/* This program blinks the red LED on the
* TI Tiva LaunchPad. The connections are:
* PF1 - red LED
* PF2 - blue LED
* PF3 - green LED
* They are high active (a '1' turns on the LED).
*/
#include "TM4C123GH6PM.h"
int flag=0;
int flag_on_off =0;
int counter_on_off =0;
int en=0;
void delayMs(int n);
int main(void)
{
	// enable clock to GPIOF at clock gating control register
	SYSCTL->RCGCGPIO |= 0x20;
	// enable the GPIO pins for the LED (PF3, 2 1) as output
	GPIOF->DIR = 0x0e;
	
	GPIOF->DEN = 0x1f;
	

	while(1)
	{	
		
			GPIOF->DATA = 0x02; // turn on red LED
			
			delayMs(500);
			
			GPIOF->DATA = 0; // turn off red LED
			delayMs(500);
		
	}
}
// delay in milliseconds (16 MHz CPU clock)
void delayMs(int n)
{
	SysTick->LOAD = 16000*n-1;
	SysTick->CTRL = 0x5; /*Enable the timer and choose sysclk */
	while((SysTick->CTRL & 0x10000) == 0) /*wait until the Count flag is set */
	{ }
	SysTick->CTRL = 0; /*Stop the timer (Enable = 0) */
}