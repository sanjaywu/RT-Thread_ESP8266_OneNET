/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-16     ZYLX         first implementation
 */

#include <rtthread.h>
#include "bh1750.h"

extern rt_err_t onenet_sample(void);
bh1750_device_t bh1750_dev = RT_NULL;


int main(void)
{
	onenet_sample();
	bh1750_dev = bh1750_init("i2c2");
	
	return 0;
}


