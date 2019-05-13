/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-19     Sanjay_Wu  the first version
 */

#ifndef __BH1750_H__
#define __BH1750_H__

#include <rthw.h>
#include <rtthread.h>

#include <rthw.h>
#include <rtdevice.h>

#ifdef BH1750_USING_SOFT_FILTER
typedef struct filter_data
{
    float buf[BH1750_AVERAGE_TIMES];
    float average;

    rt_off_t index;
    rt_bool_t is_full;

} filter_data_t;
#endif /* BH1750_USING_SOFT_FILTER */

struct bh1750_device
{
    struct rt_i2c_bus_device *i2c;

#ifdef BH1750_USING_SOFT_FILTER
    filter_data_t light_filter;

    rt_thread_t thread;
    rt_uint32_t period;
#endif /* BH1750_USING_SOFT_FILTER */

    rt_mutex_t lock;
};
typedef struct bh1750_device *bh1750_device_t;


/**
 * This function reads light intensity by bh1750 sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative light intensity converted to float data.
 */
float bh1750_read_light(bh1750_device_t dev);

/**
 * This function initializes bh1750 registered device driver
 *
 * @param dev the name of bh1750 device
 *
 * @return the bh1750 device.
 */
bh1750_device_t bh1750_init(const char *i2c_bus_name);

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void bh1750_deinit(bh1750_device_t dev);




#endif /* __BH1750_H__ */
