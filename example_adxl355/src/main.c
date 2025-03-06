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

#include "adxl355.h"
#include "zephyr_spi.h"


int main(void)
{
	int ret;

	// Initialization parameters
	struct zephyr_spi_init_param zephyr_spi_ip;
	struct no_os_spi_init_param no_os_spi_ip;
	struct adxl355_init_param adxl355_ip;

	struct adxl355_dev *adxl355_device;

	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);

	const struct spi_cs_control cs_ctrl = {
		.gpio = GPIO_DT_SPEC_GET(DT_PARENT(DT_NODELABEL(adxl355)), cs_gpios),
		.delay = 0,
	};

	struct spi_config spi_cfg = {
		.frequency = DT_PROP(DT_PARENT(DT_NODELABEL(adxl355)), clock_frequency),
		.operation = SPI_WORD_SET(8) | SPI_TRANSFER_MSB | SPI_OP_MODE_MASTER,
		.slave = DT_REG_ADDR(DT_NODELABEL(adxl355)),
		.cs = &cs_ctrl,
	};
	const struct device *spi_dev = DEVICE_DT_GET(DT_BUS(DT_NODELABEL(adxl355)));
	if (!device_is_ready(spi_dev)) {
		printk("SPI device not ready\n");
		return -1;
	}
	
	// zephyr IP
	zephyr_spi_ip.bus = spi_dev;
	zephyr_spi_ip.config = &spi_cfg;
	
	// no os IP
	no_os_spi_ip.extra = &zephyr_spi_ip;
	no_os_spi_ip.platform_ops = &zephyr_spi_ops;

	// adxl355 IP
	adxl355_ip.dev_type = ID_ADXL355;
	adxl355_ip.comm_type = ADXL355_SPI_COMM;
	adxl355_ip.comm_init.spi_init = no_os_spi_ip;

	printk("Initilizing adxl355 \n");

	ret = adxl355_init(&adxl355_device, adxl355_ip);
	if (ret < 0) {
		printk("Failed to initialize ADXL355 device\n");
		return ret;
	}

	printk("ADXL355 device is ready\n");
	return 0;
}