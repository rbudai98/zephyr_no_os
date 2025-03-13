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

#include "zephyr_gpio.h"

volatile int state;

void thread1(void)
{
	int ret;

	struct zephyr_gpio_init_param VCC_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpiof)),
		.pin = 14,
		.flags = GPIO_OUTPUT,
	};
	struct no_os_gpio_init_param VCC_pin_no_os_ip = {
		.extra = &VCC_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *VCC_desc;

	ret = no_os_gpio_get(&VCC_desc, &VCC_pin_no_os_ip);
	if (ret)
		return;

	ret = no_os_gpio_set_value(VCC_desc, NO_OS_GPIO_HIGH);
	if (ret)
		return;


	// const struct device *GPIO_VCC =  DEVICE_DT_GET(DT_NODELABEL(gpiof));
	// if (!device_is_ready(GPIO_VCC)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_VCC");
	// 	return -1;
	// }

	// ret = gpio_pin_configure(GPIO_VCC, 14, GPIO_OUTPUT_HIGH);
	// if (ret < 0) {
	// 	printf("Error %d: failed to configure pin 14\n", ret);
	// 	return -1;
	// }

	struct zephyr_gpio_init_param RED_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpioe)),
		.pin = 11,
		.flags = GPIO_OUTPUT,
	};
	struct no_os_gpio_init_param RED_pin_no_os_ip = {
		.extra = &RED_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *RED_desc;

	ret = no_os_gpio_get(&RED_desc, &RED_pin_no_os_ip);
	if (ret)
		return;

	struct zephyr_gpio_init_param GREEN_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpioe)),
		.pin = 9,
		.flags = GPIO_OUTPUT,
	};
	struct no_os_gpio_init_param GREEN_pin_no_os_ip = {
		.extra = &GREEN_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *GREEN_desc;

	ret = no_os_gpio_get(&GREEN_desc, &GREEN_pin_no_os_ip);
	if (ret)
		return;

	struct zephyr_gpio_init_param BLUE_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpiof)),
		.pin = 13,
		.flags = GPIO_OUTPUT,
	};
	struct no_os_gpio_init_param BLUE_pin_no_os_ip = {
		.extra = &BLUE_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *BLUE_desc;

	ret = no_os_gpio_get(&BLUE_desc, &BLUE_pin_no_os_ip);
	if (ret)
		return;
		

	// const struct device *GPIO_RED =  DEVICE_DT_GET(DT_NODELABEL(gpioe));
	// if (!device_is_ready(GPIO_RED)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_RED");
	// 	return -1;
	// }

	// const struct device *GPIO_GREEN =  DEVICE_DT_GET(DT_NODELABEL(gpioe));
	// if (!device_is_ready(GPIO_GREEN)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_GREEN");
	// 	return -1;
	// }

	// const struct device *GPIO_BLUE =  DEVICE_DT_GET(DT_NODELABEL(gpiof));
	// if (!device_is_ready(GPIO_BLUE)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_BLUE");
	// 	return -1;
	// }

	while(true)
	{
		if (state == 0)
		{
			// gpio_pin_configure(GPIO_RED, 	11, 	GPIO_OUTPUT_LOW);
			// gpio_pin_configure(GPIO_GREEN, 	9,	GPIO_OUTPUT_HIGH);
			// gpio_pin_configure(GPIO_BLUE, 	13, 	GPIO_OUTPUT_HIGH);
			no_os_gpio_set_value(RED_desc, NO_OS_GPIO_LOW);
			no_os_gpio_set_value(GREEN_desc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(BLUE_desc, NO_OS_GPIO_HIGH);
		}
		else if (state == 1)
		{
			// gpio_pin_configure(GPIO_RED, 	11, 	GPIO_OUTPUT_HIGH);
			// gpio_pin_configure(GPIO_GREEN, 	9, 	GPIO_OUTPUT_LOW);
			// gpio_pin_configure(GPIO_BLUE, 	13, 	GPIO_OUTPUT_HIGH);
			no_os_gpio_set_value(RED_desc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(GREEN_desc, NO_OS_GPIO_LOW);
			no_os_gpio_set_value(BLUE_desc, NO_OS_GPIO_HIGH);
		}
		else 
		{
			// gpio_pin_configure(GPIO_RED,	11, 	GPIO_OUTPUT_HIGH);
			// gpio_pin_configure(GPIO_GREEN,	9, 	GPIO_OUTPUT_HIGH);
			// gpio_pin_configure(GPIO_BLUE, 	13, 	GPIO_OUTPUT_LOW);
			no_os_gpio_set_value(RED_desc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(GREEN_desc, NO_OS_GPIO_HIGH);
			no_os_gpio_set_value(BLUE_desc, NO_OS_GPIO_LOW);
		}
		k_msleep(10);
		printk("One cycle\n\r");
	}
}

void thread2(void)
{
	int ret;

	struct zephyr_gpio_init_param X_AXIS_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpiof)),
		.pin = 3,
		.flags = GPIO_INPUT,
	};
	struct no_os_gpio_init_param X_AXIS_pin_no_os_ip = {
		.extra = &X_AXIS_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *X_AXIS_desc;

	ret = no_os_gpio_get(&X_AXIS_desc, &X_AXIS_pin_no_os_ip);
	if (ret)
		return;

	struct zephyr_gpio_init_param Y_AXIS_pin_zephyr_ip ={
		.port = DEVICE_DT_GET(DT_NODELABEL(gpiof)),
		.pin = 5,
		.flags = GPIO_INPUT,
	};
	struct no_os_gpio_init_param Y_AXIS_pin_no_os_ip = {
		.extra = &Y_AXIS_pin_zephyr_ip,
		.platform_ops = &zephyr_gpio_ops,
	};

	struct no_os_gpio_desc *Y_AXIS_desc;

	ret = no_os_gpio_get(&Y_AXIS_desc, &Y_AXIS_pin_no_os_ip);
	if (ret)
		return;
	// const struct device *GPIO_X_AXIS =  DEVICE_DT_GET(DT_NODELABEL(gpiof));
	// if (!device_is_ready(GPIO_X_AXIS)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_X_AXIS");
	// 	return -1;
	// }

	// const struct device *GPIO_Y_AXIS =  DEVICE_DT_GET(DT_NODELABEL(gpiof));
	// if (!device_is_ready(GPIO_Y_AXIS)) {
	// 	printf("Error: GPIO device %s is not ready\n", "GPIO_Y_AXIS");
	// 	return -1;
	// }

	uint8_t val3;
	uint8_t val5;

	while(true)
	{
		ret = no_os_gpio_get_value(X_AXIS_desc, &val3);
		if (ret) 
			return;
		ret = no_os_gpio_get_value(Y_AXIS_desc, &val5);
		if (ret)
			return;
		// val3 = gpio_pin_get(GPIO_X_AXIS, 3);
		// if (val3 < 0) {
		// 	printf("Error %d: failed to read pin 3\n", val3);
		// } else {
		// 	// printf("GPIO_X_AXIS pin 3 value: %d\n", val3);
		// }

		// val5 = gpio_pin_get(GPIO_Y_AXIS, 5);
		// if (val5 < 0) {
		// 	printf("Error %d: failed to read pin 5\n", val5);
		// } else {
		// 	// printf("GPIO_Y_AXIS pin 5 value: %d\n", val5);
		// }
		
		if (val3 == 0 && val5 == 0)
		{
			state = 0;
		}
		else if (val3 == 1 && val5 == 0)
		{
			state = 1;
		}
		else
		{
			state = 2;
		}
		k_msleep(10);
		printk("Second cycle\n\r");
	}
}

K_THREAD_DEFINE(thread1_id, 512, thread1, NULL, NULL, NULL, 7, 0, 0);
K_THREAD_DEFINE(thread2_id, 512, thread2, NULL, NULL, NULL, 7, 0, 0);

int main(void)
{

	printk("Start of firmware\n\r");

	k_thread_start(thread1_id);
	k_thread_start(thread2_id);
	
	return 0;
}