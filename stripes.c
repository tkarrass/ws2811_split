/*
 * stripe.c
 *
 * Splits a 800KHz ws2811 signal to distinct stripes attached to the PORTA pins.
 * Currently the split is hardcoded to happen after 75 leds, cycling through all
 * 8 pins of the port.
 *
 * This source assumes an Atmega32 to be used. Every similar mcu may be used 
 * after minor changes to the source (and Makefile, of course) as long as it 
 * provides an output port with 8 pins. Due to timing issues it will not be 
 * possible (or way trickier) to rotate and write the bits on two ports within
 * the required timeframe.
 */
#include <avr/io.h>
#include <avr/interrupt.h>

int main(void) {
	cli(); // Avoid interrupts during initialization

	// Initialize port A
	DDRA  =0b11111111;
	PORTA =0b11111110;

	// Also prepare some registers which will be used within the (time critical) interrupt handler
	asm volatile("ldi r19, 0b00000000");  // A dummy register for adc (see interrupt)
	asm volatile("ldi r18, 0b11111110");  // The initial port A value to avoid the need for reading it later.

	// No input pins required. The signal feeds the Timer directly using the T1 pin.	
	// Set up the timer:
	TCCR1A=0b00000000; // No output control, no pwm
	TCCR1B=0b00001110; // Clear counter on match, 
	                   // Trigger on falling edge of T1 pin
	OCR1A =0x0707;     // Compare value: 75*24-1 (no need to make this ATOMOIC, since interrupts are disabled)
	TIMSK =0b00010000; // Enable interrupt on match

	// That's it. Party time:
	sei();    // Enable interrupts globally
	while(1); // Block (maybe set to sleep?)
	return 0; // Mandatory for c, never reached in reality.
}

// Interrupt handler for the timer compare interrupt.
// Needs to use asm directly, since we have only 9 cycles until the port needs to be set (using 16MHz frequency).
// From this 9 cycles 4 are already used for the jmp into function (unless we would move the opcodes into the 
// interrupt vector table which would save another 2 cycles).
                                                 // maybe finish curent cmd      - 0-1 cycles
						 //                                    (could also be up to 3, but
						 //                                     not within our block loop,
						 //                                     see main)
ISR(TIMER1_COMPA_vect) {                         // interrupt call + rjmp (auto) -   4 cycles
  asm volatile(
    "lsl r18"      "\n\t"                        // left shift the help register -   1 cycle
    "adc r18, r19" "\n\t"                        // add the carry flag           -   1 cycle
    "out %0, r18" : : "I" (_SFR_IO_ADDR(PORTA))  // set the output               -   1 cycle
  );                                             //                           Total: 8 cycles max. Works!
}

