/*
 * Wiegand-USB.c
 *
 * Created: 30.03.2019 15:45:02
 * Author : rando
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 8000000UL

#include <util/delay.h>
#include <string.h>
#define BAUD 9600

volatile unsigned long data = 0;
volatile int dataCount = 0;

void serial_send(char* ar) {
	for (int i = 0; i < strlen(ar); i++){
		while (( UCSR0A & (1<<UDRE0))  == 0){};
		_delay_ms(1);
		UDR0 = ar[i];
	}
}


ISR(PCINT2_vect) {
	if(!(PIND & (1<<PIND6))) {
		data = data << 1;
		dataCount++;
	} 
	if(!(PIND & (1<<PIND7))) {
		 data = data << 1;
		 data = data + 0x01;
		 dataCount++;
	}
	_delay_ms(1);
}

int main(void)
{
	CLKPR=128;
	CLKPR=0;
	DDRD |= (1 << PIND1);
	DDRD &= ~(1 << PIND0);

	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);   // Turn on the transmission and reception circuitry
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01); // Use 8-bit character sizes

	UBRR0H = 0;
	UBRR0L=54;
	
	DDRC |= (1 << DDC3);
	DDRC |= (1 << DDC0);
	DDRB &= (1 << DDB0);

	
	DDRD &= ~(1 << DDD6);
	DDRD &= ~(1 << DDD7);
	
	PCMSK2 |= (1<<PCINT22) | (1<<PCINT23);

	PCICR |= (1<<PCIE2);
	
	sei();
	
    while (1) 
    {
	
		if(PINB & (1<<PINB0)) { //26 bit
			PORTC &= ~(1<< PORTC3);
			if(dataCount == 26) {
				char buffer[8 * sizeof (long int) + 1 ];
				ltoa(data, buffer, 2);
				serial_send(buffer);
				data = 0;
				dataCount = 0;
			}
		} else {  // 34bit
			PORTC |= (1<< PORTC3);
			if(dataCount == 34) {
				char buffer[8 * sizeof (long int) + 1 ];
				ltoa(data, buffer, 2);
				serial_send(buffer);
				data = 0;
				dataCount = 0;
			}
		}
		

	}
}

