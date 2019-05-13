# BH1750FVI 传感器驱动软件包

## 1 介绍

`BH1750FVI` 传感器软件包提供了使用光照强度传感器 `BH1750FVI` 基本功能，BH1750FVI 是一种用于两线式串行总线接口的数字型光强度传感器集成电路。这种集成电路可以根据收集的光线强度数据来调整液晶或者键盘背景灯的亮度。利用它的高分辨率可以探测较大范围的光强度变化。（1lx-65535lx）。本文介绍该软件包的基本功能，以及 `Finsh/MSH` 测试命令等，这里我做了两个版本的软件包，旧版本和`AHT10`温湿度传感器类似，新版是基于`sensor`框架。

基本功能主要由传感器 `BH1750FVI` 决定：在输入电压为 `2.4v-3.6v` 范围内，测量光照强度的量程如下表所示：

| 功能 | 量程          |
| :------: |:------: |
| 光照强度 | 1lx - 65535lx |

`BH1750FVI` 的分辨率以及测量时间都与测量模式有关，具体如下表所示：

| 测量模式| 测量时间 | 分辨率 |
| :------:|:------: | :------: |
| H-resolution Mode2| Typ. 120ms.|  0.5 lx |
| H-resolution Mode | Typ. 120ms.| 1 lx |
| L-resolution Mode | Typ. 16ms. | 4 lx |

### 1.1 目录结构

| 名称 | 说明 |
| ---- | ---- |
| bh1750.h、sensor_rohm_bh1750.h | 传感器使用头文件 |
| bh1750.c、sensor_rohm_bh1750.c | 传感器使用源代码 |
| SConscript 					| RT-Thread 默认的构建脚本 |
| README.md 					| 软件包使用说明 |
| BH1750FVI_datasheet.pdf		| 官方数据手册 |

### 1.2 许可证

BH1750FVI 传感器软件包遵循  Apache-2.0 许可，详见 LICENSE 文件。

### 1.3 依赖

依赖 `RT-Thread I2C` 设备驱动框架。

## 2 获取基于sensor框架的软件包

使用 `BH1750FVI` 软件包需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages --->
    peripheral libraries and drivers --->
        sensors drivers --->
              [*]   bh1750 sensor driver package, support: ambient light.  --->
                   	Version (latest)  --->
```


每个功能的配置说明如下：

- `bh1750 sensor driver package, support: ambient light`：选择使用 `BH1750FVI` 传感器软件包；
- `Version`：配置软件包版本，默认最新版本。

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 3 使用 基于sensor框架的BH1750FVI 软件包

按照前文介绍，获取 `BH1750FVI` 软件包后，就可以按照 下文提供的 API 使用传感器 `bh1750` 与 `Finsh/MSH` 命令进行测试，详细内容如下。

### 3.1 API

BH1750FVI 软件包初始化函数如下所示：

```
int rt_hw_bh1750_init(const char *name, struct rt_sensor_config *cfg);
```

该函数需要由用户调用，函数主要完成的功能有，

- 设备配置和初始化（根据传入的配置信息，配置接口设备）；
- 注册相应的传感器设备，完成 bh1750 设备的注册；


### 3.2 初始化示例

```
int bh1750_port(void)
{
    struct rt_sensor_config cfg;

    cfg.intf.dev_name = "i2c2";		//根据传感器所挂的I2C设备修改
    cfg.intf.user_data = (void *)BH1750_ADDR;
    cfg.irq_pin.pin = RT_PIN_NONE;

    rt_hw_bh1750_init("bh1750", &cfg);
	
    return 0;
}
INIT_APP_EXPORT(bh1750_port);
```

在使用该传感器前，需进行如上的初始，`cfg.intf.dev_name = "i2c2";`根据所挂载的i2c修改，初始化成功开机会打印如下信息：

```
 \ | /
- RT -     Thread Operating System
 / | \     4.0.1 build Apr 30 2019
 2006 - 2019 Copyright by rt-thread team
[I/sensor] rt_sensor init success
[I/sensor.rohm.bh1750] light sensor init success
msh >s
```


### 3.3 Finsh/MSH 测试命令

BH1750FVI 软件包提供了丰富的测试命令，项目只要在 RT-Thread 上开启 Finsh/MSH 功能即可。在做一些基于 `BH1750FVI` 的应用开发、调试时，这些命令会非常实用，它可以准确的读取传感器测量的光照强度。

#### 3.3.1、输入`list_device`

```
msh >list_device
device           type         ref count
-------- -------------------- ----------
li_bh175 Sensor Device        1
i2c2     I2C Bus              0
uart1    Character Device     2
pin      Miscellaneous Device 0
msh >
```
查看到对应设备以及注册成功。

#### 3.3.2、输入`sensor probe li_bh1750`

```
msh >sensor probe li_bh1750
[I/sensor.cmd] device id: 0xff!
msh >
```

#### 3.3.3、输入`sensor read`

```
msh >sensor read
[I/sensor.cmd] num:  0, light: 494.1, timestamp:2647042
[I/sensor.cmd] num:  1, light: 495.0, timestamp:2647268
[I/sensor.cmd] num:  2, light: 495.0, timestamp:2647494
[I/sensor.cmd] num:  3, light: 490.8, timestamp:2647720
[I/sensor.cmd] num:  4, light: 490.8, timestamp:2647946
msh >
```
读取到光照强度数据，单位：lux。


## 4 获取旧版本软件包

使用 `BH1750FVI` 软件包需要在 RT-Thread 的包管理器中选择它，具体路径如下：

```
RT-Thread online packages
    peripheral libraries and drivers  --->
        [*] BH1750FVI: Ambient Light Sensor IC BH1750FVI driver library  --->
        [*] Enable average filter by software                                    
        (10) The number of averaging
        (1000) Peroid of sampling data(unit ms)                                               
               Version (latest)  --->
