#include <ArduinoJson.h>
#include<AsyncJson.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <string.h>
#include "conf.h"
#include "WebSocketResponseCodes.h"
#include "uuid.h"

const int * localIPAddress = getIPArray(LOCAL_IP_ADDRESS);
const int * subnetAddress = getIPArray(SUBNET_ADDRESS);
const int * gatewayAddress = getIPArray(GATEWAY_ADDRESS);

const IPAddress local_ip(localIPAddress[0],localIPAddress[1],localIPAddress[2],localIPAddress[3]);
const IPAddress subnet(subnetAddress[0],subnetAddress[1],subnetAddress[2],subnetAddress[3]);
const IPAddress gateway(gatewayAddress[0],gatewayAddress[1],gatewayAddress[2],gatewayAddress[3]);

AsyncWebServer server(80);
AsyncWebSocket ws("/socket");
DynamicJsonDocument spacesJsonDocument(1024);
const char spacesFilePath[] = "/spaces.json";

enum DataType{STRING, INT};

/**
 * Loads current list of spaces of the board and appliances from a JSON file
 */
void loadSpaces()
{
  if(SPIFFS.exists(spacesFilePath))
  {
    File file = SPIFFS.open(spacesFilePath, "r");
    if(!file)
    {
      Serial.println("Could not open spaces file to load current status");
      return;
      } // if
      else
      {
        DeserializationError error = deserializeJson(spacesJsonDocument, file);
        if(error)
        {
          Serial.println("Failed to read json data from spaces file");
          return;
          }
        } // else (!file)
    }
  }

/**
 * Saves current spaces JSON document to file
 */
void saveSpaces()
{
  // Move file to a temporary path
  char *temporaryPath = strdup(spacesFilePath);
  strcat(temporaryPath, ".tmp");
  ets_printf("Temporary file path is %s\n", temporaryPath);
  if(!SPIFFS.rename(spacesFilePath, temporaryPath))
  {
    Serial.println("Unable to backup current spaces file. Aborting save operation!");
    return;
    }
    else
    {
      File file = SPIFFS.open(spacesFilePath, "w");
      if(!file)
      {
        ets_printf("Unable to create new status file at %s. Backup can be found at %s.\n", spacesFilePath, temporaryPath);
        return;
        }
        else
        {
          if(serializeJson(spacesJsonDocument, file) == 0)
          {
            file.close();
            SPIFFS.remove(spacesFilePath);
            ets_printf("Unable to write JSON data to file. Current spaces stay in memory. Backup file can be found at %s.\n", temporaryPath);
            }
            else
            {
              file.close();
              SPIFFS.remove(temporaryPath);
              ets_printf("Saved current list of spaces to file\n");
              }
          } // else (!file)
      } // else (!SPIFFS.rename)
  } 

void validate(JsonObject &json, char * fieldName, DataType fieldType, char *error)
{
  ets_printf("Validating %s\n", fieldName);
  switch(fieldType)
  {
    case STRING:
      ets_printf("Checking STRING\n");
      if(json.containsKey(fieldName))
      {
        ets_printf("JSON contains %s\n", fieldName);
        if(strlen(json[fieldName]) == 0)
        {
          ets_printf("%s was blank\n", fieldName);
          sprintf(error, "'%s' was blank", fieldName);
          }
        }
        else
        {
          ets_printf("%s was missing\n", fieldName);
          sprintf(error, "'%s' was missing", fieldName);
          }
          break;
     default:
     break;
    }
  }

AsyncCallbackJsonWebHandler* createSpace = new AsyncCallbackJsonWebHandler("/api/spaces.json", [](AsyncWebServerRequest *request, JsonVariant &json) {
  AsyncJsonResponse * response = new AsyncJsonResponse();
  response->addHeader("Server", "ESP Async Web API");
  JsonObject root = response->getRoot();
  JsonObject jsonRequest = json.as<JsonObject>();

  if(jsonRequest.isNull() || jsonRequest.size() == 0)
  {
    root["error"] = "Invalid request";
    response->setCode(400);
  }
  else
  {
    JsonObject space = jsonRequest["space"];
  // Return with an error if space JSON was empty or null
  if(space.isNull() || space.size() == 0)
  {
    root["error"] = "'space' was absent in JSON request";
    response->setCode(400); // Bad Request 
    }
    else
    {
      // Validate presence and non-emptiness of fields
      char error[1000];
      validate(space, "name", STRING, error);
      validate(space, "icon", STRING, error);

      if(error[0])
      {
        root["error"] = error;
        response->setCode(400);
        }
        else
        {
          root[uuidv4()] = space;
          response->setCode(200);
          }
      }
  }        
          response->setLength();
          request->send(response);
});

/**
 * Processes an incoming JSON WebSocket message from client
 */
void onJsonMessage(AsyncWebSocketClient * client, const JsonDocument& jsonMessageDocument)
{
  sendJsonSocketMessage(client, "success", "", WS_OK, jsonMessageDocument);
}

void sendJsonSocketMessage(AsyncWebSocketClient * client, char* message_type, int code, char* message){
  StaticJsonDocument<200> nullObject;
  sendJsonSocketMessage(client, message_type, message, code, nullObject);
  }
void sendJsonSocketMessage(AsyncWebSocketClient * client, char* message_type, char* message, int code, const JsonDocument& dataObject)
{
  DynamicJsonDocument jsonDoc(1024);
    JsonObject root = jsonDoc.to<JsonObject>();
    root["type"] = message_type;
    root["message"]= message;
    root["code"] = code;
    if(!dataObject.isNull())
    {
      root["data"] = dataObject;
      }
      
    size_t jsonLength = measureJson(jsonDoc);
    AsyncWebSocketMessageBuffer *messageBuffer = ws.makeBuffer(jsonLength);
    if(messageBuffer)
    {
      serializeJson(root, (char*)messageBuffer->get(), jsonLength+1);
      client->text(messageBuffer);
      }
  }

void onWebSocketEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
  if(type == WS_EVT_CONNECT){
    //client connected
    ets_printf("ws[%s][%u] connect\n", server->url(), client->id());

    String message = String("Hello client "+String(client->id()));
    char messageBuffer[message.length()];
    message.toCharArray(messageBuffer, message.length()+1);

    sendJsonSocketMessage(client, "connect", WS_ACCEPTED, messageBuffer);
    
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
      {
        StaticJsonDocument<1024> jsonMessageDocument;
        // Check if incoming message from client is JSON data or not
        DeserializationError deserializationError = deserializeJson(jsonMessageDocument, (char*)data);
        if(deserializationError) // Not a valid JSON object
        {
          // Echo incoming message
          sendJsonSocketMessage(client, "echo", WS_OK, (char*)data);
        }
        else // serializeJson
        {
          // Process JSON message
          onJsonMessage(client, jsonMessageDocument);
          }
      }
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

    // Load current status
    loadSpaces();
    saveSpaces();
    
    server.on("/api/spaces.json", HTTP_GET, [](AsyncWebServerRequest *request){
      if(spacesJsonDocument.isNull())
      {
        AsyncWebServerResponse *response = request->beginResponse(404, "text/html", "Cannot find current list of spaces!");
        response->addHeader("Server", "ESP Async Web API");
        request->send(response);
        }
        else
        {
          AsyncJsonResponse * response = new AsyncJsonResponse();
          response->addHeader("Server", "ESP Async Web API");
          JsonObject root = response->getRoot();
          root["spaces"] = spacesJsonDocument.to<JsonObject>();
          response->setLength();
          request->send(response);
          }
      });

    server.addHandler(createSpace);
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
