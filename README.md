### 一、介绍

#### 1、实现功能

通过esp8266 WiFi模块对接OneNET，使用OneNET的HTTP协议，基于RT-Thread的AT device软件包，实现POST数据流到OneNET云平台和从OneNET云平台GET数据流，使用Finsh/MSH测试命令进行测试。

#### 2、软硬件平台

（1）STM32F103RET6、外部12M晶振

（2）ESP8266 WiFi模块

（3）BH1750光照强度传感器

（4）OneNET云平台

（5）RT-Thread物联网操作系统

（6）RT-Thread AT device软件包


### 二、Finsh/MSH测试命令说明

#### 1、开机初始化
开机打印如下信息，可以看到初始化了socket组件、AT client组件（使用uart3）、ESP8266 WIFI连接热点。
![开机初始化信息](https://img-blog.csdnimg.cn/20190513105221850.png)
#### 2、连接OneNET
连接的服务器地址和端口为：`183.230.40.33`，`80`。

在连接之前，可以使用at_ping命令来ping一下这个地址：
![ping](https://img-blog.csdnimg.cn/20190513111210661.png)
ping通说明联网正常和IP地址无误，就可以连接OneNET了，可以先输入`esp8266`查看命令：
![esp8266操作命令](https://img-blog.csdnimg.cn/20190513105731855.png)
输入`esp8266 connect`连接OneNET HTTP服务器：
![esp8266 connect](https://img-blog.csdnimg.cn/20190513110142345.png)
#### 3、POST数据流到OneNET
在连接上OneNET之后，输入`esp8266 post`就可以POST数据流到OneNET：
![esp8266 post](https://img-blog.csdnimg.cn/20190513110444259.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70)
（1）如果POST成功，在OneNET可以看到如下数据：
![OneNET数据显示](https://img-blog.csdnimg.cn/20190513110839123.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70)
（2）如果POST失败，例如：
![post失败](https://img-blog.csdnimg.cn/20190513111407897.png)
那是因为OneNET HTTP是短连接，在前面connect上之后，如果隔了一段时间没进行数据交互就会主动端口断开连接，这时候我们可以先输入`esp8266 close`断开连接，再`esp8266 connect`，然后重新`esp8266 post`：
![重新post](https://img-blog.csdnimg.cn/20190513111749146.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70)
#### 4、从OneNET GET数据流
（1）如果GET数据流成功，会得到数据流名称和数据大小，例如`light`的大小是`432.5`：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190513112154873.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70)（2）如果GET失败，原因也是因为OneNET HTTP是短连接，解决方法和前面类似：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190513112441786.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70) 
#### 4、断开socket连接：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190513112559823.png)

### 三、代码移植说明
#### 1、代码在GitHub
[https://github.com/sanjaywu/RT-Thread_ESP8266_OneNET](https://github.com/sanjaywu/RT-Thread_ESP8266_OneNET)
#### 2、AT device的移植说明
[http://packages.rt-thread.org/itemDetail.html?package=at_device](http://packages.rt-thread.org/itemDetail.html?package=at_device)
#### 3、修改WiFi热点账号和名称
（1）会ENV工具的，请使用ENV工具进行修改。

（2）不会ENV工具，打开`rtconfig.h`，修改两个地方：
```
#define AT_DEVICE_WIFI_SSID "MYWiFi"
#define AT_DEVICE_WIFI_PASSWORD "1234567890"
```
#### 4、修改设备ID、APIKEY和数据流名称
打开`onenet_sample`，修改这三个地方：
```
#define DEVICE_ID	"505619290"	
#define API_KEY		"SlxhH3MCLvuuvXJ0N=a14Yo6EAQ="
#define DATA_STREAM	"light"
```

#### 5、修改ESP8266连接的UART
（1）会ENV工具的，请使用ENV工具进行修改。

（2）不会ENV工具，打开`rtconfig.h`，修改这个地方：

```
#define AT_DEVICE_NAME "uart3"
```

### 四、注意事项
打开工程打开这个：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190513135708550.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1NhbmpheV9XdQ==,size_16,color_FFFFFF,t_70)
