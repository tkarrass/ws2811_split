
#include <avr/io.h>
#include <avr/interrupt.h>


int main(void) {
	// init:
	cli();
	// port out
	DDRA = 0b11111111;
	asm volatile("ldi r19, 0b00000000");
	asm volatile("ldi r18, 0b11111110");
	PORTA = 0b11111110;

	// pin in
	// not required.
	
	// timer:
	TCCR1A= 0b00000000;
	TCCR1B= 0b00001110; // 14
	OCR1A=0x0707;
	TIMSK = 0b00010000; // 64;

	sei();
	
	// block.
	// everything will be handled within the interrupt
	while(1);
	return 0; // mandatory for c, never reached in reality
}

ISR(TIMER1_COMPA_vect) {
	asm volatile(
	  "lsl r18"	"\n\t"
		"adc r18, r19" "\n\t"
		"out %0, r18"	: : "I" (_SFR_IO_ADDR(PORTA))
  );
	//PORTA=0x0;
}
