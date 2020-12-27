# Zephyr on Teensy 4.0

This is a demo application using the shell of Zephyr RTOS on Teensy 4.0 hardware. This is the second part of a series of Tutorials. 

## Preparation
This tutorial was tested on Ubuntu 18.04.

### Install Zephyr 2.4 at the default location
If you do not have installed Zephyr with support for Teensy 4.0 yet, please follow the instructions at https://github.com/ufechner7/zephyr_teensy4_test .

### Check out this sample program
```
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
uart:~$
```

## Standard Output
Standard output is mapped to usb according to [usb/console](https://github.com/zephyrproject-rtos/zephyr/tree/master/samples/subsys/usb/console) example. This is done in ```main.c```. As described there the board will be detected as a CDC_ACM serial device. 

## LED + toggle pin
The led flashes continously to give an visual indication that application is running. The toggle pin is mapped to teensy pin 9 (Teensy 4.0).

**Example**

## Button
On every edge the button pushes a string to a message queue which is then processed by the uart. Pushputton is mapped to teensy pin 23. An external pullup resistor must be used currently.

## UART
The uart always listens for incoming data and dumps it to the log. It also listens for data in the message queue, which are sent out immediately. Used uart is mapped to teens pins 7/8 (Rx2/TX2 in teensy numbering, lpuart4 in nxp numbering). Rx/Tx shorted externally.
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
## Additional features in enabled in prj.conf
### Hardware floating point

### Posix time
```
CONFIG_POSIX_CLOCK=y
CONFIG_DATE_SHELL=y
```
These two settings make it possible to use the posix date time api in programs, but also to set and to read the date and time from the shell.

## Shell

If you type "help" in the console (in putty), you should see the following output:
```

uart:~$ help
Please press the <Tab> button to see all available commands.
You can also use the <Tab> button to prompt or auto-complete all commands or its subcommands.
You can try to call commands with <-h> or <--help> parameter for more information.

Available commands:
  clear    :Clear screen.
  device   :Device commands
  help     :Prints the help message.
  history  :Command history.
  hwinfo   :HWINFO commands
  kernel   :Kernel commands
  log      :Commands for controlling logger
  resize   :Console gets terminal screen size or assumes default in case the
            readout fails. It must be executed after each terminal width change
            to ensure correct text display.
  shell    :Useful, not Unix-like shell commands.
uart:~$
```
You can use the \<TAB> and \<UP> keys as in Linux for command completion and to scroll through the command histoy. To see sub-commands, type \<SPACE>\<TAB> after the main command.

An important command is "kernel stacks":
```
uart:~$ kernel stacks

uart:~$ kernel stacks
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
uart:~$
```
If you press the button (connect pin 23 to shortly to ground) you will see that the stack usage of the pin_id thread increases significantly. Make sure you have enough stack reserved, e.g. in line 22 of file pin.c . If you do not have enough the program will crash without warning. 

## Questions?
In case you have problems to run the above mentioned examples, please create an issue in this repository.

Uwe Fechner
