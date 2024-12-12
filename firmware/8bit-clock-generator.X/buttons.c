/**
 * File:   buttons.c
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#include <xc.h>
#include "buttons.h"
#include "types.h"


buttons_state_t buttons = {
    .buttonUp = {
        .pressedCounter = 0,
        .clicked = FALSE,
        .pressed = FALSE,
        .longPressed = FALSE,
        .lastPinState = BUTTON_RELEASED
    },
    .buttonDown = {
        .pressedCounter = 0,
        .clicked = FALSE,
        .pressed = FALSE,
        .longPressed = FALSE,
        .lastPinState = BUTTON_RELEASED
    },
    .buttonMode = {
        .pressedCounter = 0,
        .clicked = FALSE,
        .pressed = FALSE,
        .longPressed = FALSE,
        .lastPinState = BUTTON_RELEASED
    }
};


/**
 * 
 * @param buttonState
 * @param pinHwState
 */
static void _updateState(button_state_t * buttonState, uint8_t pinHwState)
{
    // if button is pressed, it will read 0
    if (pinHwState == BUTTON_PRESSED) {
        if (buttonState->pressedCounter < BUTTON_LONGPRESS_CNT) {
            buttonState->pressedCounter++;
        }
        if (buttonState->pressedCounter == BUTTON_PRESS_CNT) {
            buttonState->pressed = TRUE;
        }
        if (buttonState->pressedCounter == BUTTON_LONGPRESS_CNT) {
            buttonState->longPressed = TRUE;
        }
    } else {
        if (buttonState->pressedCounter >= BUTTON_PRESS_CNT
            && buttonState->pressedCounter < BUTTON_LONGPRESS_CNT
        ) {
            buttonState->clicked = TRUE;
        }
        
        buttonState->pressedCounter = 0;
    }
}


/**
 * 
 * @return buttons_state_t *
 */
inline buttons_state_t * buttons_getState(void)
{
    return &buttons;
}

/**
 * 
 * @param name
 * @param state
 */
void buttons_updateButtonState(button_name_t name, uint8_t state)
{
    switch (name)
    {
        case BUTTON_UP:
            _updateState(&buttons.buttonUp, state);
            break;
        case BUTTON_DOWN:
            _updateState(&buttons.buttonDown, state);
            break;
        case BUTTON_MODE:
            _updateState(&buttons.buttonMode, state);
            break;
        default:
            // invalid name - skip
            break;
    }
}
