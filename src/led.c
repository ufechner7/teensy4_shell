/**
 * @file led.c
 * @author Bernhard Kraemer
 * @brief Toggle board led periodically.
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
#define LOG_MODULE_NAME led

#define START_DELAY     500
#define STACK_SIZE      500
#define PRIORITY        5

#define TOGGLE_TIME_MS  1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED0_GPIO_LABEL DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_GPIO_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_GPIO_FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)

/*** Types **************************************************************************/

/*** Variables **********************************************************************/

/*** Function Prototypes ************************************************************/
void led_main_f();

/*** Macros *************************************************************************/
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_INF);

K_THREAD_DEFINE(led_id, STACK_SIZE, led_main_f, NULL, NULL, NULL,
                PRIORITY, 0, START_DELAY);

/*** Function implementation ********************************************************/

void led_main_f()
{
    bool led_is_on = true;
    const struct device *dev;

    dev = device_get_binding(LED0_GPIO_LABEL);
    if (dev == NULL)
    {
        LOG_ERR("Device binding is NULL");
        return;
    }

    if (gpio_pin_configure(dev, LED0_GPIO_PIN, GPIO_OUTPUT_ACTIVE | LED0_GPIO_FLAGS))
    {
        LOG_ERR("GPIO configuration failed");
        return;
    }

    LOG_INF("Thread started");

    while (true)
    {
        gpio_pin_set(dev, LED0_GPIO_PIN, (int)led_is_on);

        LOG_DBG("LED state changed: %u", (int)led_is_on);

        led_is_on = !led_is_on;

        k_msleep(TOGGLE_TIME_MS);
    }
}
