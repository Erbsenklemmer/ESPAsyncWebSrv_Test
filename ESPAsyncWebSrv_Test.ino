#include <Arduino.h>

//#include <ESP32Servo.h> // by Kevin Harrington
#include <ESPAsyncWebSrv.h> // by dvarrel
#include <iostream>
#include <sstream>

#if defined(ESP32)
#include <AsyncTCP.h> // by dvarrel
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESPAsyncTCP.h> // by dvarrel
#endif

#include "HtmlPage.h"
//extern const char* htmlHomePage PROGMEM;
const char* ssid     = "Mini SkidSteer";

AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");


void handleRoot(AsyncWebServerRequest *request)
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "File Not Found");
}

void onCarInputWebSocketEvent(AsyncWebSocket *server,
                              AsyncWebSocketClient *client,
                              AwsEventType type,
                              void *arg,
                              uint8_t *data,
                              size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      //moveCar(STOP);
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        std::istringstream ss(myData);
        std::string key, value;
        std::getline(ss, key, ',');
        std::getline(ss, value, ',');
        Serial.printf("Key [%s] Value[%s]\n", key.c_str(), value.c_str());
        int valueInt = atoi(value.c_str());
        
        Serial.printf("WebSocket client #%u command(key): '%s' with  valueInt: %d\n", client->id(), key.c_str(), valueInt);

        // if (key == "MoveCar")
        // {
        //   moveCar(valueInt);
        // }
        // else if (key == "AUX")
        // {
        //   auxControl(valueInt);
        // }
        // else if (key == "Bucket")
        // {
        //   bucketTilt(valueInt);
        // }
        // else if (key == "Light")
        // {
        //   lightControl();
        // }
        // else if (key == "Switch")
        // {
        //   if (!(horizontalScreen))
        //   {
        //     horizontalScreen = true;
        //   }
        //   else {
        //     horizontalScreen = false;
        //   }
        // }
      }
      break;
    case WS_EVT_PONG:
      Serial.printf("WebSocket client #%u PONG\n", client->id());
      break;
    case WS_EVT_ERROR:
      Serial.printf("WebSocket client #%u ERROR\n", client->id());
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);

  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);

  server.begin();
  Serial.println("HTTP server started");


}
int count = 0;
void loop() {
  wsCarInput.cleanupClients();

  if (count < 20)
  {
    count++;
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);
  }
}
