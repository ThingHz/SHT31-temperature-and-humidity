#include <Wire.h>
#define _TASK_TIMEOUT
#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

//SHT31 I2C address
#define Addr 0x44

//static Ip for AP
IPAddress ap_local_IP(192,168,1,4);
IPAddress ap_gateway(192,168,1,254);
IPAddress ap_subnet(255,255,255,0);

//ssid and AP for local WiFi in STA mode
const char WiFissid[] = "*********";
const char WiFipass[] = "*********";
//ssid and pass for AP
const char APssid[] = "********";
const char APpass[] = "********";

boolean connectAP = false;
boolean connectWiFi =false;

//Things speak hostid and apikey 
const char* hostId="Your_Things_speak_hostid";
String apiKey = "Things_speak_api_key";

volatile float humid; 
volatile float tempC;
volatile float tempF;

//prototype for task callback
void taskI2CCallback();
void taskI2CDisable();
void taskAPCallback();
void taskAPDisable();
void taskWiFiCallback();
void taskWiFiDisable();

//webpage1
String PAGE1 = 
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset=\"utf-8\">"
"<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0\">"
"<title> My first HTML page</title>"
"</head>"
"<body>"
"<h1 style= \"color:blue\"> ESP8266 Controlling temperature and Humidity</h1>"
"<p>This web application is deployed on ESP8266 WiFi development board. This web application shows us the real time temperature and humidity value. You can also see the analytics on Things Speak. Here, the real time analytics are provided and very well represented using graphs.</p>"
"<a href=\"https://github.com/vbshightime\">You can go to this link for code</a>"
"<p>Go to next page for real time analytics:</p>"
"<button>"
"<a href=\"/Value\">Page2</a>"
"</button>"
"</body>"
"</html>";


ESP8266WebServer server(80);
Scheduler ts;

//Tasks for i2c, hosting web server and post on thingspeak
Task tI2C(1 * TASK_SECOND, TASK_FOREVER, &taskI2CCallback, &ts, false, NULL, &taskI2CDisable);
Task tAP(5*TASK_SECOND, TASK_FOREVER, &taskAPCallback,&ts,false,NULL,&taskAPDisable);
Task tWiFi(5* TASK_SECOND, TASK_FOREVER, &taskWiFiCallback, &ts, false, NULL, &taskWiFiDisable);

void setup() {
 for(uint8_t t =4;t>=0;t--){
   Serial.println("[SETUP] WAIT...%d \n",t);
   Serial.flush();
   delay(1000);
  }
 
  //begin i2c begin(SDA,SCL)
 Wire.begin(2,14);
 Serial.begin(115200);
  
 server.on("/", onHandleDataRoot);
 server.on("/Value",onHandleDataFeed);
 server.onNotFound(onHandleNotFound);
 
 //timeout for tasks
 tI2C.setTimeout(10 * TASK_SECOND);
 tAP.setTimeout(50 * TASK_SECOND);
 tWiFi.setTimeout(50 * TASK_SECOND); 
 
 //enable I2C task
 tI2C.enable();
}


void loop() {
  //execute the scheduler
  ts.execute(); 
}  

//I2C task callback
 void taskI2CCallback(){

  Serial.println("taskI2CStarted");
  unsigned int root[6];
  
  //begin transmission from 0x44;
    Wire.beginTransmission(Addr);
    //for one shot transmisstion with high repeatability we use 0x2C(MSB) and 0x06(LSB)
    Wire.write(0x2C);
    Wire.write(0x06);
    //end transmission
    Wire.endTransmission();
    //request bytes from 0x44
    
    Wire.beginTransmission(Addr);
    Wire.endTransmission();
    Wire.requestFrom(Addr,6);

    if(Wire.available() == 6){

        //data[0] and data[1] contains 16 bit of temperature.
        root[0] = Wire.read();
        root[1] =Wire.read();
        //data[2] contains 8 bit of CRC
        root[2] = Wire.read();
        //data[3] and data[4] contains 16 bit of humidity
        root[3] = Wire.read();
        root[4] = Wire.read();
        //data[5] consists of 8 bit CRC
        root[5] = Wire.read();
      }
     
  int temp = (root[0] * 256) + root[1];
 
  //shift MSB by 8 bits add LSB
  float cTemp = -45.0 + (175.0 * temp / 65535.0);
  float fTemp = (cTemp * 1.8) + 32.0;
  
  //shift the MSB by 8 bits add LSB to it devide by full resolution and *100 for percentage   
  float humidity = (100.0 * ((root[3] * 256.0) + root[4])) / 65535.0;

  tempC = cTemp;
  tempF = fTemp;
  humid = humidity;
   
   Serial.print("Temperature in C:\t");
   Serial.println(String(cTemp,1));
   Serial.print("Temperature in F:\t");
   Serial.println(String(fTemp,1));
   Serial.print("Humidity:\t ");
   Serial.println(String(humidity,1));
 }