```


每个功能的配置说明如下：

- `BH1750FVI: Ambient Light Sensor IC BH1750FVI driver library`：选择使用 `BH1750FVI` 软件包；
- `Enable average filter by software`：开启采集光照强度软件平均数滤波器功能；
- `The number of averaging`：取平均数的采样数目；
- `Peroid of sampling data(unit ms)`：采集数据的周期，时间单位 `ms`；
- `Version`：配置软件包版本，默认最新版本。

然后让 RT-Thread 的包管理器自动更新，或者使用 `pkgs --update` 命令更新包到 BSP 中。

## 5 使用 旧版本BH1750FVI 软件包

按照前文介绍，获取 `BH1750FVI` 软件包后，就可以按照 下文提供的 API 使用传感器 `bh1750` 与 `Finsh/MSH` 命令进行测试，详细内容如下。

### 5.1 API

#### 5.1.1  初始化 

`bh1750_device_t bh1750_init(const char *i2c_bus_name)`

根据总线名称，自动初始化对应的 `bh1750` 设备，具体参数与返回说明如下表

| 参数    | 描述                      |
| :----- | :----------------------- |
| name   | i2c 设备名称 |
| **返回** | **描述** |
| != NULL | 将返回 bh1750 设备对象 |
| = NULL | 查找失败 |

#### 5.1.2  反初始化

void bh1750_deinit(bh1750_device_t dev)

如果设备不再使用，反初始化将回收 `bh1750` 设备的相关资源，具体参数说明如下表

| 参数 | 描述           |
| :--- | :------------- |
| dev  | bh1750 设备对象 |

#### 5.1.3 读取光照强度

float bh1750_read_light(bh1750_device_t dev)

通过 `bh1750` 传感器读取光照强度测量值，返回浮点型光照强度值，具体参数与返回说明如下表

| 参数     | 描述           |
| :------- | :------------- |
| dev      | bh1750 设备对象 |
| **返回** | **描述**       |
| != 0.0   | 测量光照强度值     |
| =00000.0     | 测量失败       |


### 5.2 Finsh/MSH 测试命令

BH1750FVI 软件包提供了丰富的测试命令，项目只要在 RT-Thread 上开启 Finsh/MSH 功能即可。在做一些基于 `BH1750FVI` 的应用开发、调试时，这些命令会非常实用，它可以准确的读取传感器测量的光照强度。具体功能可以输入 `bh1750` ，可以查看完整的命令列表

```
msh />bh1750
Usage:
bh1750 probe <dev_name>   - probe sensor by given name
bh1750 read               - read sensor bh1750 data
msh />
```

#### 5.2.1 在指定的 i2c 总线上探测传感器 

当第一次使用 `bh1750` 命令时，直接输入 `bh1750 probe <dev_name>` ，其中 `<dev_name>` 为指定的 i2c 总线，例如：`i2c2`。如果有这个传感器，就不会提示错误；如果总线上没有这个传感器，将会显示提示找不到相关设备，日志如下：

```
msh />bh1750 probe i2c2      #探测成功，没有错误日志
msh />
msh />bh1750 probe i2c66     #探测失败，提示对应的 I2C 设备找不到
[E/bh1750] can't find bh1750 device on 'i2c66'
msh />
```

#### 5.2.2 读取数据

探测成功之后，输入 `bh1750 read` 即可获取光照强度，包括提示信息，日志如下： 

```
msh />bh1750 read
read bh1750 sensor intensity   : 00431.6 lx
msh />
```


## 6 注意事项

**如果使用基于sensor框架的软件包**，在执行`sensor read`之后没有任何打印出光照强度数据，打开\components\drivers\sensors\sensor_cmd.c,在`sensor_show_data`函数后面自行增加环境光照强度打印代码：
```
case RT_SENSOR_CLASS_LIGHT:
        LOG_I("num:%3d, light:%4d.%d, timestamp:%5d", num, sensor_data->data.light / 10, sensor_data->data.light % 10, sensor_data->timestamp);
        break;
```

## 7 联系方式

* 维护：[Sanjay_Wu](https://github.com/sanjaywu)
* 主页：
* 1、新版本基于sensor框架：[https://github.com/sanjaywu/BH1750FVI](https://github.com/sanjaywu/BH1750FVI "https://github.com/sanjaywu/BH1750FVI")
* 2、旧版本：[https://github.com/sanjaywu/bh1750](https://github.com/sanjaywu/bh1750 "https://github.com/sanjaywu/bh1750")
* 邮箱：sanjaywu@yeah.net




