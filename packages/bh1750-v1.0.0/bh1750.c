/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-04-19     Sanjay_Wu  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <string.h>

#define DBG_ENABLE
#define DBG_SECTION_NAME "bh1750"
#define DBG_LEVEL DBG_LOG
#define DBG_COLOR
#include <rtdbg.h>

#include "bh1750.h"


#ifdef PKG_USING_BH1750_V100

/*bh1750 device address */
#define BH1750_ADDR 0x23

/*bh1750 registers define */
#define BH1750_POWER_DOWN   	0x00	// power down
#define BH1750_POWER_ON			0x01	// power on
#define BH1750_RESET			0x07	// reset	
#define BH1750_CON_H_RES_MODE	0x10	// Continuously H-Resolution Mode
#define BH1750_CON_H_RES_MODE2	0x11	// Continuously H-Resolution Mode2 
#define BH1750_CON_L_RES_MODE	0x13	// Continuously L-Resolution Mode
#define BH1750_ONE_H_RES_MODE	0x20	// One Time H-Resolution Mode
#define BH1750_ONE_H_RES_MODE2	0x21	// One Time H-Resolution Mode2
#define BH1750_ONE_L_RES_MODE	0x23	// One Time L-Resolution Mode


static rt_err_t bh1750_read_regs(struct rt_i2c_bus_device *bus, rt_uint8_t len, rt_uint8_t *buf)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_RD;
    msgs.buf = buf;
    msgs.len = len;

    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
    {
        return RT_EOK;
    }
    else
    {
        return -RT_ERROR;
    }
}

static rt_err_t bh1750_write_cmd(struct rt_i2c_bus_device *bus, rt_uint8_t cmd)
{
    struct rt_i2c_msg msgs;

    msgs.addr = BH1750_ADDR;
    msgs.flags = RT_I2C_WR;
    msgs.buf = &cmd;
    msgs.len = 1;

    if (rt_i2c_transfer(bus, &msgs, 1) == 1)
        return RT_EOK;
    else
        return -RT_ERROR;
}

static rt_err_t bh1750_start(bh1750_device_t dev)
{
    RT_ASSERT(dev);

    bh1750_write_cmd(dev->i2c, BH1750_POWER_ON);
	bh1750_write_cmd(dev->i2c, BH1750_RESET);
	bh1750_write_cmd(dev->i2c, BH1750_CON_H_RES_MODE2);	// Start measurement at 0.5lx resolution. Measurement Time is typically 120ms
	
    return RT_EOK;
}

static float read_hw_light(bh1750_device_t dev)
{
    rt_uint8_t temp[2];
    float current_light = 0.0; // The data is error with missing measurement. 
    rt_err_t result;

    RT_ASSERT(dev);

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
		if (RT_EOK == bh1750_start(dev))
		{
			rt_thread_mdelay(120);
			
			bh1750_read_regs(dev->i2c, 2, temp);
			current_light = (float)((temp[0] << 8) + temp[1]) / 1.2;	// Convert to reality
		}
		else
		{
			LOG_E("The bh1750 could not start. Please try again");
		}
    }
    else
    {
        LOG_E("The bh1750 could not respond relative humidity measurement at this time. Please try again");
    }
    rt_mutex_release(dev->lock);

    return current_light;
}

#ifdef BH1750_USING_SOFT_FILTER

static void average_measurement(bh1750_device_t dev, filter_data_t *filter)
{
    rt_uint32_t i;
    float sum = 0;
    rt_uint32_t temp;
    rt_err_t result;

    RT_ASSERT(dev);

    result = rt_mutex_take(dev->lock, RT_WAITING_FOREVER);
    if (result == RT_EOK)
    {
        if (filter->is_full)
        {
            temp = BH1750_AVERAGE_TIMES;
        }
        else
        {
            temp = filter->index + 1;
        }

        for (i = 0; i < temp; i++)
        {
            sum += filter->buf[i];
        }
        filter->average = sum / temp;
    }
    else
    {
        LOG_E("The software failed to average at this time. Please try again");
    }
    rt_mutex_release(dev->lock);
}

static void bh1750_filter_entry(void *device)
{
    RT_ASSERT(device);

    bh1750_device_t dev = (bh1750_device_t)device;

    while (1)
    {
        if (dev->light_filter.index >= BH1750_AVERAGE_TIMES)
        {
            if (dev->light_filter.is_full != RT_TRUE)
            {
                dev->light_filter.is_full = RT_TRUE;
            }

            dev->light_filter.index = 0;
        }

        dev->light_filter.buf[dev->light_filter.index] = read_hw_light(dev);

        rt_thread_delay(rt_tick_from_millisecond(dev->period));

        dev->light_filter.index++;
    }
}

