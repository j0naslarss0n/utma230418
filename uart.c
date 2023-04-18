#include <stdio.h>
#include "uart.h"

/* 
En funktion som initierar USART och USART2 konfigurationen.
"USART1 and USART2 also provide hardware management of the CTS and RTS signals,
Smart Card mode (ISO 7816 compliant) and SPI-like communication capability. All
interfaces can be served by the DMA controller." (Guide s 29)


(s 547) (USART reg map s 557)
*/
void USART2_Init(void){
 /* RCC - Reset and Clock Control sätter APB1 peripheral Clock Enable Register till 100000000000000000 
 - minnesplats/bit 17 till 1(s 118-119) USART2EN: USART2 clock enable  */
 RCC->APB1ENR  |=  0x20000; 
/* RCC - sätter AHN1 bit 1 till enabled - GPIOAEN: IO port A clock enable
(s 120)*/
 RCC->AHB1ENR   |=0x01;
 
 /*GPIO reg map (s 163) o (s 157)
 General Purpose In/Out port A Mode register MODER0-MODER3??
 0x00F0 = 11110000
 Första 0-3 bits till 0 och 4-7 bits 1
& AND = Equals ~ NOT
MODER 0 o 1 sätts till NOT Input(reset state)  
MODER 2 o 3 sätts till NOT analog */
GPIOA->MODER &=~0x00F0;

/*
Port MODE Register
MODER 0 o 1 sätts till Input (reset state)
MPDER 2 o 3 sätts till Alternate function mode
0x00A0 = 10100000
*/
GPIOA->MODER |= 0x00A0; 
/*GPIO Alternate Function (Low) Register - Pin 0-3 // AFRL0-ARFL3
0xFF00 = 1111111100000000

(s 161)*/
GPIOA->AFR[0] &= ~0xFF00;
/*
0x7700 = 111011100000000
*/
GPIOA->AFR[0] |= 0x7700;

/*
USART Baud Rate Register 
0x0683 = 11010000011
These 4 bits define the fraction of the USART Divider (USARTDIV)
mantissa of USARTDIV [11:0]
(s 550)
*/
USART2->BRR  =  0x0683;

/*Control Register 1 
0x000C = 1100 - TE Transmitter enable & RE Reciver Enable
*/
USART2->CR1  =  0x000C;

/* Control register 2
Address till 0000
*/
USART2->CR2  =  0x000;

/* Control register 3
0000 
HDSEL Half Duplex selection =0 HD mode not selected
IRLP - IrDA Low-power 
IREN - IrDA mode enable = 0
EIE - Error Interupt Enable = 0 = Interrupt is inhibited
*/
USART2->CR3  =  0x000;
/* ConReg 1
10000000000000
UE: USART enable
*/
USART2->CR1  |=  0x2000; 
	
	

}

int USART2_write(int ch){
  
	while(!(USART2->SR & 0x0080)){} /* While NOT - Status Register Bit 8 TXE Transmit data registry empty 1: Data transfered to shift register(s 548) //1000 0000*/
	USART2->DR = (ch & 0xFF);       /* Data Register 8 bit data value  1111 1111 */ 
	
  return ch;
}

int USART2_read(void){
  while(!(USART2->SR & 0x0020)){} /* SR RXNE Read Data Reg Not Empty 1: Recieved data is ready to be read bit //10 0000 (s 548)*/
	return USART2->DR;              /* REturn data value from DR*/
}

struct __FILE { int handle; };
FILE __stdin  = {0};              /* En pekare till en array för in */
FILE __stdout = {1};              /* En pekare till en array för ut*/
FILE __stderr = {2};              /* En pekare till en array error*/


int fgetc(FILE *f) {            /* Function to get input from pointer array(s)*/
    int c;

    c = USART2_read();            /* Take input from func USART_read */

    if (c == '\r') {              /* If USART2_read is Enter */
        USART2_write(c);          /* Write Enter */
        c = '\n';                 /* USART2_read is new line*/
    }

    USART2_write(c);              /* Write again .. for new line*/

    return c;                     /* return USART2_read till fputc*/
}

int fputc(int c, FILE *f) {       /* funktion skriver ut USART2_read/c från pointern f */
    return USART2_write(c);
}


int n;
char str[80];                     /* String array */
/*
En funktion som är utkommenterad i main.c
Printar och läser kommentarer och svar. 
*/		
void test_setup(void){
	
	printf("please enter a number: ");
	scanf("%d", &n);
	printf("the number entered is: %d\r\n", n);
	printf("please type a character string: ");
	gets(str);
	printf("the character string entered is: ");
	puts(str);
	printf("\r\n");
	
}
