/* 
 * File:   generator.h
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#ifndef GENERATOR_H
#define	GENERATOR_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define GENERATOR_OUT_PIN               RC5
    

typedef enum {
    GEN_STATE_MANUAL_LOW = 0,
    GEN_STATE_MANUAL_HIGH,
    GEN_STATE_SLOW_AUTO_LOW,
    GEN_STATE_SLOW_AUTO_HIGH,
    GEN_STATE_FAST_AUTO
} generator_state_t;

typedef enum {
    GEN_MODE_MANUAL,
    GEN_MODE_AUTO
} generator_mode_t;

inline void generator_timer1Callback(void);

/**
 * 
 */
inline void generator_init(void);

/**
 * 
 */
void generator_toggleMode(void);

/**
 * 
 */
inline generator_mode_t generator_getMode(void);

/**
 * 
 */
void generator_setAutoMode(void);

/**
 * 
 */
void generator_stopFast(void);

/**
 * 
 */
void generator_stop(void);

/**
 * 
 * @param manualState
 */
void generator_setManualState(uint8_t manualState);

/**
 * 
 */
void generator_increaseFrequency(void);

/**
 * 
 */
void generator_decreaseFrequency(void);


#ifdef	__cplusplus
}
#endif

#endif	/* GENERATOR_H */

