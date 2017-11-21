#define F_CPU 19600000UL		//constant - frequency of MCU, used for calc of delay time in function _delay_ms
#define BAUD 9600
#define bauddivider (F_CPU/(16*BAUD)-1)
#include <avr/io.h>			//header with input-output registers names (like PORTB)
#include <avr/interrupt.h>	//header with iterrupt functions (ISR), which are avr-gcc specific
typedef unsigned char byte;	//unsigned char always matches one byte in C

ISR(USART_RX_vect){ // interrupt called on UART Rx Complete 
	PORTB=UDR0;		// send recived value to PORTB
}

int idle(){			//idle loop, always runs after init in main()
	while(1)
	{asm("nop");}
}

int main(){
	//uart setup
	UBRR0H	=(byte) bauddivider>>8;	//set baud rate
	UBRR0L	=(byte) bauddivider;
	UCSR0B	=1<<RXCIE0|1<<RXEN0|0<<UDRIE0;//reciever
	UCSR0C	=1<<UCSZ01|1<<UCSZ00;	//8bit mode
	sei();							//enable SREG interrupt flag
	idle();
}
