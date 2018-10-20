# SHT31-temperature-and-humidity
 
It's an ESP8266 project to measure Temperature and Humidity using SHT31 and Adafruit Huzzah ESP8266. It provides us Real Time Temperature and Humidity data and 
 hourly analytics. The data is send to web Server hosted by the device and to Things Speak API
 

## Features 
 1. Provides you real time analytics and stats using Thing Speak API 
 2. The sensor data can also be seen in Web Server hosted by the device
 3. Task Scheduler are used to Schedule the task like fetching data from sensors, hosting a web server, posting the readings to cloud
 4. It uses I2C protocol to fetch the sensor reading which are more accurate, expandableand scalable
 5. sleep mode when device is idle or no task callback is called.
 6. effective task scheduling provide hastle free usage
 
## Parts Used
 1. [Adafruit esp8266 Huzzah board](https://www.adafruit.com/product/2471)
 2. [Huzzah Board Shield](https://shop.controleverything.com/products/adafruit-huzzah-esp8266-breakout-with-usb-and-i2c-expansion-port)
 3. [SHT31 Sensor module](https://shop.controleverything.com/products/humidity-and-temperature-sensor-2-rh-0-3-c)
 4. [I2C cable](https://store.ncd.io/product/i%C2%B2c-cable/)
 

## How it works
 1. We have scheduled three tasks refering to three different control operations
 2. Task 1 is for reading the sensor value this task runs for 1 second till it reaches timeout of 10 secs.
 3. When the Task1 reaches its time out Task 2 is enabled and Task1 is disabled. We connect to AP in this callback two boolean variables are taken to
 4. Two boolean variables are taken to take care of the switching between STA and AP 
 5. In Task 2 we are hosting a web server at 192.168.1.4 . This task runs for every 5 sec till it reaches its timeout which is 50 sec
 6. When Task 2 reaches timeout Task 3 is enabled and Task2 is disabled. We connect to STA(local IP) in this calback 
 7. In Task 3 we are posting the sensor reading to cloud [ThingSpeak API](https://thingspeak.com/channels/602864)
 8. Task 3 runs for every five seconds till it reached its timeout i.e 50 sec
 9. When the Task3 reaches its time out Task 1 is enabled again and Task3 is disabled.
10. when no callback is called or the device is idle it goes to Light Sleep thus saving power.


## Usage

### Hosting a from webserver

1. connect to following url:
  ```  
  192.168.1.4
  ```
2. open the url connection using `HttpUrlConnection`:
  ```
  192.168.1.4 is an intro page
  192.168.1.4/Value gives the sensor value
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

![alt text](https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/thingSpeak.png)

![alt text]( https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/webpage1.png)

![alt text]( https://github.com/ncdcommunity/SHT31-temperature-and-humidity/blob/master/webpage2.png)


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
4. Createa mesh network using ESp's.
