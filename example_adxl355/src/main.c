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

	static struct adxl355_frac_repr x[32] = {0};
	static struct adxl355_frac_repr y[32] = {0};
	static struct adxl355_frac_repr z[32] = {0};

	// Initialization parameters
	struct zephyr_spi_init_param zephyr_spi_ip;
	struct no_os_spi_init_param no_os_spi_ip;
	struct adxl355_init_param adxl355_ip;

	struct adxl355_dev *adxl355_device;

	printk("Hello World! %s\n", CONFIG_BOARD_TARGET);


	const struct device *const spi_dev = DEVICE_DT_GET(DT_PARENT(DT_NODELABEL(adxl355)));
	if (!device_is_ready(spi_dev)) {
		printk("%s: device not ready.\n", spi_dev->name);
		return 0;
	}

	struct spi_cs_control cs_ctrl = (struct spi_cs_control){
		.gpio = GPIO_DT_SPEC_GET(DT_PARENT(DT_NODELABEL(adxl355)), cs_gpios),
		.delay = 0u,
	};

	struct spi_config spi_cfg = {
		.frequency = DT_PROP(DT_PARENT(DT_NODELABEL(adxl355)), clock_frequency),
		.operation = SPI_OP_MODE_MASTER | SPI_WORD_SET(8),
		.slave = 0,
		.cs = cs_ctrl,
	};
	
	// zephyr IP
	zephyr_spi_ip.bus = spi_dev;
	zephyr_spi_ip.config = &spi_cfg;
	
	// no os IP
	no_os_spi_ip.extra = &zephyr_spi_ip;
	no_os_spi_ip.platform_ops = &zephyr_spi_ops;
	no_os_spi_ip.device_id = 0;

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
	ret = adxl355_soft_reset(adxl355_device);
	if (ret)
	    goto error;
	ret = adxl355_set_odr_lpf(adxl355_device, ADXL355_ODR_3_906HZ);
	if (ret)
	    goto error;
	ret = adxl355_set_op_mode(adxl355_device, ADXL355_MEAS_TEMP_ON_DRDY_OFF);
	if (ret)
	    goto error;
      
	while(1) {
		ret = adxl355_get_xyz(adxl355_device,&x[0], &y[0], &z[0]);
		if (ret)
			goto error;
		printk(" x=%d"".%09u", (int)x[0].integer, (abs)(x[0].fractional));
		printk(" y=%d"".%09u", (int)y[0].integer, (abs)(y[0].fractional));
		printk(" z=%d"".%09u \n\r", (int)z[0].integer,(abs)(z[0].fractional));
	}
	printk("ADXL355 device is ready\n");

error:
	return 0;
}