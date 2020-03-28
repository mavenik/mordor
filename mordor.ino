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
AsyncWebSocket ws("/socket");

int os_printf(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    ets_printf("ws[%s][%u] connect\n", server->url(), client->id());
    client->printf("Hello Client %u :)", client->id());
    client->ping();
  } else if(type == WS_EVT_DISCONNECT){
    //client disconnected
    ets_printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
  } else if(type == WS_EVT_ERROR){
    //error was received from the other end
    ets_printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
  } else if(type == WS_EVT_PONG){
    //pong message was received (in response to a ping request maybe)
    ets_printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
  } else if(type == WS_EVT_DATA){
    //data packet
    AwsFrameInfo * info = (AwsFrameInfo*)arg;
    if(info->final && info->index == 0 && info->len == len){
      //the whole message is in a single frame and we got all of it's data
      ets_printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);
      if(info->opcode == WS_TEXT){
        data[len] = 0;
        ets_printf("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < info->len; i++){
          ets_printf("%02x ", data[i]);
        }
        ets_printf("\n");
      }
      if(info->opcode == WS_TEXT)
        client->text("I got your text message");
      else
        client->binary("I got your binary message");
    } else {
      //message is comprised of multiple frames or the frame is split into multiple packets
      if(info->index == 0){
        if(info->num == 0)
          ets_printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
        ets_printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
      }

      ets_printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);
      if(info->message_opcode == WS_TEXT){
        data[len] = 0;
        ets_printf("%s\n", (char*)data);
      } else {
        for(size_t i=0; i < len; i++){
          ets_printf("%02x ", data[i]);
        }
        ets_printf("\n");
      }

      if((info->index + len) == info->len){
        ets_printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
        if(info->final){
          ets_printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
          if(info->message_opcode == WS_TEXT)
            client->text("I got your text message");
          else
            client->binary("I got your binary message");
        }
      }
    }
  }
}

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

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "http://localhost:8080");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
    
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
  ws.cleanupClients();
}
