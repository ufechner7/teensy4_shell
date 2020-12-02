# Zephyr on Teensy 4.0

This is a litte application using Zephyr RTOS on Teensy 4.x hardware. It aims to test various hardware peripherals and kernel functions. Maybe it can provide a starting point for real world applications.

Every thread has a start delay of 500ms. This ensures that the device is properly listed on the usb host and comport is open before the log output runs in. 

## Building this example

## Programming the board

## Standard Output
Standard output is mapped to usb according to [usb/console](https://github.com/zephyrproject-rtos/zephyr/tree/master/samples/subsys/usb/console) example. This is done in ```main.c```. As described there the board will be detected as a CDC_ACM serial device. To see the console output, use a command similar to ```minicom -D /dev/ttyACM0```.

## LED + toggle pin
The led flashes continously to give an visual indication that application is running. Toggle pin is mapped to teensy pin 2 (Teensy 4.0).

## Button
On every edge the button pushes a string to a message queue which is then processed by the uart. Pushputton is mapped to teensy pin 23. An external pullup resistor must be used currently.

## UART
The uart always listens for incoming data and dumps it to the log. It also listens for data in the message queue, which are sent out immediately. Used uart is mapped to teens pins 7/8 (Rx2/TX2 in teensy numbering, lpuart4 in nxp numbering). Rx/Tx shorted externally.
