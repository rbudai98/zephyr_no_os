# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

# Disable dynamic memory allocation in no-os
add_definitions(-DNO_OS_DRIVER_DYNAMIC_ALLOC=1)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(TEST_DRIVER)
zephyr_include_directories($TOOLCHAIN_ROOT/drivers/gpio)


target_sources(app PRIVATE src/main.c)
