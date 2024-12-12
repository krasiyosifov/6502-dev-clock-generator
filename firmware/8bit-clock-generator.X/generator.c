/**
 * File:   generator.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */
#include <xc.h>
#include "types.h"
#include "generator.h"
#include "leds.h"


generator_state_t generatorState = GEN_STATE_MANUAL_LOW;
generator_mode_t generatorMode = GEN_MODE_MANUAL;

volatile uint8_t timer1Counter = 0;

typedef enum {
    FREQ_500mHz,
    FREQ_2Hz,
    FREQ_10Hz,
    FREQ_50Hz,
    FREQ_200Hz,
    FREQ_1KHz,
    FREQ_5KHz,
    FREQ_20KHz,
    FREQ_100KHz,
    FREQ_500KHz,
    FREQ_1MHz,
    FREQ_2MHz
} frequency_value_t;

enum timer1_prescaler {
    TMR1_PRESCALE_1 = 0,
    TMR1_PRESCALE_2 = 1,
    TMR1_PRESCALE_4 = 2,
    TMR1_PRESCALE_8 = 3
};

enum timer2_prescaler {
    TMR2_PRESCALE_1 = 0,
    TMR2_PRESCALE_4 = 1,
    TMR2_PRESCALE_16 = 2
};

uint16_t timer1ReloadValue = 0;
frequency_value_t generatorFrequency;

static void _updateHardwareSetupForGeneration(void);
static void _slowGeneratorToggle(void);

/**
 * 
 */
void _updateHardwareSetupForGeneration(void)
{
    // stop all timers and CCP if manual mode
    if (generatorMode == GEN_MODE_MANUAL) {        
        T1CON = 0;      // stop timer 1
        T2CON = 0;      // stop timer 2
        CCP1CON = 0;    // stop CCP

        return;
    }
    
    // generator is in AUTO mode - continue
    uint8_t prescaller = 0;     // 1:1
    
    if (generatorFrequency <= FREQ_200Hz) {
        // SLOW generation mode - use TMR1 -> interrupt logic -> RC5

        // stop CCP
        CCP1CON = 0;

        prescaller = TMR1_PRESCALE_1;     // 1:1
        switch (generatorFrequency)
        {
            case FREQ_500mHz:
                prescaller = TMR1_PRESCALE_8;     // 1:8
                timer1ReloadValue = (0xFFFF - 62500);
                break;
            case FREQ_2Hz:
                prescaller = TMR1_PRESCALE_8;     // 1:8
                timer1ReloadValue = (0xFFFF - 62500);
                break;
            case FREQ_10Hz:
                prescaller = TMR1_PRESCALE_4;     // 1:4
                timer1ReloadValue = (0xFFFF - 25000);
                break;
            case FREQ_50Hz:
                timer1ReloadValue = (0xFFFF - 20000);
                break;
            case FREQ_200Hz:
                timer1ReloadValue = (0xFFFF - 5000);
                break;
            default:
                return;     // not supported
                break;
        }
        
        // configure TIMER1 module
        TMR1    = timer1ReloadValue;            // reset counter
        //            xx            T1CKPS=3    1:1-1:8 prescaler
        //               0          nT1SYNC=0   synchronize
        //                0         TMR1CS=0    source Internal clock (FOSC/4)
        //                 1        TMR1ON=1    disable Timer 1
        T1CON   = 0b00000001 | (uint8_t)(prescaller << 4);
        
        // enable TMR1 interrupt
        TMR1IE = 1;
        
        return;
    }
    
    // FAST generation mode - use TMR2 -> CCP1 -> RC5

    // disable TMR1 interrupt
    TMR1IE = 0;
    
    uint8_t timer2Period = 0xFF;
    uint8_t pulseWith   = 0x7F;
    prescaller = TMR2_PRESCALE_1;     // 1:1
    switch (generatorFrequency)
    {
        case FREQ_1KHz:
            prescaller = TMR2_PRESCALE_16;
            timer2Period = 125;
            break;
        case FREQ_5KHz:
            prescaller = TMR2_PRESCALE_4;
            timer2Period = 100;
            break;
        case FREQ_20KHz:
            timer2Period = 100;
            break;
        case FREQ_100KHz:
            timer2Period = 20;
            break;
        case FREQ_500KHz:
            timer2Period = 4;
            break;
        case FREQ_1MHz:
            timer2Period = 2;
            break;
        case FREQ_2MHz:
            timer2Period = 1;
            break;
       default:
            return;     // not supported
            break;
    }
    
    pulseWith = timer2Period >> 1;

    uint8_t pwmPrecise = 0;
    if (pulseWith == 0) {
        pwmPrecise = 2;
    }
    
    // start CCP1
    //          00              P1M=b0       Single output; P1A modulated; P1B, P1C, P1D assigned as port pins
    //            00            DC1B=b00     These bits are the two LSbs of the PWM duty cycle. The eight MSbs are found in CCPR1L.
    //              1100        CCP1M=b1100  PWM mode; P1A, P1C active-high; P1B, P1D active-high
    CCP1CON = 0b00001100 | (uint8_t)(pwmPrecise << 4);
    
    // configure TIMER2 module (used by CCP module for freq. gen.)
    //          x
    //           1111           TOUTPS      postscaler 1:16
    //               1          TMR2ON      TIMER2 ON
    //                00        T2CKPS      prescaler 1:1
    T2CON   = 0b01111100 | prescaller;
    PR2     = (timer2Period - 1);
    
    // set 50% as duty cycle
    CCPR1L = pulseWith;
    
    // setup auto-shutdown
    //         0                ECCPASE=0   ECCP outputs are operating
    //          100             ECCPAS=100  Auto-shutdown Source - VIL on INT pin
    //             00           PSSAC=00    Drive pins P1A and P1C to '0'
    //               00         PSSBD=00    Drive pins P1B and P1D to '0'
    ECCPAS = 0b01000000;

    // set yellow light
    leds_setState(LEDS_AUTO_YELLOW);
}

