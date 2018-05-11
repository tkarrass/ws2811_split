/*
 * stripe.c
 *
 * Splits a 800KHz ws2811 signal to distinct stripes attached to the PORTA pins.
 * Currently the split is hardcoded after 75 leds
 *
 */
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
	// avoid interrupts during initialization
	cli();

	// set port A to output
	DDRA = 0b11111111;
	PORTA = 0b11111110;
	// also prepare some registers which will be used
	// within the (time critical) interrupt handler
	asm volatile("ldi r19, 0b00000000");
	asm volatile("ldi r18, 0b11111110");

	// no input required. The signal feeds the Timer 
	// directly using the T1 pin.
	
	// set up the timer:
	TCCR1A= 0b00000000; // no output control, no pwm
	TCCR1B= 0b00001110; // clear counter on match, 
	                    // trigger on falling edge of T1 pin
	OCR1A = 0x0707;     // compare value: 75*24-1
	TIMSK = 0b00010000; // enable interrupt on match

	// enable interrupts
	sei();
	
	// block.
	// everything will be handled within the interrupt handler
	while(1);
	return 0; // mandatory for c, never reached in reality
}

// Interrupt handler for the timer compare interrupt.
// Needs to use asm directly, since we have only 9 cycles
// until the port needs to be set (using 16MHz frequency).
// From this 9 cycles 4 are already used for the jmp into
// function (unless we would move the opcodes into the 
// interrupt vector table).
ISR(TIMER1_COMPA_vect) {
  // do a left shift on r18 and add the carry flag back 
	// using an always-zero helping register. Then output.
	// 3 cycles in total.
	asm volatile(
	  "lsl r18"	"\n\t"
		"adc r18, r19" "\n\t"
		"out %0, r18"	: : "I" (_SFR_IO_ADDR(PORTA))
  );
}

