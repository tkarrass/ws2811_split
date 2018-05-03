
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
	// init:
	cli();
	sei();
	
	// block.
	// everything will be handled within the interrupt
	while(1);
	return 0; // mandatory for c, never reached in reality
}
