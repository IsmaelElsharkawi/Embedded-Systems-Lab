/* Read data from UART0 and display it at the tri-color LEDs. The LEDs are
connected to Port F 3-1. Press any A-z, a-z, 0-9 key at the terminal emulator
and see ASCII value in binary is displayed on LEDs of PORTF. */
#include "tm4c123gh6pm.h"
int main(void)
{
 SYSCTL->RCGCUART |= 1; /* provide clock to UART0 */
 SYSCTL->RCGCGPIO |= 1; /* enable clock to PORTA */
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

 GPIOF->DIR = 0x0E; /* configure Port F to control the LEDs */
 GPIOF->DEN = 0x0E;
 GPIOF->DATA = 0; /* turn LEDs off */
 /* enable interrupt in NVIC and set priority to 3 */
 NVIC->IP[5] = 3 << 5; /* set interrupt no 5 priority to 3 */
 NVIC->ISER[0] |= 0x00000020; /* enable IRQ5 for UART0 */
 __enable_irq(); /* global enable IRQs */
 for(;;){}
}
void UART0_Handler(void)
{
 volatile int readback;
 char c;
 if (UART0->MIS & 0x0010) /* if a receive interrupt has occurred */
 {
 c = UART0->DR; /* read the received data */
 GPIOF->DATA = c << 1; /* shift left and write it to LEDs */
 UART0->ICR = 0x0010; /* clear Rx interrupt flag */
 readback = UART0->ICR; /* a read to force clearing of interrupt flag */
 }
 else
 {
 /* should not get here. But if it does, */
 UART0->ICR = UART0->MIS; /* clear all interrupt flags */
 readback = UART0->ICR; /* a read to force clearing of interrupt flag */
 }
}
// Remember to disable IRQs in SystemInit function in generated startup C src.
