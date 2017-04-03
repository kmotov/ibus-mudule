/*
 * led.c
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#include "led.h"
#include <avr/io.h>

#define sbi(var, mask)   ((var) |= (uint16_t)(1 << mask))  // This are nice functions to let you control 1 pin; Makes High
#define cbi(var, mask)   ((var) &= (uint16_t)~(1 << mask)) // This are nice functions to let you control 1 pin; Makes Low

void led_init (led_t *led)
{
    // todo: led->pin should be replace with DDB0
    switch(led->port) {
    case AVR_PORTB:
        cbi(PORTB, led->pin);
        sbi(DDRB, led->pin);
        break;
    case AVR_PORTC:
        cbi(PORTC, led->pin);
        sbi(DDRC, led->pin);
        break;
    case AVR_PORTD:
        cbi(PORTD, led->pin);
        sbi(DDRD, led->pin);
        break;
    }
}

void led_on(led_t *led)
{
    switch(led->port) {
    case AVR_PORTB:
        sbi(PORTB, led->pin);
        break;
    case AVR_PORTC:
        sbi(PORTC, led->pin);
        break;
    case AVR_PORTD:
        sbi(PORTD, led->pin);
        break;
    };
}

void led_off(led_t *led)
{
    switch(led->port) {
    case AVR_PORTB:
        cbi(PORTB, led->pin);
        break;
    case AVR_PORTC:
        cbi(PORTC, led->pin);
        break;
    case AVR_PORTD:
        cbi(PORTD, led->pin);
        break;
    }
}

