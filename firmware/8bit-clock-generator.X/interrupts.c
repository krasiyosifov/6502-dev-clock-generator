/**
 * File:   interrupts.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 05.12.2024
 */

#include <xc.h>
#include "interrupts.h"
#include "generator.h"


volatile interrupt_flags_t interrupt_flags = {
    .readButtons = 0,
    .stopGenerator = 0,
    .timer1Overflow = 0
};

volatile uint16_t timer1StartValue = 0;


/**
 * Interrupt routine
 */
void __interrupt() ISR(void)    
{
    // external INT signal - HALT the generator
    if (INTF) {
        RC5 = 0;            // set output low
        T2CON = 0;          // stop timer 2
        T1CON = 0;          // stop timer 1
        CCP1CON = 0;        // stop CCP
        INTF = 0;
        interrupt_flags.stopGenerator = 1;        // set flag to stop generator
    }
    
    // timer 1 interrupt
    if (TMR1IF) {
        TMR1IF = 0;
        // call generator timer1 callback
        interrupt_flags.timer1Overflow = 1;
    }
    
    // timer 0 interrupt
    if (TMR0IF) {
        TMR0IF = 0;
        interrupt_flags.readButtons = 1;          // set flag to read buttons
    }
}
