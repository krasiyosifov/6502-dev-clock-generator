/**
 * File:   main.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 * 
 * ****************************
 * Development Clock Generator
 * ****************************
 * 
 * This project implements a clock generator with two operational modes:
 * Manual Mode and Auto Mode, controlled via the "mode" button.
 * 
 * Behavior:
 * - Manual Mode:
 *   - "+/high" button: Sets the output clock HIGH.
 *   - "-/low" button: Sets the output clock LOW.
 *   - If the output clock is already in the desired state, clicking the respective button toggles 
 *     the clock state momentarily before returning to the desired level.
 * 
 * - Auto Mode:
 *   - "+/high" button: Increases the output frequency to the next higher standard frequency.
 *   - "-/low" button: Decreases the output frequency to the next lower standard frequency.
 *   - Standard frequencies: 
 *          1Hz, 4Hz, 20Hz, 100Hz, 500Hz, 
 *          2KHz, 10KHz, 50KHz, 200KHz, 
 *          1MHz, 2MHz.
 * 
 * Additional Features:
 * - Halt Signal: An active LOW input halts the generator and sets the clock output to manual LOW state.
 * 
 * Microcontroller IC: PIC16F684 (14 pin PDIP 8-bit microcontroller)
 * Documentation: https://ww1.microchip.com/downloads/en/DeviceDoc/41202F-print.pdf
 * 
 * Developed using:
 *      MPLAB X IDE: v6.20
 *      Compiler: XC8 v2.36
 * 
 * Hardware connections
 * PIN 1    VDD - 5V
 * PIN 2    RA5 - Button up             (internal pullup)
 * PIN 3    RA4 - Button down           (internal pullup)
 * PIN 4    RA3 - Button MODE           (external pullup)
 * PIN 5    RC5 - CLK out
 * PIN 6    RC4 - 
 * PIN 7    RC3 - LED Auto (green)
 * PIN 8    RC2 - LED Auto (red)
 * PIN 9    RC1 - LED Manual (green)
 * PIN 10   RC0 - LED Manual (red)
 * PIN 11   RA2 - nHALT input signal    (internal pullup)
 * PIN 12   RA1 - ICSP 
 * PIN 13   RA0 - ICSP
 * PIN 14   VSS - GND
 * 
 */


// PIC16F684 CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select bit (MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON       // Brown Out Detect (BOR enabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)


#include <xc.h>
#include "buttons.h"
#include "hardware.h"
#include "interrupts.h"
#include "leds.h"
#include "generator.h"


/**
 * Main function
 */
void main(void) 
{
    buttons_state_t * buttonsState;
    
    harware_init();             // initialize hardware
    generator_init();           // initialize generator
    ei();                       // enable all interrupts
    
    // loop forever
    while (1) {
        // update timer 1 overflow event
        if (interrupt_flags.timer1Overflow) {
            if (interrupt_flags.stopGenerator == 0) {
                // check that the stop generator signal is not active
                generator_timer1Callback();
            }
            interrupt_flags.timer1Overflow = 0;
        }
        
        // update application state
        if (interrupt_flags.stopGenerator) {
            interrupt_flags.stopGenerator = 0;
            generator_stop();
        }
        
        // read buttons
        if (interrupt_flags.readButtons) {
            interrupt_flags.readButtons = 0;

            // update individual buttons states
            buttons_updateButtonState(BUTTON_UP, BUTTON_UP_PIN);
            buttons_updateButtonState(BUTTON_DOWN, BUTTON_DOWN_PIN);
            buttons_updateButtonState(BUTTON_MODE, BUTTON_MODE_PIN);
            
            // read buttons calculated state
            buttonsState = buttons_getState();
            
            // handle button UP
            if (buttonsState->buttonUp.pressed) {
                buttonsState->buttonUp.pressed = 0;
                if (generator_getMode() == GEN_MODE_MANUAL) {
                    generator_setManualState(GEN_STATE_MANUAL_HIGH);
                } else {
                    generator_increaseFrequency();
                }
            }
            
            // handle button DOWN
            if (buttonsState->buttonDown.pressed) {
                buttonsState->buttonDown.pressed = 0;
                if (generator_getMode() == GEN_MODE_MANUAL) {
                    generator_setManualState(GEN_STATE_MANUAL_LOW);
                } else {
                    generator_decreaseFrequency();
                }
            }

            // handle button MODE
            if (buttonsState->buttonMode.pressed) {
                buttonsState->buttonMode.pressed = 0;
                generator_toggleMode();
            }            
        }
        
    }
}

