# Find Zephyr. This also loads Zephyr's build system.
cmake_minimum_required(VERSION 3.13.1)
# find_package(Zephyr)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(my_zephyr_app)

# Add your source file to the "app" target. This must come after
# find_package(Zephyr) which defines the target.
target_sources(app PRIVATE src/main.c)
target_sources(app PRIVATE src/led.c)
target_sources(app PRIVATE src/pin.c)
target_sources(app PRIVATE src/button.c)
target_sources(app PRIVATE src/uart.c)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)