//callback when I2C task disables
void taskI2CDisable(){
  if(tI2C.timedOut()){
        Serial.println("//taskI2C disabled");
        Serial.println("call taskAP");
        //connect to the Access point
        reconnectAPWiFi();
        //enable and call APtask
        tAP.setCallback(&taskAPCallback);
        tAP.enable();
        //disable task I2C
        tI2C.disable();
      }   
  }

//callback for AP task
void taskAPCallback(){
    Serial.println("taskAP started");
    server.handleClient();
  }

//callback when AP task disables
void taskAPDisable(){
  if(tAP.timedOut()){
    Serial.println("//taskI2C disabled");
        Serial.println("call taskAP");
        //connect to WiFi
        reconnectWiFi();
        //enable and call WifItask
        tWiFi.setCallback(&taskWiFiCallback);
        tWiFi.enable();
        //disable task AP
        tAP.disable();
    }
  }

//callback for WiFi
void taskWiFiCallback(){
    WiFiClient wifiClient;
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
      }  

void taskWiFiDisable(){
  if(tWiFi.timedOut()){
    Serial.println("//taskAP disabled");
        Serial.println("call taskI2C");
        //enable I2C task again and call taskI2CCallback
        tI2C.setCallback(&taskI2CCallback);
        tI2C.enable();
        //disables WiFi task
        tWiFi.disable();
    }
  }  

void onHandleDataRoot(){
       server.send(200, "text/html", PAGE1);
    }

  void onHandleDataFeed(){
     String PAGE2 = 
     "<!DOCTYPE html>"
     "<html>"
     "<head>"
     "<meta charset=\"utf-8\">"
     "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0\">"
     "<title> My first HTML page</title>"
     "</head>"
     "<body style= \"background-color:#EDEEF2\">"
     "<h1 style= \"color:blue\">Temperature and Humidity</h1>"
     "<h3 style= \"font-family:verdana; color:blue\">Temperature in Celcius:</h3>"
     "<p style=\"font-size:160%; font-family:Lucida Console\">"+String(tempC,1)+
     "</p>"
     "<h3 style= \"font-family:verdana; color:blue\">Temperature in Fahrenheit:</h3>"
     "<p style=\"font-size:160%; font-family:Lucida Console\">"+String(tempF,1)+
     "</p>"
     "<h3 style= \"font-family:verdana; color:blue\">Humidity:</h3>"
     "<p style=\"font-size:160%; font-family:Lucida Console\">"+String(humid,1)+
     "</p>"
     "</body>"
     "</html>";

     server.send(200,"text/html", PAGE2);
    }

void onHandleNotFound(){
      String message = "File Not Found\n\n";
      message += "URI: ";
      message += server.uri();
      message += "\nMethod: ";
      message += (server.method() == HTTP_GET)?"GET":"POST";
      message += "\nArguments: ";
      message += server.args();
      message += "\n";
      server.send(404, "text/plain", message);
}

void reconnectAPWiFi(){
    WiFi.mode(WIFI_AP_STA);
    delay(100);
    WiFi.disconnect();
    boolean status = WiFi.softAPConfig(ap_local_IP, ap_gateway, ap_subnet);
    if(status ==true){
        Serial.print("Setting soft-AP ... ");
        boolean ap = WiFi.softAP(APssid, APpass);
          if(ap==true){
              Serial.print("connected to:\t");
              //IPAddress myIP = WiFi.softAPIP();
              Serial.println(WiFi.softAPIP());  
            }
            server.begin(); 
      }
}    

void reconnectWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
   
  WiFi.begin(WiFissid,WiFipass);
  while(WiFi.status()!= WL_CONNECTED){
     Serial.print(".");
     delay(500);
    }
    Serial.print("Connected to:\t");
    Serial.println(WiFi.localIP());
}


