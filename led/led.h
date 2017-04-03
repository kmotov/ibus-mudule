/*
 * led.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 

#ifndef LED_H_
#define LED_H_

#include "defines.h"

typedef struct {
    AVR_U8 port; 
    AVR_U8 pin;
} led_t;

void led_init(led_t *led);
void led_on(led_t *led);
void led_off(led_t *led);

#endif /* LED_H_ */
