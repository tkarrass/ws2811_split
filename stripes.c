
#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t port;

int main(void) {
	// init:
	cli();
	// port out
	 
	port = 1

	// pin in
	
	// timer:
	TCCR1A= 0;
	TCCR1B=14;
	OCR1A=0x0707;
	TIMSK=64;

	sei();
	
	// block.
	// everything will be handled within the interrupt
	while(1);
	return 0; // mandatory for c, never reached in reality
}

ISR(TIMER1_COMPA_vect) {
	
}
