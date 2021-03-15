/* Receive characters from phone using UART1 and send it to PC using UART0 */
#include "TM4C123GH6PM.h"
char volatile buffer[1024];
int volatile i = 0;
int volatile flag =0;
void UART0Tx(char c);
int main(void)
{
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
	/* UART1 initialization */
	UART1->CTL = 0; /* disable UART1 */
	UART1->IBRD = 104; /* 16MHz/(16*9600 baud rate) = 104.1666666666 */
	UART1->FBRD = 11; /* fraction part= 0.1666666*64+0.5 = 11.1666666 */
	UART1->CC = 0; /* use system clock */
	UART1->LCRH = 0x60; /* 8-bit, no parity, 1-stop bit, no FIFO */
	UART1->IM |= 0x0010; /* enable RX interrupt */
	UART1->CTL = 0x301; /* enable UART1, TXE, RXE */
	/* UART0 TX0 and RX0 use PA1 and PA0. Set them up. */
	GPIOA->DEN = 0x03; /* Make PA0 and PA1 as digital */
	GPIOA->AFSEL = 0x03; /* Use PA0,PA1 alternate function */
	GPIOA->PCTL = 0x11; /* configure PA0 and PA1 for UART */

	GPIOF->DIR = 0x0E; /* configure Port F to control the LEDs */
	GPIOF->DEN = 0x0E;
	GPIOF->DATA = 0; /* turn LEDs off */
	/* UART1 TX0 and RX0 use PB1 and PB0. Set them up. */
	GPIOB->DEN = 0x03; /* Make PB0 and PB1 as digital */
	GPIOB->AFSEL = 0x03; /* Use PB0,PB1 alternate function */
	GPIOB->PCTL = 0x11; /* configure PB0 and PB1 for UART */

	/* enable interrupt in NVIC and set priority to 3 */
	NVIC->IP[6] = 3 << 5; /* set interrupt no 6 priority to 3 */
	NVIC->ISER[0] |= 0x00000040; /* enable IRQ6 for UART1 */
	__enable_irq(); /* global enable IRQs */

 while (1) {
	while(flag==0){}
	__disable_irq();
	 if(buffer[0] =='A' && buffer[1] == 'T' && buffer[2] == '+'){
			GPIOF->DATA=0;
			if(buffer[3]=='R'){
				if(buffer[5]=='0') GPIOF->DATA  &= ~2;
				else GPIOF->DATA |= 2;
			}
			if(buffer[3]=='G'){
				if(buffer[5]=='0') GPIOF->DATA  &= ~8;
				else GPIOF->DATA |= 8;
			}
			if(buffer[3]=='B'){
				if(buffer[5]=='0') GPIOF->DATA  &= ~4;
				else GPIOF->DATA |= 4;
			}
			if(buffer[3]=='W'){
				if(buffer[5]=='0') GPIOF->DATA  &= ~14;
				else GPIOF->DATA |= 14;
			}
			if(buffer[3]=='O' && buffer[4] == 'F' && buffer[5] == 'F'){
				GPIOF->DATA =0;
			}
			
	 }
	i=0;
	flag =0;
	__enable_irq();

 
 }
}
void UART1_Handler(void)
{
 volatile int readback;
 char c;

 if (UART1->MIS & 0x0010) /* if a receive interrupt has occurred */
 {
	 
	 c = UART1->DR; /* read the received data */
	 if(c=='\n' || i>=6){
			i=0;
			flag =1;
	 }else{
		 buffer[i] = c;
		 i++;
	 }
	 UART1->ICR = 0x0010; /* clear Rx interrupt flag */
	 readback = UART1->ICR; /* a read to force clearing of interrupt flag */
	 UART0Tx(c);
 }
 else
 {
	 /* should not get here. But if it does, */
	 UART1->ICR = UART0->MIS; /* clear all interrupt flags */
	 readback = UART1->ICR; /* a read to force clearing of interrupt flag */
	 }
}
void UART0Tx(char c)
{
 UART0->DR = c; /* read the received data */
}