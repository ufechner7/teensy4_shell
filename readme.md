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
  make gcc gcc-multilib g++-multilib libsdl2-dev putty
```
#### Install cmake 3.17.3 ####
```
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo apt install cmake=3.17.3-0kitware1 cmake-data=3.17.3-0kitware1
```
Newer versions of cmake will not work!
#### Install Python dependencies ####
```
pip3 install --user -U west
echo 'export PATH=~/.local/bin:"$PATH"' >> ~/.bashrc
source ~/.bashrc
```
#### Create Zephyr project and update the dependencies ####
```
west init ~/zephyrproject
cd ~/zephyrproject
west update
```
#### Optionally: checkout version 2.4 ####
If you want to use the latest stable version of Zephyr instead of bleeding-edge, execute:
```
cd zephyr
git checkout v2.4-branch
```
#### Install additional requirements ####
```
pip3 install --user -r ~/zephyrproject/zephyr/scripts/requirements.txt
```
### Install toolchain
```
cd ~
wget https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v0.11.4/zephyr-sdk-0.11.4-setup.run
chmod +x zephyr-sdk-0.11.4-setup.run
./zephyr-sdk-0.11.4-setup.run -- -d ~/zephyr-sdk-0.11.4
```
### Install udev rules
```
sudo cp ~/zephyr-sdk-0.11.4/sysroots/x86_64-pokysdk-linux/usr/share/openocd/contrib/60-openocd.rules /etc/udev/rules.d
sudo udevadm control --reload
```
### Check out the board definition of Teensy 4.0
```
cd ~
mkdir repos
cd repos
git clone https://github.com/bdkrae/zephyr.git
cd zephyr
git checkout teensy4-branch
cd ~/zephyrproject/zephyr/boards/arm/
ln -s ~/repos/zephyr/boards/arm/teensy4
```
### Check out this sample program
```
cd ~/repos
git clone https://github.com/ufechner7/zephyr_teensy4_test.git
cd zephyr_teensy4_test/
```

## Building this example
1. source the script zephyr-env.sh
```source ~/zephyrproject/zephyr/zephyr-env.sh```
2. execute the build script ```./build.sh```
## Programming the board
### Install the programming software 
1. check out and compile teensy_loader_cli
```
cd ~/repos
git clone https://github.com/PaulStoffregen/teensy_loader_cli.git
sudo apt-get install libusb-dev
make
cd /usr/local/bin
sudo ln -s ~/repos/teensy_loader_cli/teensy_loader_cli
```
3. install 49-teensy.rules in /etc/udev/rules.d (download them from http://www.pjrc.com/teensy/49-teensy.rules ) and reboot

### Program the board
Connect Teensy 4.0 via USB. It must blink slowly. Press the button.
Execute the script ```./burn.sh```

The script automatically launches the terminal program putty after flashing the board, so you can see the output.

**Example output**
```
[00:00:00.357,000] <inf> usb_cdc_acm: Device configured
[00:00:00.357,000] <wrn> usb_dc_mcux_ehci: endpoint 0x1 already occupied
[00:00:00.420,000] <inf> sys: Started zephyr 2.4.0 on board teensy40.
[00:00:00.500,000] <inf> uart: Thread started
[00:00:00.500,000] <inf> but: Set up button at GPIO_1 pin 25
[00:00:00.500,000] <inf> led: Thread started
[00:00:00.500,000] <inf> pin: Thread started
[00:00:07.754,000] <dbg> but.button_main_f: Button state is now 1
[00:00:07.786,000] <dbg> but.button_main_f: Button state is now 0
```

## Standard Output
Standard output is mapped to usb according to [usb/console](https://github.com/zephyrproject-rtos/zephyr/tree/master/samples/subsys/usb/console) example. This is done in ```main.c```. As described there the board will be detected as a CDC_ACM serial device. 

Known limitation: In the moment the usb console cannot be used as input device.

## LED + toggle pin
The led flashes continously to give an visual indication that application is running. The toggle pin is mapped to teensy pin 2 (Teensy 4.0).

## GPIO names
The names and pin numbers used by the Teensy documentation, the schematics/datasheet and by the Zephyr driver are not consistant. Here a table to translate
these names:
| GPIO | PIN | Name    | Teensy PIN | Remark |
|:----:|:---:|---------|:----------:|--------|
| 0    | -   |AD_B0_2  | 1          | TX1 |
| 0    | -   |AD_B0_3  | 0          | RX1 |
| 0    | 12  |AD_B0_12 | 24 / A10   | Backside |
| 0    | 13  |AD_B0_13 | 25 / A11   | Backside |
| 1    | 0/16   |AD_B1_00 | 19 / A5    | |
| 1    | 1/17   |AD_B1_01 | 18 / A4    | |
| 1    | 2/18   |AD_B1_02 | 14 / A0    | |
| 1    | 3/19   |AD_B1_03 | 15 / A1    | |
| 1    | 6/22   |AD_B1_06 | 17 / A3    | |
| 1    | 7/23   |AD_B1_07 | 16 / A2    | |
| 1    | 8/24   |AD_B1_08 | 22 / A8    | |
| 1    | 9/25   |AD_B1_09 | 23 / A9    | |
| 1    | 14  |AD_B1_14 | 26 / A12   | Backside |
| 1    | 15  |AD_B1_15 | 27 / A13   | Backside |
| 2    | 0/0   |B0_00 | 10 | CS |
| 2    | 1/1   |B0_01 | 12 | MISO |
| 2    | 2/2   |B0_02 | 11 | MOSI |
| 2    | 11/11  |B0_11 | 9  | |
| 2    | 12  |B0_12 | 32 | Backside |
| 3    | -   |B1_00 | 8  | TX2 |
| 3    | -   |B1_01 | 7  | RX2 |
| 4    | 4   |EMC_04 | 2  | not working |
| 4    | 5   |EMC_05 | 3  | not working |
| 4    | 7   |EMC_07 | 33  | Backside |
| 4    | 6   |EMC_06 | 4  |  |
| 4    | 8   |EMC_08 | 5  |  |
| 4    | 31  |EMC_31 | 29  | Backside |
| 4    | 32  |EMC_32 | 28  | Backside |
| 4    | 36  |EMC_36 | 31  | Backside |
| 4    | 37  |EMC_37 | 30  | Backside |

If two PIN numbers are given, the second one works, the first one should work, but does not.

## Button
On every edge the button pushes a string to a message queue which is then processed by the uart. Pushputton is mapped to teensy pin 23. An external pullup resistor must be used currently.

## UART
The uart always listens for incoming data and dumps it to the log. It also listens for data in the message queue, which are sent out immediately. Used uart is mapped to teens pins 7/8 (Rx2/TX2 in teensy numbering, lpuart4 in nxp numbering). Rx/Tx shorted externally.

## Questions?
In case you have problems to run the above mentioned examples, please create an issue in this repository.

Uwe Fechner
