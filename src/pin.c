/**
 * @file pin.c
 * @author Bernhard Kraemer
 * @brief Toggle a pin periodically.
 * @version 0.1
 * @date 2020-11-15
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*** Imported Objects ***************************************************************/
#include <zephyr.h>
#include <logging/log.h>

#include <drivers/gpio.h>

/*** Constants **********************************************************************/
#define LOG_MODULE_NAME pin

#define START_DELAY     500
#define STACK_SIZE      500
#define PRIORITY        5

#define TOGGLE_TIME_MS  493

/* The devicetree node identifier for the "toggle" alias. */
#define TOGGLE_NODE DT_ALIAS(toggle)
#define TOGGLE_GPIO_LABEL DT_GPIO_LABEL(TOGGLE_NODE, gpios)
#define TOGGLE_GPIO_PIN DT_GPIO_PIN(TOGGLE_NODE, gpios)
#define TOGGLE_GPIO_FLAGS DT_GPIO_FLAGS(TOGGLE_NODE, gpios)

/*** Function Prototypes ************************************************************/
void pin_main_f();

/*** Macros *************************************************************************/
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_INF);

K_THREAD_DEFINE(pin_id, STACK_SIZE, pin_main_f, NULL, NULL, NULL,
                PRIORITY, 0, START_DELAY);

/*** Function implementation ********************************************************/

void pin_main_f()
{
    const struct device *dev;
    bool pin_is_on = true;

    dev = device_get_binding(TOGGLE_GPIO_LABEL);
    if (dev == NULL) {
        LOG_ERR("Device binding is NULL");
        return;
    }

    if (gpio_pin_configure(dev, TOGGLE_GPIO_PIN, GPIO_OUTPUT_ACTIVE | TOGGLE_GPIO_FLAGS)) {
        LOG_ERR("GPIO configuration failed");
        return;
    }

    LOG_INF("Pin thread started");
    LOG_INF(TOGGLE_GPIO_LABEL);
    LOG_INF("PIN %u", (int)TOGGLE_GPIO_PIN);

    while (true) {
        gpio_pin_set(dev, TOGGLE_GPIO_PIN, (int)pin_is_on);

        LOG_INF("PIN state changed: %u", (int)pin_is_on);

        pin_is_on = !pin_is_on;

        k_msleep(TOGGLE_TIME_MS);
    }
}
