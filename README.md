# SHT31-temperature-and-humidity
 
It's an ESP8266 project to measure Temperature and Humidity using SHT31 and Adafruit Huzzah ESP8266. It provides us Real Time Temperature and Humidity data and hourly analytics. The data is sent to web Server hosted by the device and to Things Speak API
 

## Features 

  - Provides you real time analytics and stats using Thing Speak API 
  - The sensor data can also be seen in Web Server hosted by the device
  - Task Scheduler are used to Schedule the task like fetching data from sensors, hosting a web server, posting the readings to cloud
  - It uses I2C protocol to fetch the sensor reading which are more accurate, expandable and scalable
  - sleep mode when device is idle or no task callback is called.
  - effective task scheduling provide hastle free usage
 
## Hardware Specification
  - [Adafruit esp8266 Huzzah board](https://www.adafruit.com/product/2471)
  - [Huzzah Board Shield](https://shop.controleverything.com/products/adafruit-huzzah-esp8266-breakout-with-usb-and-i2c-expansion-port)
  - [SHT31 Sensor module](https://shop.controleverything.com/products/humidity-and-temperature-sensor-2-rh-0-3-c)
  - [I2C cable](https://store.ncd.io/product/i%C2%B2c-cable/)
 

## How it works
  -  We have scheduled three tasks refering to three different control operations
  - Task 1 is for reading the sensor value this task runs for 1 second till it reaches timeout of 10 secs.
  - When the Task1 reaches its time out Task 2 is enabled and Task1 is disabled. We connect to AP in this callback two boolean variables are taken to
  - Two boolean variables are taken to take care of the switching between STA and AP 
  - In Task 2 we are hosting a web server at 192.168.1.4 . This task runs for every 5 sec till it reaches its timeout which is 50 sec
  - When Task 2 reaches timeout Task 3 is enabled and Task2 is disabled. We connect to STA(local IP) in this calback 
  - In Task 3 we are posting the sensor reading to cloud [ThingSpeak API](https://thingspeak.com/channels/602864)
  - Task 3 runs for every five seconds till it reached its timeout i.e 50 sec
  - When the Task3 reaches its time out Task 1 is enabled again and Task3 is disabled.
  - when no callback is called or the device is idle it goes to Light Sleep thus saving power.


## Usage

### Hosting a from webserver

1. connect to following url:
  ```  
  192.168.1.4
  ```

  ```
  192.168.1.4 is an intro page
  This values gives the sensor value
  192.168.1.4/Value 
 ```

### POST data to thing speak

```
  if(wifiClient.connect(hostId,80)){
            String postStr = apiKey;
            postStr +="&field1=";
            postStr += String(humid);
            postStr +="&field2=";
            postStr += String(tempC);
            postStr +="&field3=";
            postStr += String(tempF);
            postStr += "\r\n\r\n";

            wifiClient.print("POST /update HTTP/1.1\n");
                             wifiClient.print("Host: api.thingspeak.com\n");
                             wifiClient.print("Connection: close\n");
                             wifiClient.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             wifiClient.print("Content-Type: application/x-www-form-urlencoded\n");
                             wifiClient.print("Content-Length: ");
                             wifiClient.print(postStr.length());
                             wifiClient.print("\n\n");
                             wifiClient.print(postStr);
          }
          wifiClient.stop();
```


## Profile

![alt text](https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/CaptureHumidity.PNG)


![alt text]( https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/CaptureTEmpGauge.PNG)


![alt text]( https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/CaptureTempC.PNG)


![alt text](https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/CaptureTempF.PNG)


![alt text]( https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/CaptureWeb2.PNG)


![alt text](https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/Captureweb1_1.PNG)


## Credits

1. Arduino JSON: https://arduinojson.org/v5/doc/encoding/ 
2. ESP8266: https://links2004.github.io/Arduino/d3/d58/class_e_s_p8266_web_server.html
3. TaskScheduler: https://github.com/arkhipenko/TaskScheduler
4. SHT 31: https://github.com/ControlEverythingCommunity/SHT31/blob/master/Arduino/SHT31.ino
5. I2C scan: https://github.com/bhaskar-anil429/arduino-i2c-scan
6. https://www.instructables.com/id/ESP8266-Temp-Humidity-Monitoring-Web-App-Using-Ard/
7. Wire: https://www.arduino.cc/en/Reference/Wire

## Future Enhancement 
1. To host another web page where the user can config its wifi credentials.
2. Deep sleep mode- We can put ESP to deep sleep mode to conserve power
3. To enhance the no. of sensors using i2c
4. Create a mesh network using ESp's.
