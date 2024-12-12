/* 
 * File:   buttons.h
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#ifndef BUTTONS_H
#define	BUTTONS_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUTTON_UP_PIN           RA4
#define BUTTON_DOWN_PIN         RA5
#define BUTTON_MODE_PIN         RA3


#define BUTTON_PRESSED          0
#define BUTTON_RELEASED         1
#define BUTTON_PRESS_CNT        1
#define BUTTON_LONGPRESS_CNT    10

// struct of a single button
typedef struct {
    uint8_t lastPinState:1;
    uint8_t pressed:1;
    uint8_t pressedCounter;
    uint8_t longPressed:1;
    uint8_t clicked:1;
} button_state_t;


// struct for all buttons
typedef struct {
    button_state_t buttonUp;
    button_state_t buttonDown;
    button_state_t buttonMode;
} buttons_state_t;

/**
 * 
 */
typedef enum {
    BUTTON_UP,
    BUTTON_DOWN,
    BUTTON_MODE
} button_name_t;

/**
 * 
 * @param name
 * @param state
 */
void buttons_updateButtonState(button_name_t name, uint8_t state);

/**
 * 
 * @return 
 */
inline buttons_state_t * buttons_getState(void);

#ifdef	__cplusplus
}
#endif

#endif	/* BUTTONS_H */

