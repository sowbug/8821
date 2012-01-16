/*
 * Memory Stuffer. Copyright © 2012 Mike Tsao. Use, modification, and
 * distribution are subject to the BSD license as described in the accompanying
 * LICENSE file.
 *
 * Loads up a run-of-the-mill SRAM with specific data, with the help of a few
 * 8-bit serial-to-parallel latches ('595, two for the 16-bit address bus, and
 * one for the 8-bit data bus). Mad props to Quinn Dunki
 * <http://quinndunki.com/blondihacks/?p=780> for the overall idea. Thanks,
 * also, to Ladyada's part finder <http://www.ladyada.net/wiki/partfinder/ic>,
 * which caused me to have plenty of '595s on hand.
 *
 * Part of the 8821 Project: http://www.sowbug.com/tagged/8821
 */


#include <avr/io.h>
#include <avr/pgmspace.h>
#include "image.h"


int main(void) {
  // '595 datasheet (and Quinn) recommend tying together SRCLK and RCLK. We do
  // so and toggle them both on this pin. But it means the storage register
  // will be one clock behind the shift register, so we need one more clock to
  // get things where they need to be.
  register uint8_t CLOCK = _BV(PB0);

  // The '595 has this great extra pin that lets you chain a bunch of them
  // together, so that you need only a single data pin to get data into all of
  // them. You latch repeatedly until all the bits get cascaded into place,
  // even across multiple chips.
  register uint8_t LATCH_IN = _BV(PB1);

  // Send this low to tell /OE that it's time to move what's been latched onto
  // all the Q (parallel) pins.
  register uint8_t STROBE = _BV(PB2);

  // Set a line to let everyone (including my logic analyzer) know the loop is
  // running.
  register uint8_t WORKING = _BV(PB3);

  // Everything's an output.
  DDRB = 0xff;

  // Reset states.
  PORTB |= STROBE;
  PORTB &= ~CLOCK;
  PORTB |= WORKING;

  uint16_t i;
  for (i = 0; i < IMAGE_LEN; ++i) {
    // Load the two address bytes, then the data byte into a single long that
    // we can then shift into the three '595s.
    uint32_t data = pgm_read_byte(&image[i]) |
      ((uint32_t)(IMAGE_START + i) << 8);

    // +1 because of the SRCLK/RCLK tie.
    uint8_t j;
    for (j = 0; j < (8 * 3) + 1; ++j) {
      // Load up the data pin.
      if (data & 1) {
	PORTB |= LATCH_IN;
      } else {
	PORTB &= ~LATCH_IN;
      }

      // Pulse the clock.
      PORTB |= CLOCK;
      PORTB &= ~CLOCK;

      // Move the next bit to the LSB.
      data >>= 1;
    }

    // Pulse the RAM strobe so it copies everything on the '595 Q lines.
    PORTB &= ~STROBE;
    PORTB |= STROBE;
  }

  PORTB &= ~WORKING;

  return 0;
}
