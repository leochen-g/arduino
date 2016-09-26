#include <SPI.h>  //调用通读接口库
#include <Ethernet.h>  //调用网络模块库
#include <Wire.h>  //调用I2C通讯库
#include <math.h>  //调用函数库
byte buff[2];  //定义缓冲区
float cm;    //浮点型变量
char flag = 0; //定义开关循环控制变量

// for yeelink api

#define APIKEY         "6d58041777974e0a50d508b78ac28db4" // 替换你的 API KEY
#define DEVICEID       345302 // 替换你的device ID
String SENSORno[] = {"386996","386997","386998"}; // 把sensor ID 依次替在这里，程序定义为数组，几个开关就写几个

//for led pin

byte ledPin[] = {A0,A1,A2}; //定义输出的针脚A0-LED1 A2-LED2  A3-LED3

// assign a MAC address for the ethernet controller.

byte mac[] = { 0x00, 0x1D, 0x72, 0x82, 0x35, 0x9D};

// initialize the library instance:

EthernetClient client;

char server[] = "api.yeelink.net";   // name address for yeelink API

unsigned long lastConnectionTime = 0;          // 初始化链接时间

boolean lastConnected = false;                 // state of the connection last time through the main loop

const unsigned long postingInterval = 3 * 1000; // delay between 2 datapoints, 3s

String returnValue = "";

boolean ResponseBegin = false;




void setup() {

  for (int i = 0; i < 3; i++) //3个开关，循环次数

  {
    pinMode(ledPin[i], OUTPUT);
  }


  Wire.begin();

  // start serial port:

  Serial.begin(9600);

  // start the Ethernet connection with DHCP:

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

  // if there's incoming data from the net connection.

  // send it out the serial port.  This is for debugging

  // purposes only:

  if (client.available()) {

    char c = client.read();

    // Serial.print(c);

    if (c == '{')

      ResponseBegin = true;

    else if (c == '}')

      ResponseBegin = false;
    if (ResponseBegin)

      returnValue += c;

  }


  if (returnValue.length() != 0 && (ResponseBegin == false))
  {

    Serial.println(returnValue);

    if (returnValue.charAt(returnValue.length() - 1) == '1') {

      Serial.println("turn on the LED");

      //  digitalWrite( 7, HIGH);

      digitalWrite( ledPin[flag], HIGH);

    }

    else if (returnValue.charAt(returnValue.length() - 1) == '0') {

      Serial.println("turn off the LED");

      //  digitalWrite(7, LOW);

      digitalWrite( ledPin[flag], LOW);
    }

    returnValue = "";

    flag ++;

  }



  // if there's no net connection, but there was one last time

  // through the loop, then stop the client:

  if (!client.connected() && lastConnected) {

    Serial.println();

    Serial.println("disconnecting.");

    client.stop();

  }

  // if you're not connected, and ten seconds have passed since

  // your last connection, then connect again and send data:

  if (!client.connected() && (millis() - lastConnectionTime > postingInterval)) {

    // read sensor data, replace with your code

    //int sensorReading = readLightSensor();

    Serial.print("yeelink:");
    if (flag < 3) // 7改成开关数量
    {

      //get data from server

      getData();

    }

    else

    {

      //put data to server

      //     postData(readData());

      if (flag = 3) {
        flag = 0; // 7改成开关数量
      }

    }




    // note the time that the connection was made or attempted:

    lastConnectionTime = millis();

  }

  // store the state of the connection for next time through

  // the loop:

  lastConnected = client.connected();



}







// this method makes a HTTP connection to the server and get data back

void getData(void) {

  // if there's a successful connection:

  if (client.connect(server, 80)) {

    Serial.println("connecting...");

    // send the HTTP GET request:

    client.print("GET /v1.0/device/");

    client.print(DEVICEID);

    client.print("/sensor/");

    //   client.print(SENSORID);

    client.print(SENSORno[flag]);

    client.print("/datapoints");

    client.println(" HTTP/1.1");

    client.println("Host: api.yeelink.net");

    client.print("Accept: *");

    client.print("/");

    client.println("*");

    client.print("U-ApiKey: ");

    client.println(APIKEY);

    client.println("Content-Length: 0");

    client.println("Connection: close");

    client.println();

    Serial.println("print get done.");

  }

  else

  {

    // if you couldn't make a connection:

    Serial.println("connection failed");

    Serial.println();

    Serial.println("disconnecting.");

    client.stop();

  }

  // note the time that the connection was made or attempted:

  //  lastConnectionTime = millis();
  lastConnectionTime = millis();
}
