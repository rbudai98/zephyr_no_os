# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Disable dynamic memory allocation in no-os
add_definitions(-DNO_OS_DRIVER_DYNAMIC_ALLOC=1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(TEST_DRIVER)
zephyr_include_directories($TOOLCHAIN_ROOT/drivers/gpio)

# No_os
# Hardcoded path, TODO: fixit
set(NOOS "/home/rbudai/Workspace_no_os/no-os")
zephyr_library_include_directories(${NOOS}/include)

# No_os platform api
zephyr_library_sources(${NOOS}/drivers/api/no_os_i2c.c)

zephyr_library_sources(${NOOS}/drivers/platform/zephyr/zephyr_i2c.c)
zephyr_library_include_directories(${NOOS}/drivers/platform/zephyr)

zephyr_library_sources(${NOOS}/drivers/temperature/max31875/max31875.c)
zephyr_library_include_directories(${NOOS}/drivers/temperature/max31875)

zephyr_library_sources(${NOOS}/util/no_os_alloc.c)
zephyr_library_sources(${NOOS}/util/no_os_mutex.c)


target_sources(app PRIVATE src/main.c)
