#define F_CPU 19600000UL		//constant - frequency of MCU, used for calc of delay time in function _delay_ms
#define BAUD 1000000
#define bauddivider (F_CPU/(16*BAUD)-1)
#include <avr/io.h>			//header with input-output registers names (like PORTB)
#include <avr/interrupt.h>	//header with iterrupt functions (ISR), which are avr-gcc specific
#define false 0
#define true 1
#define UART_TEST 0
typedef unsigned char byte;

byte is_adc_ready	=false;
byte ADC_result_HI	=0;
byte ADC_result_LO	=0;
byte is_ADC_HI_sent =0;

ISR(USART_UDRE_vect){ // interrupt called on UART Tx Complete 
	if (is_adc_ready) {//if adc interrupt has been called before, send Hi byte and then low byte of result
		if (!is_ADC_HI_sent){
#if UART_TEST
			UDR0=0b10000001;
#else
			UDR0=ADC_result_HI;
#endif
			is_ADC_HI_sent=true;
		}
		else{
#if UART_TEST
			UDR0=0b10000001;
#else
			UDR0=ADC_result_LO;
#endif

			is_adc_ready=false;
			is_ADC_HI_sent=false;
		}
	}
	else{
 		UCSR0B	&=~(1<<UDRIE0);
	}//disable interrupt, otherwise program will never reach adc interrupt
}
ISR(ADC_vect){	//_VECTOR(21)  /* ADC Conversion Complete */
	UCSR0B|=1<<UDRIE0;
	ADC_result_LO=ADCL;
	ADC_result_HI=ADCH;
	is_adc_ready=true;
}

int idle(){
	while(1)
	{asm("nop");}
}

int main(){
	//usart setup
	UBRR0H	=(byte) bauddivider>>8;//set baud rate
	UBRR0L	=(byte) bauddivider;
	UCSR0B	=1<<TXEN0|1<<UDRIE0;//transmit enable, interrupt enable
	UCSR0C	=1<<UCSZ01|1<<UCSZ00;//8bit mode
	//adc setup
	ADCSRA	=1<<ADATE|1<<ADEN|1<<ADSC|1<<ADIE|1<<ADPS2|0<<ADPS1|0<<ADPS0;
	ADMUX	=1<<REFS1|1<<REFS0;//set internal analog reference voltage Vin=1.1
	//ADMUX	=0<<REFS1|1<<REFS0;//set analog reference voltage AVCC with external capacitor at AREF
	sei();
	idle();
}
