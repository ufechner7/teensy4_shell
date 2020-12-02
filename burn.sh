teensy_loader_cli --mcu TEENSY40 ./build/zephyr/zephyr.hex
sleep 1
putty -serial /dev/ttyACM0 &
