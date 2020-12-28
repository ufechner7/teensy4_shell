/**
 * @file main.c
 * @author Bernhard Kraemer, Uwe Fechner
 * @brief Application startup
 * @version 0.2
 * @date 2020-12-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */

/*** Imported Objects ***************************************************************/
#include <zephyr.h>
#include <logging/log.h>
#include <logging/log_ctrl.h>

#include <usb/usb_device.h>
#include <version.h>
#include <shell/shell.h>

#include "globals.h"

/*** Constants **********************************************************************/
#define LOG_MODULE_NAME sys

#define START_DELAY 420

/*** Initialize global variables ****************************************************/
bool blink_stat = true;

/*** Function Prototypes ************************************************************/
void main(void);

/*** Macros *************************************************************************/
LOG_MODULE_REGISTER(LOG_MODULE_NAME, LOG_LEVEL_DBG);

/*** Function implementation ********************************************************/

void main(void)
{
	const struct device *usb;
	uint32_t version = sys_kernel_version_get();

	usb = device_get_binding(CONFIG_UART_CONSOLE_ON_DEV_NAME);
	if (usb == NULL) {
		return;
	}

	log_init();
	if (usb_enable(NULL)) {
		return;
	}

	// sleep some time after boot to give USB-Client a chance to connect before first log output is produced
	k_msleep(START_DELAY);
	LOG_INF("Started zephyr %u.%u.%u on board %s.", SYS_KERNEL_VER_MAJOR(version), SYS_KERNEL_VER_MINOR(version), SYS_KERNEL_VER_PATCHLEVEL(version), CONFIG_BOARD);

    // this loop is needed to define how often log messages are processed
	while (true) {
        if (log_process(false) == false) {
            k_msleep(30);   /* sleep 30 ms*/
        }
     }
}

static int cmd_demo_ping(const struct shell *shell, size_t argc, char **argv)
{
        ARG_UNUSED(argc);
        ARG_UNUSED(argv);

        shell_print(shell, "pong");
        return 0;
}

static int cmd_demo_params(const struct shell *shell, size_t argc, char **argv)
{
        int cnt;

        shell_print(shell, "argc = %d", argc);
        for (cnt = 0; cnt < argc; cnt++) {
                shell_print(shell, "  argv[%d] = %s", cnt, argv[cnt]);
        }
        return 0;
}

static int cmd_blink(const struct shell *shell, size_t argc, char **argv)
{
		if (argc == 2) {
			if (strcmp(argv[1], "on") == 0) {
				shell_print(shell, "on");
				blink_stat = true;
				k_wakeup(led_id);
			} else if (strcmp(argv[1], "off") == 0) {
				shell_print(shell, "off");
				blink_stat = false;
				k_wakeup(led_id);
			} else {
				shell_print(shell, "Unknown parameter, must be 'on' or 'off'");
			}
		}
        return 0;
}

static int cmd_version(const struct shell *shell, size_t argc, char **argv)
{
        ARG_UNUSED(argc);
        ARG_UNUSED(argv);

        shell_print(shell, "Zephyr version %s", KERNEL_VERSION_STRING);

        return 0;
}

/* Creating subcommands (level 1 command) array for command "demo". */
SHELL_STATIC_SUBCMD_SET_CREATE(sub_demo,
        SHELL_CMD(params, NULL, "Print params command.",
                                               cmd_demo_params),
        SHELL_CMD(ping,   NULL, "Ping command.", cmd_demo_ping),
        SHELL_SUBCMD_SET_END
);
/* Creating root (level 0) command "demo" without a handler */
SHELL_CMD_REGISTER(demo, &sub_demo, "Demo commands", NULL);

/* Creating root (level 0) command "version" */
SHELL_CMD_REGISTER(version, NULL, "Show kernel version", cmd_version);

/* Creating root (level 0) command "blink" */
SHELL_CMD_ARG_REGISTER(blink, NULL, "Turn blinking on or off", cmd_blink, 2, 0);
