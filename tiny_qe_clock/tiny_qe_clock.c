/* Tiny QE Clock
 *
 * Copyright © 2012 Mike Tsao. Use, modification, and distribution are subject
 * to the BSD license as described in the accompanying LICENSE file.
 *
 * Generates a slow but working Q/E clock signal for the MC6809E CPU.  Designed
 * for an ATtiny13A running at 9.6MHz, which is able to put out a 285kHz signal
 * with Q on PB3 and E on PB4, nowhere near the 1MHz capability of the slowest
 * '09, but good enough to build a circuit with it. TODO: can PWM do a better
 * job?
 *
 * Part of the 8821 Project: http://www.sowbug.com/tagged/8821
 *
 * Using fuse calculator from http://www.engbedded.com/fusecalc/, determined
 * that the right avrdude magic for the internal 9.6MHz clock with SUT 14+64
 * was...
 *
 * -U lfuse:w:0x7a:m -U hfuse:w:0xff:m
 */


#include <avr/io.h>


// 285kHz and outputs a nice-looking, nearly symmetric signal.
static void slow_but_pretty() {
  register uint8_t portb = 0;
  register uint8_t state = 0;
  register uint8_t pb3_val = _BV(PB3);
  register uint8_t pb4_val = _BV(PB4);

  while (1) {
    if (state) {
      portb ^= pb4_val;
    } else {
      portb ^= pb3_val;
    }
    PORTB = portb;
    state = !state;
  }
}

// 800kHz! But the signal looks quite ugly.
static void fast_but_ugly() {
  register uint8_t portb = 0;
  register uint8_t state = 0;
  register uint8_t pb3_val = _BV(PB3);
  register uint8_t pb4_val = _BV(PB4);

  while (1) {
    if (state) {
      portb ^= pb4_val;
      PORTB = portb;
      state = !state;
    } else {
      portb ^= pb3_val;
      PORTB = portb;
      state = !state;
    }
  }
}

int main(void) {
  DDRB |= (1 << PB3) + (1 << PB4);
  slow_but_pretty();
  return 0;
}
