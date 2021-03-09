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
int volatile temp;
int volatile delay=0;
double volatile freq=0;
void delayMs(int n);
int main(void)
{
	// enable clock to GPIOF at clock gating control register
	SYSCTL->RCGCGPIO |= 0x20;
	// enable the GPIO pins for the LED (PF3, 2 1) as output
	GPIOF->DIR = 0x0e;
	// enable GPIO pins PF0 and PF4 as input 
	GPIOF->DIR&= ~0x11;
	// enable the GPIO pins for digital function
	//specified by the datasheet
	
	GPIOF->LOCK =0x4C4F434B;
	GPIOF->CR |= 0xff;
	GPIOF->PUR |= 0x11;
	//GPIOF->PUR = 0;
	GPIOF->DEN = 0x1f;
	
	
	SYSCTL->RCGCGPIO |= 0x04;
	GPIOC->DIR = 0xff;
	GPIOC->DEN = 0xff;
	
	
	//while((GPIOF->DATA & 0x10)==0){}
	freq = 1.5;
	delay = 500/freq;
	while(1)
	{	
		if((GPIOF->DATA & 0x10)==0){
			freq+=0.3;
			
			delay=500/freq;
			
			//delayMs(2000);
			//if((GPIOF->DATA & 0x10)==0)	flag =(flag +1)%3;
		}
		
		if((GPIOF->DATA & 0x01)==0){
				freq-=0.3;
				if(freq<0){ 
					delay = 100000000;
					freq=0;
				}
				else delay=500/freq;
			
			//delayMs(2000);
			//if((GPIOF->DATA & 0x01)==0)	flag_on_off =(flag_on_off +1)%2;
			
		}
			GPIOC->DATA = en? 0x128:0;
			//temp = 0x128&GPIOC->DATA;
			//GPIOF->DATA = temp; // turn on red LED
			en=!en;
			delayMs(delay);
			
		
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