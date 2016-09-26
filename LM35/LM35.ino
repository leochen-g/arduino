#include <SPI.h>
#include <Ethernet.h>
#include <math.h>
const int THERM_PIN = A3; //定义模拟端口A3 连接LM35温度传感器
//  yeelink api
#define APIKEY         "6d58041777974e0a50d50********" // APIKEY
#define DEVICEID       345302 // 设备ID
#define SENSORID       386351 // 传感器ID

//初始化MAC地址
byte mac[] = { 0x00, 0x1D, 0x72, 0x82, 0x35, 0x9D};
// 初始化库函数
EthernetClient client;
char server[] = "api.yeelink.net";   // yeelink API


unsigned long lastConnectionTime = 0;          // 上次连接服务器的时间 ms单位
boolean lastConnected = false;                 // 最后一次的主循环
const unsigned long postingInterval = 14 * 1000; // 延时30s发送数据

void setup() {
  Serial.begin(9600);//设置波特率为9600
  //开始连接服务器
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for (;;)
      ;
  }
  else {
    Serial.println("Ethernet configuration OK");
  }
}

void loop() {
  // 数据测试
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

  // 如果没有网络连接 再循环一次然后停止客户端
  if (!client.connected() && lastConnected) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }

  // 网络未连接，10s后重试
  if (!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    //读取数据
    int sensorReading = readtemperSensor();
    Serial.print("yeelink:");//输出数据
    Serial.println(sensorReading);
    //发送数据
    sendData(sensorReading);
  }
  // 通过循环为下次链接做准备
  lastConnected = client.connected();
}

// HTTP连接服务器发送数据 封装报文  学习东西最多的地方 首先要了解如何上传数据，数据格式是什么样的
void sendData(int thisData) {
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    //发送 HTTP PUT 要求
    client.print("POST /v1.0/device/");
    client.print(DEVICEID);
    client.print("/sensor/");
    client.print(SENSORID);
    client.print("/datapoints");
    client.println(" HTTP/1.1");
    client.println("Host: api.yeelink.net");
    client.print("Accept: *");
    client.print("/");
    client.println("*");
    client.print("U-ApiKey: ");
    client.println(APIKEY);
    client.print("Content-Length: ");
    // 计算传感器发送数据的字节
    // 8个字节的数据加上数字的字节数
    int thisLength = 10 + getLength(thisData);
    client.println(thisLength);
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.println("Connection: close");
    client.println();
    // PUT请求的实际内容，从 API文档获得:
    client.print("{\"value\":");
    client.print(thisData);
    client.println("}");
  }
  else {
    // 链接失败提醒
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // 最后一次连接一下
  lastConnectionTime = millis();
}


// 计算数字传感器读数  每一位的ASCII十进制表示一个字节,数字的数量等于字节数:
int getLength(int someValue) {
  int digits = 1;
  // 值不断除以10,添加一个数字计数,直到值为0:
  int dividend = someValue / 10;
  while (dividend > 0) {
    dividend = dividend / 10;
    digits++;
  }
  // 返回数值
  return digits;
}

///////////////////////////////////////////////////////////////////////////
// 传感器数值获取


int readtemperSensor()
{
  int v ;
  int dat;
  v = analogRead(THERM_PIN);//读取传感器模拟值赋给v
  dat = (125 * v) >> 8; //温度计算公式
  /*学习知识
    A/D出来是电压值，如果以mV表示的话，就是：V=Vref*val/1024，这里val是转换后的数值，是整形。
    以为LM35输出电压与温度关系是：V=10mV*T，所以有：
                 10mV*T=Vref*val/1024，这里，Vref=5000mV，即参考电压，
           则：T=500/1024*val，将1024分为：256*4，
           有：T=500/（4*256）*val=125/256*val，C语言位操作中，除以256，就是>>8，
        所以：T=（125*val）>>8
  */
  Serial.print("Sensor value is: ");
  Serial.println(dat);
  delay(500);
  return dat;
}
