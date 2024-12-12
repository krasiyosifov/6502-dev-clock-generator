/* 
 * File:   interrupts.h
 * Author: krasi.yosifov@gmail.com
 *
 * Created on 09.12.2024
 */

#ifndef INTERRUPTS_H
#define	INTERRUPTS_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t readButtons:1;
    uint8_t stopGenerator:1;
    uint8_t timer1Overflow:1;
} interrupt_flags_t;

extern volatile interrupt_flags_t interrupt_flags;

#ifdef	__cplusplus
}
#endif

#endif	/* INTERRUPTS_H */

