/**
 * @file button.c
 * @author Bernhard Kraemer
 * @brief Catch a debounced pushbutton state.
 * @version 0.1
 * @date 2020-11-16
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*** Imported Objects ***************************************************************/
#include <zephyr.h>
#include <logging/log.h>

#include <drivers/gpio.h>
#include <globals.h>

#include "stdio.h"
#include "string.h"

/*** Constants **********************************************************************/
#define LOG_MODULE_NAME but

#define START_DELAY     500
#define STACK_SIZE 1000
#define PRIORITY        5

#define SLEEP_TIME_MS   30

/*
 * Get button configuration from the devicetree sw0 alias.
 *
 * At least a GPIO device and pin number must be provided. The 'flags'
 * cell is optional.
 */
#define SW0_NODE DT_ALIAS(sw0)
#define SW0_GPIO_LABEL DT_GPIO_LABEL(SW0_NODE, gpios)
#define SW0_GPIO_PIN DT_GPIO_PIN(SW0_NODE, gpios)
#define SW0_GPIO_FLAGS (GPIO_INT_DEBOUNCE | GPIO_INPUT | DT_GPIO_FLAGS(SW0_NODE, gpios))

/*** Function Prototypes ************************************************************/
void button_main_f();

/*** Macros *************************************************************************/
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

K_THREAD_DEFINE(button_id, STACK_SIZE, button_main_f, NULL, NULL, NULL,
                PRIORITY, 0, START_DELAY);

K_MSGQ_DEFINE(data_message_q, sizeof(data_message_t), 10, 4);

/*** Function implementation ********************************************************/

void button_main_f()
{
    const struct device *button;
    int ret;
    bool currentState = 0;
    bool newState = 0;

    button = device_get_binding(SW0_GPIO_LABEL);
    if (button == NULL) {
        LOG_ERR("Error: didn't find %s device", SW0_GPIO_LABEL);
        return;
    }

    ret = gpio_pin_configure(button, SW0_GPIO_PIN, SW0_GPIO_FLAGS);
    if (ret != 0) {
        LOG_ERR("Error %d: failed to configure %s pin %d, flags %u",
               ret, SW0_GPIO_LABEL, SW0_GPIO_PIN, SW0_GPIO_FLAGS);
        return;
    }

    LOG_INF("Set up button at %s pin %d", SW0_GPIO_LABEL, SW0_GPIO_PIN);

    while(true) {
        newState = gpio_pin_get(button, SW0_GPIO_PIN); 
        if (newState != currentState) {
            LOG_DBG("Button state is now %u", newState);
            currentState = newState;

            char s[20];
            sprintf(s, "Button state is %u\r\n", newState);
            data_message_t msg;
            memcpy(msg.data, (uint8_t *)s, strlen(s));
            msg.length = strlen(s);

            while (k_msgq_put(&data_message_q, &msg, K_NO_WAIT) != 0) {
                /* message queue is full: purge old data & try again */
                k_msgq_purge(&data_message_q);
                LOG_ERR("Data message queue is full -- purge");
            }
        }
        k_msleep(SLEEP_TIME_MS);
    }
}
