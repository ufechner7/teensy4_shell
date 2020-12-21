# Zephyr on Teensy 4.0

This is a litte application using Zephyr RTOS on Teensy 4.0 hardware. It aims to test various hardware peripherals and kernel functions. Maybe it can provide a starting point for real world applications.

Every thread has a start delay of 500ms. This ensures that the device is properly listed on the usb host and com port is open before the log output runs in. 

## Preparation
This tutorial was tested on Ubuntu 18.04. 
### Install Zephyr 2.4 at the default location
#### Update OS ####
```
sudo apt update
sudo apt upgrade
```
#### Install dependencies ####
```
sudo apt install --no-install-recommends git cmake ninja-build gperf \
  ccache dfu-util device-trcpupower frequency-set --governor performanceee-compiler wget \
  python3-dev python3-pip python3-setuptools python3-tk python3-wheel xz-utils file \
  make gcc gcc-multilib g++-multilib libsdl2-dev
```
#### Install cmake 3.13.1 or higher ####
```
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo apt install cmake
```
#### Install Python dependencies ####
```
pip3 install --user -U west
echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
source ~/.bashrc
```
#### Create Zephyr project and checkout version 2.4 ####
```
west init ~/zephyrproject
cd ~/zephyrproject
west update
cd zephyr
git checkout v2.4-branch
```
#### Install additional requirements ####
```
pip3 install --user -r ~/zephyrproject/zephyr/scripts/requirements.txt
```
### Install toolchain

### Check out the board definition of Teensy 4.0

### Check out this sample program

## Building this example
1. source the script zephyr-env.sh
```source ~/zephyrproject/zephyr/zephyr-env.sh```
2. execute the build script ```./build.sh```
## Programming the board
### Install the programming software 

### Program the board
Execute the script ```./burn.sh```

The script automatically launches the terminal program putty after flashing the board, so you can see the output.

## Standard Output
Standard output is mapped to usb according to [usb/console](https://github.com/zephyrproject-rtos/zephyr/tree/master/samples/subsys/usb/console) example. This is done in ```main.c```. As described there the board will be detected as a CDC_ACM serial device. 

## LED + toggle pin
The led flashes continously to give an visual indication that application is running. Toggle pin is mapped to teensy pin 2 (Teensy 4.0).

## Button
On every edge the button pushes a string to a message queue which is then processed by the uart. Pushputton is mapped to teensy pin 23. An external pullup resistor must be used currently.

## UART
The uart always listens for incoming data and dumps it to the log. It also listens for data in the message queue, which are sent out immediately. Used uart is mapped to teens pins 7/8 (Rx2/TX2 in teensy numbering, lpuart4 in nxp numbering). Rx/Tx shorted externally.
