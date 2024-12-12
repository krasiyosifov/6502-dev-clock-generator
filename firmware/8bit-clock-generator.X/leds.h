/* 
 * File:   leds.h
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#ifndef LEDS_H
#define	LEDS_H

#ifdef	__cplusplus
extern "C" {
#endif

/**
 * Enum for all possible valid states of the LEDs
 * The values are defined in such a way, that
 * matches the needed PORT value to control the LEDs.
 */    
                                //  A-G A-R M-G M-R
typedef enum {                  //  RC3 RC2 RC1 RC0 
    LEDS_OFF = 0,               //  0   0   0   0
    LEDS_MANUAL_RED = 0x01,     //  0   0   0   1
    LEDS_MANUAL_GREEN = 0x02,   //  0   0   1   0
    LEDS_AUTO_RED = 0x04,       //  0   1   0   0
    LEDS_AUTO_GREEN = 0x08,     //  1   0   0   0
    LEDS_AUTO_YELLOW = 0x0C,    //  1   1   0   0
} leds_state_t;


/**
 * Setter for LEDs state
 * @param state
 */
void leds_setState(leds_state_t state);


#ifdef	__cplusplus
}
#endif

#endif	/* LEDS_H */

