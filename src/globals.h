/**
 * @file globals.h
 * @author Bernhard Kraemer
 * @brief Project globals
 * @version 0.1
 * @date 2020-11-18
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#ifndef GLOBALS_H_
#define GLOBALS_H_
#include <app_memory/app_memdomain.h>

/*** Constants **********************************************************************/

/*** Types **************************************************************************/
typedef struct {
    uint32_t length;
    uint8_t data[50];
}__attribute__((aligned(4))) data_message_t;

/*** Variables **********************************************************************/
extern bool blink_stat; /* true = blinking, false = off */

#endif /* GLOBALS_H_ */