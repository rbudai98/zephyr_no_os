/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/drivers/gpio.h>                                                                                                                                                     
#include <zephyr/drivers/spi.h>

#include <max31875.h>
#include <zephyr_i2c.h>


int main(void)
{
	int ret;

	// Initialization parameters
	struct zephyr_i2c_init_param zephyr_i2c_ip;
	struct no_os_i2c_init_param no_os_i2c_ip;
	struct max31875_init_param max31875_ip;

	// Device descriptors
	struct max31875_dev* max31875_desc_ptr;

	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);
	
	struct i2c_dt_spec TEST_DEVICE_FULL = I2C_DT_SPEC_GET(DT_NODELABEL(max31875));

	printk("I2C device addr: %d\n", TEST_DEVICE_FULL.addr);

	// zephyr IP
	zephyr_i2c_ip.i2c_desc = &TEST_DEVICE_FULL;

	// no os IP
	no_os_i2c_ip.extra = &zephyr_i2c_ip;
	no_os_i2c_ip.platform_ops = &zephyr_i2c_ops;
	no_os_i2c_ip.device_id = 1;

	// max31875 ip
	max31875_ip.i2c_init = no_os_i2c_ip;

	ret = max31875_init(&max31875_desc_ptr, &max31875_ip);
	printk("Max31875 init ret: %d\n", ret);

	uint32_t configBuff = 0b0000001000010000;

	ret = max31875_reg_write(max31875_desc_ptr, MAX31875_CONFIGURATION_REG, configBuff);
	if (ret)
	{
		printk("Max31875 reg config set ret: %d\n", ret);
		return ret;
	}
	int32_t val_tmp;
	while (1)
	{
		ret = max31875_reg_read(max31875_desc_ptr, MAX31875_TEMPERATURE_REG, &val_tmp);
		if (ret)
		{
			printk("Max31875 reg read ret: %d\n", ret);
			return ret;
		}
		printk("Temperature: %d\n", (val_tmp >> 8));
	}
	return 0;
}