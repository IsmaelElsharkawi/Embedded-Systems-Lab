#include <stdint.h>
#include "tm4c123gh6pm.h"
void UART0Tx(char const c);
void delayMs(int n);
int main(void)
{
	SYSCTL->RCGCUART |= 1; /* provide clock to UART0 */
	SYSCTL->RCGCGPIO |= 1; /* enable clock to PORTA */

	/* UART0 initialization */
	UART0->CTL = 0; /* disable UART0 */
	UART0->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART0->FBRD = 11; /* fraction part = 0.1666666*64+0.5 = 11.1666666 */
	UART0->CC = 0; /* use system clock */
	UART0->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART0->CTL = 0x301; /* enable UART0(bit0), TXE(bit8), RXE(bit9) */
	/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */

	delayMs(25); /* wait for output line to stabilize */

	for(;;)
	{
		UART0Tx('Y');
		UART0Tx('e');
		UART0Tx('s');
		UART0Tx(' ');
	}
}
/* UART0 Transmit */
/* This function waits until the transmit buffer is available then writes */
/* the character in it. It does not wait for transmission to complete */
void UART0Tx(char const c)
{
	while((UART0->FR & 0x20)!= 0){} // Wait until Tx buffer is not full
	UART0->DR = c; // Write byte
}

void delayMs(int delay)
{
	SysTick->LOAD = 16000*delay -1;
	SysTick->CTRL = 0x5; /*Enable the timer and choose sysclk */
	while((SysTick->CTRL & 0x10000) == 0) /*wait until the Count flag is set */
	{ }
	SysTick->CTRL = 0; /*Stop the timer (Enable = 0) */
}
