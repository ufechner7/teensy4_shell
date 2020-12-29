# Motivation
Teensy 4.0 and 4.1 are very small and powerfull boards, in particular useful for control purposes and IoT. The default software stack (Arduino compatible) is easy to use for beginners, but it is missing important features, like:
- hard real time
- secure network stack, IP6 capable
- secure remote updates
- [CANopen](https://docs.zephyrproject.org/latest/samples/subsys/canbus/canopen/README.html) support

[Zephyr](https://docs.zephyrproject.org/latest/introduction/index.html) gives you all these features, AND your code easily portable to other MCUs and boards. Finally you can easily disable all unused software components, which makes your application smaller, more secure and easier to maintain.
![Teeny 4.0](doc/teensy.jpg) 

# Zephyr on Teensy 4.0

This is a demo application using the shell of Zephyr RTOS on Teensy 4.0 hardware. This is the second part of a series of Tutorials. 

## Preparation
This tutorial was tested on Ubuntu 18.04, but should also work on Windows and MAC with some adaptations.

### Install Zephyr 2.4 at the default location
If you do not have installed Zephyr with support for Teensy 4.0 yet, please follow the instructions at https://github.com/ufechner7/zephyr_teensy4_test .

### Check out this sample program
```bash
cd ~/repos
https://github.com/ufechner7/teensy4_shell
cd teensy4_shell/
```

## Building this example
1. source the script zephyr-env.sh
```source ~/zephyrproject/zephyr/zephyr-env.sh```
2. execute the build script ```./build.sh```

## Programming the board

Connect Teensy 4.0 via USB. It must blink slowly. Press the button.
Execute the script ```./burn.sh```

The script automatically launches the terminal program putty after flashing the board, so you can see the output.

**Example output**
```
[00:00:00.297,000] <wrn> usb_dc_mcux_ehci: endpoint 0x1 already occupied
[00:00:00.420,000] <inf> sys: Started zephyr 2.4.99 on board teensy40.
[00:00:00.500,000] <inf> uart: UART thread started
[00:00:00.500,000] <inf> but: Set up button at GPIO_1 pin 25
[00:00:00.500,000] <inf> led: Thread started
[00:00:00.500,000] <inf> pin: Pin thread started
[00:00:00.500,000] <inf> pin: GPIO_2
[00:00:00.500,000] <inf> pin: PIN 11
teensy:~$
```

## Standard Output
Standard output is mapped either to CDC_ACM_0, which is USB-serial, or to 
UART_6 which is present on pin 0 (RX) and pin 1 (TX), depending on line
27 in prj.conf. Example:
```
CONFIG_UART_SHELL_ON_DEV_NAME="CDC_ACM_0" # or UART_6
```
USB-serial appears on Linux under the name /dev/ttyACM0 .
It can be used with the printk() function even before logging and shell are available.

## LED + toggle pin
The led flashes continously to give an visual indication that application is running. The toggle pin is mapped to teensy pin 9 (Teensy 4.0).

## Button
On every edge the button pushes a string to a message queue which is then processed by the uart. The push button must be connected to teensy pin 23. An external pullup resistor is currently required.

## UART
The uart **lpuart4** always listens for incoming data and dumps it to the log. It also listens for data in the message queue, which are sent out immediately. Used uart is mapped to teens pins 7/8 (Rx2/TX2 in teensy numbering, lpuart4 in nxp numbering). Rx/Tx shorted externally.
If pin 7 and 8 are connected and you press the button, you see
the data that the UART received as follows:
```
[00:06:32.588,000] <dbg> but.button_main_f: Button state is now 1
[00:06:32.590,000] <dbg> uart: Received data:
                               0a 42 75 74 74 6f 6e 20  73 74 61 74 65 20 69 73 |.Button  state is
                               20 31 0d                                         | 1.
[00:06:33.914,000] <dbg> but.button_main_f: Button state is now 0
[00:06:33.916,000] <dbg> uart: Received data:
                               0a 42 75 74 74 6f 6e 20  73 74 61 74 65 20 69 73 |.Button  state is
                               20 30 0d                                         | 0.
```
Be careful: In prj.conf instead of **lpuartX** the name **UART_X** must be used, with X one of the numbers 1 to 7. Every UART that you want to use must be enabled in the device tree (teensy40.overlay) or disabled, if the pins shall be used for something else.

## Additional features in enabled in prj.conf
### Hardware floating point
```
CONFIG_FPU=y
CONFIG_FPU_SHARING=y
```
These two options enable the use of the hardware FPU for single and double float values. The second option saves the FPU registers in case of an interrupt, which increases the time for a switch but enables the use of the FPU from different threads. Don't forget to increase the stack size of your threads when enabling the second option!

### Posix time
```
CONFIG_POSIX_CLOCK=y
CONFIG_DATE_SHELL=y
```
These two settings make it possible to use the posix date time api in programs, but also to set and to read the date and time from the shell.

### Trying out other options
You can use the command ```west build -t menuconfig``` to launch the menu based configuration tool. The settings will get saved in the file ./build/zephyr/.config , but this is only valid temporarily. To make a change permanent look up the relevant commands either in this file, or in the help message of menuconfig and copy the commands manually to prj.conf.

## Shell

If you type "help" in the console (in putty), you should see the following output:
```
teensy:~$ help
Please press the <Tab> button to see all available commands.
You can also use the <Tab> button to prompt or auto-complete all commands or its subcommands.
You can try to call commands with <-h> or <--help> parameter for more information.

Shell supports following meta-keys:
  Ctrl + (a key from: abcdefklnpuw)
  Alt  + (a key from: bf)
Please refer to shell documentation for more details.

Available commands:
  blink    :Turn blinking on or off
  clear    :Clear screen.
  date     :Date commands
  demo     :Demo commands
  device   :Device commands
  help     :Prints the help message.
  history  :Command history.
  hwinfo   :HWINFO commands
  kernel   :Kernel commands
  log      :Commands for controlling logger
  resize   :Console gets terminal screen size or assumes default in case the
            readout fails. It must be executed after each terminal width change
            to ensure correct text display.
  select   :Selects new root command. In order for the command to be selected,
            it must meet the criteria:
            - it is a static command
            - it is not preceded by a dynamic command
            - it accepts arguments
            Return to the main command tree is done by pressing alt+r.
  shell    :Useful, not Unix-like shell commands.

teensy:~$ 
```
You can use the \<TAB> and \<UP> keys as in Linux for command completion and to scroll through the command histoy. To see sub-commands, type \<SPACE>\<TAB> after the main command.

See also: https://docs.zephyrproject.org/latest/reference/shell/index.html

## Examples for shell commands

An important command is "kernel stacks":
```
teensy:~$ kernel stacks

teensy:~$ kernel stacks
0x20202830 uart_id    (real size 504):  unused 360      usage 144 / 504 (28 %)
0x20202630 pin_id     (real size 504):  unused 376      usage 128 / 504 (25 %)
0x20202530 led_id     (real size 504):  unused 376      usage 128 / 504 (25 %)
0x20202730 button_id  (real size 504):  unused 272      usage 232 / 504 (46 %)
0x20202b90 usbworkq   (real size 1024): unused 816      usage 208 / 1024 (20 %)
0x20202a70 shell_uart (real size 2048): unused 1040     usage 1008 / 2048 (49 %)
0x20202970 logging    (real size 768):  unused 656      usage 112 / 768 (14 %)
0x20202cb0 idle 00    (real size 320):  unused 248      usage 72 / 320 (22 %)
0x20202db0 main       (real size 928):  unused 656      usage 272 / 928 (29 %)
0x20206080 IRQ 00     (real size 2144): unused 1488     usage 656 / 2144 (30 %)
teensy:~$
```
If you press the button (connect pin 23 to shortly to ground) you will see that the stack usage of the pin_id thread increases significantly. Make sure you have enough stack reserved, e.g. in line 22 of file pin.c . If you do not have enough the program will crash without warning. 

## Defining your own shell commands
In main.c the top level shell commands demo_params, blink and version are defined. First, you need to include the header:
```C
#include <shell/shell.h> 
```
As example the code for defining and registering the command blink:
```C
/* Function for the processing the blink command */
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

/* Creating root (level 0) command "blink" */
SHELL_CMD_ARG_REGISTER(blink, NULL, "Turn blinking on or off", cmd_blink, 2, 0);
```
We are using global variables to communicate with the led thread. They are defined as follows:
```C
extern bool blink_stat;      /* true = blinking, false = off */
extern const k_tid_t led_id; /* the id of the led thread */
```
The variable blink is initialized at the beginning of main:
```C
bool blink_stat = true;
```

You find more examples in main.c, try them out!

## Communication between threads

In this example, two methods of inter-thread communication are used:
- a message queue
- global variables 

Normal global variables are only safe if they are written in only one thread (here: the main thread) and of a type that can be written atomically in one cycle. For complex types it is better to use a FIFO or a message queue.

We are sending variables of the following type:
```C
typedef struct {
    uint32_t length;
    uint8_t data[50];
}__attribute__((aligned(4))) data_message_t;
```
from the button thread to the uart thread.

The message queue is created in uart.c with the statement, where 10 is the number of messages the queue can hold and 4 the alignment in bytes.
```C
K_MSGQ_DEFINE(data_message_q, sizeof(data_message_t), 10, 4);
```
To put data into the queue, the following code is used:
```C
data_message_t msg;
memcpy(msg.data, (uint8_t *)s, strlen(s));
msg.length = strlen(s);

while (k_msgq_put(&data_message_q, &msg, K_NO_WAIT) != 0) {
    /* message queue is full: purge old data & try again */
    k_msgq_purge(&data_message_q);
                LOG_ERR("Data message queue is full -- purge");
}
```
The while loop ensures that in case of an overflow of the queue the old data is deleted and only the newest message is kept. Normally this should never happen.

The uart thread is reading messages from this queue with the statement:
```C
k_msgq_get(&data_message_q, &tx_msg, K_FOREVER);
```
## Using the shell via a serial port instead of USB
The following changes are needed:

a. change the line 25 of prj.conf from 
```
CONFIG_UART_SHELL_ON_DEV_NAME="CDC_ACM_0"
```
to
```
CONFIG_UART_SHELL_ON_DEV_NAME="UART_8"
```
b. change line 3 of burn.sh from 
```
putty -serial /dev/ttyACM0 &
```
to
```
putty -serial /dev/ttyUSB0 &
```
and connect a 3.3V serial adapter to pin 21 (RX) and pin 20 (TX) of the Teensy 2.0.

## Questions?
In case you have problems to run the above mentioned examples, please create an issue in this repository.

Uwe Fechner
