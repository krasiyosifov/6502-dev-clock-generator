/**
 * File:   hardware.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#include <xc.h>
#include "hardware.h"

inline void harware_init(void)
{
    // setup clock
    //          111             IRCF=b111   8MHz internal OSC
    //             xxx
    //                1         SCS=1       Internal oscillator is used for system clock
    OSCCON = 0b01110000;
    
    // disable analog hardware
    CMCON0  = 0x07;             // Comparators off. CxIN pins are configured as digital I/O
    ANSEL   = 0;                // digital I/O for all pins on PORTA and PORTC
    
    // setup PORT A functions
    PORTA   = 0;                // clear PORTA value
    //            1             RA5 in      Button UP
    //             1            RA4 in      Button DOWN
    //              1           RA3 in      HALT signal
    //               1          RA2 in      Button MODE
    //                1         RA1 in      ICSP
    //                 1        RA0 in      ICSP
    TRISA   = 0b00111111;       // set A0-A5 as inputs
    WPUA    = 0b00110111;       // enable pullups on A5,A4,A2
    IOCA    = 0b00000000;       // setup interrupt on change - OFF
    
    // setup PORT C functions
    PORTC   = 0;                // clear PORTC value
    //            0             RC5 out     CCP1 - generator out signal
    //              0           RC3 out     LED Auto (green)
    //               0          RC2 out     LED Auto (red)
    //                0         RC1 out     LED Manual (green)
    //                 0        RC0 out     LED Manual (red)
    TRISC   = 0b11010000;       // set C0-C3 as outputs for LEDs, C5 - out for CCP1
    
    // configure TIMER0 module, pull-ups, INT signal
    TMR0 = 0;                   // reset counter
    //             0            nRAPU=0     enable pull ups on port A
    //              0           INTEDG=0    interrupt on falling INT edge
    //               0          T0CS=0      TMR0 source = Fosc/4
    //                 0        PSA=0       prescaler used by TMR0
    //                  111     PS=111      prescaler 1:128
    OPTION_REG = 0b00000111;
    
    // setup peripheral interrupts
    //        0                 CCP1IE=0    enable CCP1 interrupt
    //              0           TMR1IE=0    enable timer 1 interrupt
    PIE1 = 0b00000000;
    
    // setup interrupt on change
    //           0              IOCA3=0     disable IOC on RA3    
    IOCA = 0b00000000;
    
    // enable interrupts
    //          1               PEIE=1      enable peripheral int
    //           1              T0IE=1      enable TMR0 int
    //            1             INTE        enable INT int
    INTCON = 0b01110000;
}