/*
 * ibus.h
 *
 * Created: 12/12/2012
 *  Author: Konstantin Motov
 */ 


#ifndef IBUS_H_
#define IBUS_H_

#include "defines.h"

#ifdef F_CPU

#define IBUS_CMD_MAX_SIZE 32
#define IBUS_CMD_MIN_SIZE 5

void ibus_init(void);
void ibus_deinit(void);

/* get one message from ibus input message queue
 * return: Null if queue is empty
 *         pointer to first element of the oldest message */
AVR_U8* ibus_get_msg(void);

/* send a message to ibus. Input should be pointer to first element
 * of message. Maximum supported message size is IBUS_CMD_MAX_SIZE */
void  ibus_send_msg(AVR_U8 *cmd);

#else
    #error ERROR: F_CPU is not defined!
#endif

#endif /* IBUS_H_ */
