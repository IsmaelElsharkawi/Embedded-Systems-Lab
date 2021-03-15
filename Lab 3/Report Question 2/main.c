/* Receive characters from phone using UART1 and send it to PC using UART0 */
#include "TM4C123GH6PM.h"
#include <stdio.h>
char  buffer[8];
char n2[] = {'T','(' ,'M', 's', ')', ':'};
char nl[] = {'\r', '\n'};
int volatile i = 0;
int volatile counter = 0;
int volatile flag =0;
void UART0Tx(char c);
int main(void)
{
	__disable_irq();
	SYSCTL->RCGCUART |= 3; /* provide clock to UART0 and UART1*/
	SYSCTL->RCGCGPIO |= 3; /* enable clock to PORTA and PORTB */
	SYSCTL->RCGCGPIO |= 0x20; /* enable clock to PORTF */

	/* UART0 initialization */
	UART0->CTL = 0; /* disable UART0 */
	UART0->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART0->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART0->CC = 0; /* use system clock */
	UART0->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART0->IM |= 0x0010; /* enable RX interrupt */
	UART0->CTL = 0x301; /* enable UART0, TXE, RXE */
	
	/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */
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
			GPIOF->IBE |= 0x11; /* trigger is controlled by IEV */
			GPIOF->ICR |= 0x11; /* clear any prior interrupt */
			GPIOF->IM |= 0x11; /* unmask interrupt for PF4,PF0 */
			/* enable interrupt in NVIC and set priority to 3 */
			NVIC->IP[30] = 3 << 5; /* set interrupt priority to 3 */
			NVIC->ISER[0] |= 0x40000000; /* enable IRQ30 (D30 of ISER[0]) */
			//SysTick->CTRL = 7;
			//SysTick->LOAD = 16000;
	__enable_irq(); /* global enable IRQs */
	
 while(1){}
}

void UART0Tx(char c)
{
	int time_consumption =0;
	while((UART0->FR & 0x100) != 0){} /* wait until Rx buffer is not empty */
	UART0->DR= c;
	while(time_consumption<10000){ time_consumption++;}	
}

void GPIOF_Handler(void)
{
	int n;
	int i;

	/*if(flag ==0){
		flag =1;
		counter =0;
		SysTick->CTRL = 7;
		SysTick->LOAD = 16000;
	}else{
		flag =0;
		SysTick->CTRL=0;
		UART0Tx(counter);
		counter=0;
	}*/
if (GPIOF->MIS & 0x0001) /* if a receive interrupt has occurred */
	{
	 if((GPIOF->DATA & 0x1)==0){
		 //counter =0;
		 SysTick->LOAD = 16000-1;
		 SysTick->CTRL = 7;
		 
		 
	 }else{
		 if(counter>0){
			SysTick->CTRL=0;
				for (i =0; i< 6; i++) UART0Tx(n2[i]);
			  n = sprintf(buffer, "%d", counter);
			
				for(i = 0; i < n; i++){
					UART0Tx(buffer[i]);
				}
				UART0Tx(nl[0]);
				UART0Tx(nl[1]);

			counter=0;
			}
	 }
		GPIOF->RIS |= 0x0001; 
 }
}
void SysTick_Handler(void)
{
	counter+=1;
}
