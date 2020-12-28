/**
 * @file globals.h
 * @author Bernhard Kraemer, Uwe Fechner
 * @brief Project globals
 * @version 0.2
 * @date 2020-12-28
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_

/*** Constants **********************************************************************/

/*** Types **************************************************************************/
typedef struct {
    uint32_t length;
    uint8_t data[50];
}__attribute__((aligned(4))) data_message_t;

/*** Variables **********************************************************************/
extern bool blink_stat;      /* true = blinking, false = off */
extern const k_tid_t led_id; /* the id of the led thread */

#endif /* GLOBALS_H_ */