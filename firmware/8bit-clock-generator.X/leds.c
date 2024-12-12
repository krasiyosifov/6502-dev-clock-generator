/**
 * File:   leds.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#include <xc.h>
#include "leds.h"

// LEDs are connected on port bits 3-0
#define LEDS_MASK               0x0F

/**
 * Setter for LEDs state
 * @param state
 */
void leds_setState(leds_state_t state)
{
    // first MASK the PORT value,
    // then 'OR' the bits for the enabled LEDs
    // depending on the state structure
    PORTC = (PORTC & ~LEDS_MASK) | state;
}
