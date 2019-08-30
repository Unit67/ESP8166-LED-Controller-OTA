#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "Dupa123" //ssid
#define STAPSK  "1234" //password
#endif

const int Red = 13;
const int Green = 12;
const int Blue = 15;

const char* ssid = STASSID;
const char* password = STAPSK;

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  delay(10);
  pinMode(Green, OUTPUT);
  pinMode(Red, OUTPUT);
  pinMode(Blue, OUTPUT);
  //analogWriteFreq(freq);
  //analogWriteRange(512);
  
  analogWrite(Red,512);
  analogWrite(Green,512);
  analogWrite(Blue,512);


  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.println("Connecting to ");
  Serial.println(ssid);


  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  
  Serial.println("");
  Serial.println("Connected");

  //start the server
  server.begin();
  Serial.println("Server Started");

  //print the IP adress
  Serial.println("Use this URL to connect");
  Serial.println("http://");
  Serial.println(WiFi.localIP());
  Serial.println("/");
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  ArduinoOTA.handle();

  while(WiFi.status() != WL_CONNECTED)
  {
    analogWrite(Blue,1023);
    delay(2000);
    analogWrite(Blue,0);
    delay(2000);    
  }

  
  //check if a client has connected
  WiFiClient client = server.available();
  if(!client)
  {
    return;
  }

  //wait until the client sends some data
  Serial.println("new client");
  int TimeWate = 0;
  while(!client.available())
  {
    Serial.println("Client IP: ");
    Serial.println(client.remoteIP());
    delay(1);
    TimeWate = TimeWate + 1;
    if(TimeWate > 1800)
    {
      Serial.println(">>> Client TimeOut");
      client.stop();
      return;
    }
  }

  //read the fist line of the request
  String request = client.readStringUntil('\r');
  client.flush();

  //math the request
  int value = LOW;
  if(request.indexOf("/LED=ON") != -1)
  {
    //digitalWrite(Green, LOW);
    for(int dutyCycle = 1023; dutyCycle > 0; dutyCycle--){
     // changing the LED brightness with PWM
     analogWrite(Green, dutyCycle);
     analogWrite(Blue, dutyCycle);
     analogWrite(Red, dutyCycle);
     delay(1);
    }
    value = HIGH;
  }
  if(request.indexOf("/LED=OFF") != -1)
  {
    //digitalWrite(Green, HIGH);
    for(int dutyCycle = 0; dutyCycle < 1023; dutyCycle++){   
      // changing the LED brightness with PWM
      analogWrite(Green,dutyCycle);
      analogWrite(Blue, dutyCycle);
      analogWrite(Red, dutyCycle);
      delay(1);
    }
    value = LOW;
  }
  if(request.indexOf("/turquoise=ON") != -1)
  {
    for(int dutyCycle = 1023; dutyCycle > 0; dutyCycle--){
     // changing the LED brightness with PWM
     analogWrite(Green, dutyCycle - 800);
     analogWrite(Blue, dutyCycle);
     delay(1);
    }
    value = HIGH;
  }
  if(request.indexOf("/turquoise=OFF") != -1)
  {
    //digitalWrite(Green, HIGH);
    for(int dutyCycle = 0; dutyCycle < 1023; dutyCycle++){   
      // changing the LED brightness with PWM
      analogWrite(Green,dutyCycle);
      analogWrite(Blue, dutyCycle);
      delay(1);
    }
    value = LOW;    
  }

  //return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: Text/html");
  client.println("");
  client.println("");
  client.println("");
  client.print("<center>");
  client.print("<h1>Led Controller</h1>");
  client.println("<a href=\"/LED=ON\"><button class=\"button\">Led ON</button></a>");
  client.println("<a href=\"/LED=OFF\"><button class=\"button\">Led Off</button></a>");
  client.println("<a href=\"/turquoise=ON\"><button class=\"button\">Turquoise On</button></a>");
  client.println("<a href=\"/turquoise=OFF\"><button class=\"button\">Turquoise Off</button></a>");
  client.println("</center>");
}
