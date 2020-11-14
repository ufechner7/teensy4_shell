/*
 * Copyright (c) 2020 Bernhard Kraemer
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>
#include <logging/log.h>
#include <usb/usb_device.h>

#define SLEEP_TIME_MS   1000


/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED0_GPIO_LABEL		DT_GPIO_LABEL(LED0_NODE, gpios)
#define LED0_GPIO_PIN		DT_GPIO_PIN(LED0_NODE, gpios)
#define LED0_GPIO_FLAGS		DT_GPIO_FLAGS(LED0_NODE, gpios)


/* The devicetree node identifier for the "toggle" alias. */
#define TOGGLE_NODE DT_ALIAS(toggle)
#define TOGGLE_GPIO_LABEL		DT_GPIO_LABEL(TOGGLE_NODE, gpios)
#define TOGGLE_GPIO_PIN		DT_GPIO_PIN(TOGGLE_NODE, gpios)
#define TOGGLE_GPIO_FLAGS		DT_GPIO_FLAGS(TOGGLE_NODE, gpios)


#define DATA_MAX_DLEN 8
#define LOG_MODULE_NAME syst
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);


struct test_frame {
	uint32_t id_type : 1;
	uint32_t rtr     : 1;
	/* Message identifier */
	union {
		uint32_t std_id  : 11;
		uint32_t ext_id  : 29;
	};
	/* The length of the message (max. 8) in byte */
	uint8_t dlc;
	/* The message data */
	union {
		uint8_t data[8];
		uint32_t data_32[2];
	};
} __packed;


void main(void)
{
	const struct device *dev;
	const struct device *dev2;
	const struct device *usb;
	bool led_is_on = true;

	usb = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);
	if (usb == NULL) {
		return;
	}

	if (usb_enable(NULL)) {
		return;
	}

	dev = device_get_binding(LED0_GPIO_LABEL);
	if (dev == NULL) {
		return;
	}

	if (gpio_pin_configure(dev, LED0_GPIO_PIN, GPIO_OUTPUT_ACTIVE | LED0_GPIO_FLAGS)) {
		return;
	}
	
	dev2 = device_get_binding(TOGGLE_GPIO_LABEL);
	if (dev2 == NULL) {
		return;
	}

	if (gpio_pin_configure(dev2, TOGGLE_GPIO_PIN, GPIO_OUTPUT_ACTIVE | TOGGLE_GPIO_FLAGS)) {
		return;
	}



	struct test_frame frame = { 0 };

	const uint8_t data[DATA_MAX_DLEN] = { 0x01, 0x02, 0x03, 0x04,
					0x05, 0x06, 0x07, 0x08 };

	/* sleep some time after boot to give USB-Client a chance to connect */
	k_msleep(500);

	/* standard print */
	LOG_ERR("Error message example.");
	LOG_WRN("Warning message example.");
	LOG_INF("Info message example.");
	LOG_DBG("Debug message example.");

	/* hexdump */
	frame.rtr = 1U;
	frame.id_type = 1U;
	frame.std_id = 1234U;
	frame.dlc = sizeof(data);
	memcpy(frame.data, data, sizeof(data));

	LOG_HEXDUMP_ERR((const uint8_t *)&frame, sizeof(frame), "frame");
	// LOG_HEXDUMP_WRN((const uint8_t *)&frame, sizeof(frame), "frame");
	// LOG_HEXDUMP_INF((const uint8_t *)&frame, sizeof(frame), "frame");
	// LOG_HEXDUMP_DBG((const uint8_t *)&frame, sizeof(frame), "frame");

	/* raw string, no sync to log chronology */
	// printk("hello sys-t on board %s\n", CONFIG_BOARD);

	while (1) {
		gpio_pin_set(dev, LED0_GPIO_PIN, (int)led_is_on);
		gpio_pin_set(dev2, TOGGLE_GPIO_PIN, (int)led_is_on);

		LOG_DBG("LED state changed: %u", (int)led_is_on);

		led_is_on = !led_is_on;

		k_msleep(SLEEP_TIME_MS);
	}
}