#endif /* BH1750_USING_SOFT_FILTER */

/**
 * This function reads light intensity by bh1750 sensor measurement
 *
 * @param dev the pointer of device driver structure
 *
 * @return the relative light intensity converted to float data.
 */
float bh1750_read_light(bh1750_device_t dev)
{
#ifdef BH1750_USING_SOFT_FILTER
    average_measurement(dev, &dev->light_filter);

    return dev->light_filter.average;
#else
    return read_hw_light(dev);
#endif /* BH1750_USING_SOFT_FILTER */
}

/**
 * This function initializes bh1750 registered device driver
 *
 * @param dev the name of bh1750 device
 *
 * @return the bh1750 device.
 */
bh1750_device_t bh1750_init(const char *i2c_bus_name)
{
    bh1750_device_t dev;

    RT_ASSERT(i2c_bus_name);

    dev = rt_calloc(1, sizeof(struct bh1750_device));
    if (dev == RT_NULL)
    {
        LOG_E("Can't allocate memory for bh1750 device on '%s' ", i2c_bus_name);
        return RT_NULL;
    }

    dev->i2c = rt_i2c_bus_device_find(i2c_bus_name);
    if (dev->i2c == RT_NULL)
    {
        LOG_E("Can't find bh1750 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

    dev->lock = rt_mutex_create("mutex_bh1750", RT_IPC_FLAG_FIFO);
    if (dev->lock == RT_NULL)
    {
        LOG_E("Can't create mutex for bh1750 device on '%s' ", i2c_bus_name);
        rt_free(dev);
        return RT_NULL;
    }

#ifdef BH1750_USING_SOFT_FILTER
    dev->period = BH1750_SAMPLE_PERIOD;

    dev->thread = rt_thread_create("bh1750", bh1750_filter_entry, (void *)dev, 1024, 15, 10);
    if (dev->thread != RT_NULL)
    {
        rt_thread_startup(dev->thread);
    }
    else
    {
        LOG_E("Can't start filtering function for bh1750 device on '%s' ", i2c_bus_name);
        rt_mutex_delete(dev->lock);
        rt_free(dev);
    }
#endif /* BH1750_USING_SOFT_FILTER */

    bh1750_start(dev);

    return dev;
}

/**
 * This function releases memory and deletes mutex lock
 *
 * @param dev the pointer of device driver structure
 */
void bh1750_deinit(bh1750_device_t dev)
{
    RT_ASSERT(dev);

    rt_mutex_delete(dev->lock);

#ifdef BH1750_USING_SOFT_FILTER
    rt_thread_delete(dev->thread);
#endif

    rt_free(dev);
}

void bh1750(int argc, char *argv[])
{
    static bh1750_device_t dev = RT_NULL;

    if (argc > 1)
    {
        if (!strcmp(argv[1], "probe"))
        {
            if (argc > 2)
            {
                /* initialize the sensor when first probe */
                if (!dev || strcmp(dev->i2c->parent.parent.name, argv[2]))
                {
                    /* deinit the old device */
                    if (dev)
                    {
                        bh1750_deinit(dev);
                    }
                    dev = bh1750_init(argv[2]);
                }
            }
            else
            {
                rt_kprintf("bh1750 probe <dev_name>   - probe sensor by given name\n");
            }
        }
        else if (!strcmp(argv[1], "read"))
        {
            if (dev)
            {
                float light;

                /* read the sensor data */
                light = bh1750_read_light(dev);
                rt_kprintf("read bh1750 sensor intensity   : %d%d%d%d%d.%d lx\n", (int)(light * 10)/100000%10, (int)(light * 10)/10000%10, (int)(light * 10)/1000%10,  \
																					(int)(light * 10)/100%10, (int)(light * 10)/10%10, (int)(light * 10)/1%10);
            }
            else
            {
                rt_kprintf("Please using 'bh1750 probe <dev_name>' first\n");
            }
        }
        else
        {
            rt_kprintf("Unknown command. Please enter 'bh1750' for help\n");
        }
    }
    else
    {
        rt_kprintf("Usage:\n");
        rt_kprintf("bh1750 probe <dev_name>   - probe sensor by given name\n");
        rt_kprintf("bh1750 read               - read sensor bh1750 data\n");
    }
}
MSH_CMD_EXPORT(bh1750, bh1750 sensor function);


#endif /* PKG_USING_BH1750_V100 */

