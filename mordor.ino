#include <WiFi.h>

#include <ESPAsyncWebServer.h>

/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include "SPIFFS.h"
#include "conf.h"

LOCAL_IP_ADDRESS;
SUBNET_ADDRESS;
GATEWAY_ADDRESS;

AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);

    if(!WiFi.config(local_ip, subnet, gateway)){
      Serial.println("Failed to configure WiFi static IP");
      }
      
    if(!SPIFFS.begin()){
      Serial.println("An error occurred while initializing SPIFFS");
      }

    WiFi.begin(ssid, wifi_password);

    Serial.print("Connecting to WiFi network");
    
    while(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print(".");
      }

    Serial.println("");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
//
//    
//    server.serveStatic("/static/css/", SPIFFS, "/web/static/css/")
//    .setCacheControl("max-age=31536000")
//    .setAuthentication("admin", "password");
//    
//    server.serveStatic("/static/js/", SPIFFS, "/web/static/js/")
//    .setCacheControl("max-age=31536000")
//    .setAuthentication("admin", "password");
//    
//    server.serveStatic("/static/media/", SPIFFS, "/web/static/media/")
//    .setCacheControl("max-age=31536000")
//    .setAuthentication("admin", "password");

    server.serveStatic("/", SPIFFS, "/web/")
    .setDefaultFile("index.html")
    .setAuthentication(admin_user, admin_password);

    server.onNotFound([](AsyncWebServerRequest *request){
      AsyncWebServerResponse *response = request->beginResponse(404, "text/html", "404: Not Found!");
      response->addHeader("Server", "ESP Async Web Server");
      request->send(response);
      });

    server.begin();

    Serial.println("Server started!");
}

void loop()
{
}
