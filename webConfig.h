#ifndef __webConfig__
#define __webConfig__

#include "global.h"
#include "memory.h"


#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
//#include <ArduinoOTA.h>
#include <ESP8266HTTPUpdateServer.h>
//#include <ESP8266mDNS.h>
#include <ESP8266Ping.h>

ESP8266HTTPUpdateServer httpUpdater;
ESP8266WebServer server(80);

//holds the current upload
File fsUploadFile;

void webConfigInit();
void initWebServer();
String formatBytes(size_t bytes);
String getContentType(String filename);
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void handleFileCreate();
void handleFileList();

void delayWithWebHandle(int d);

#endif