/**
 * 
 */
void _slowGeneratorToggle(void)
{
    static leds_state_t ledState = LEDS_OFF;
    if (generatorState == GEN_STATE_SLOW_AUTO_LOW) {
        // set it to HIGH
        generatorState = GEN_STATE_SLOW_AUTO_HIGH;
        RC5 = 1;
        ledState = LEDS_AUTO_GREEN;
    } else {
        // set it to LOW
        generatorState = GEN_STATE_SLOW_AUTO_LOW;
        RC5 = 0;
        ledState = LEDS_AUTO_RED;
    }
    if (generatorFrequency > FREQ_10Hz) {
        ledState = LEDS_AUTO_YELLOW;
    }
    // update LEDs state
    leds_setState(ledState);
}


/**
 * 
 */
inline void generator_timer1Callback(void)
{
    if (timer1ReloadValue != 0) {
        TMR1 += timer1ReloadValue;
    }
    
    if (generatorFrequency == FREQ_500mHz) {
        if (++timer1Counter >= 4) {
            timer1Counter = 0;
            _slowGeneratorToggle();
        }
    } else if (generatorFrequency <= FREQ_200Hz) {
        _slowGeneratorToggle();
    }
}


/**
 * 
 */
inline void generator_init(void)
{
    // set default frequency
    generatorFrequency = FREQ_2Hz;
    // set state auto
    generator_setAutoMode();
}

/**
 * 
 */
void generator_toggleMode(void)
{
    if (generatorMode == GEN_MODE_AUTO) {
        generator_setManualState(GEN_STATE_MANUAL_LOW);
    } else {
        generator_setAutoMode();
    }
}


void generator_stopFast(void)
{
    RC5 = 0;            // set output low
    T2CON = 0;          // stop timer 0
    T1CON = 0;          // stop timer 1
    CCP1CON = 0;        // stop CCP
}

/**
 * 
 */
void generator_stop(void)
{
    generator_stopFast();
    leds_setState(LEDS_MANUAL_RED);
    generatorMode = GEN_MODE_MANUAL;
    generatorState = GEN_STATE_MANUAL_LOW;
}


/**
 * 
 * @return generator_mode_t
 */
inline generator_mode_t generator_getMode(void)
{
    return generatorMode;
}

/**
 * 
 */
void generator_setAutoMode(void)
{
    if (generatorMode != GEN_MODE_AUTO) {
        // update generator state
        generatorMode = GEN_MODE_AUTO;
        // 
        generatorState = GEN_STATE_MANUAL_LOW;
        // update hardware depending on the mode
        _updateHardwareSetupForGeneration();
        _slowGeneratorToggle();
    }
}

void _toggleDelay(void)
{
    uint16_t counter = 0;
    
    while(counter++ < 10000) {
        asm("NOP");
    }
}

/**
 * 
 * @param manualState
 */
void generator_setManualState(uint8_t manualState)
{
    if (generatorMode != GEN_MODE_MANUAL) {
        // update generator state
        generatorMode = GEN_MODE_MANUAL;
        // update hardware depending on the mode
        _updateHardwareSetupForGeneration();
    }
    
    if (manualState == 0) {
        // desired level is LOW
        if (generatorState == GEN_STATE_MANUAL_LOW) {
            // if the state is the same, toggle clock once
            leds_setState(LEDS_MANUAL_GREEN);
            GENERATOR_OUT_PIN = 1;
            _toggleDelay();
        }
        // set state low
        generatorState = GEN_STATE_MANUAL_LOW;
        leds_setState(LEDS_MANUAL_RED);
        GENERATOR_OUT_PIN = 0;
    } else {
        // desired level is HIGH
        if (generatorState == GEN_STATE_MANUAL_HIGH) {
            // if the state is the same, toggle clock once
            leds_setState(LEDS_MANUAL_RED);
            GENERATOR_OUT_PIN = 0;
            _toggleDelay();
        }
        // set state high
        generatorState = GEN_STATE_MANUAL_HIGH;
        leds_setState(LEDS_MANUAL_GREEN);
        GENERATOR_OUT_PIN = 1;
    }
}

/**
 * Increase frequency to the next predefined value
 */
void generator_increaseFrequency(void)
{
    if (generatorFrequency < FREQ_2MHz) {
        generatorFrequency++;
        _updateHardwareSetupForGeneration();
    }
}

/**
 * Decrease frequency to the previous predefined value
 */
void generator_decreaseFrequency(void)
{
    if (generatorFrequency > FREQ_500mHz) {
        generatorFrequency--;
        _updateHardwareSetupForGeneration();
    }
}

