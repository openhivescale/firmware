#ifndef __webConfig__
#define __webConfig__

#include "global.h"
#include "memory.h"

//// cd data 
//// for file in `ls -A1 -I gz -I .directory`; do gzip -c $file > gz/$file.gz ; xxd -i gz/$file.gz ../webEmbed_$file.h; sed -i -- 's/unsigned char/const char/g; s/\[\] =/\[\] PROGMEM =/g' ../webEmbed_$file.h; done
#include "webEmbed_edit.htm.h"
#include "webEmbed_favicon.ico.h"
#include "webEmbed_graphs.js.h"
#include "webEmbed_icone57.png.h"
#include "webEmbed_index.htm.h"
#include "webEmbed_logo.png.h"
#include "webEmbed_ace.js.h"

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

const char contentTypehtml[] PROGMEM = "text/html";
const char contentTypejs[] PROGMEM = "application/javascript";
const char contentTypepng[] PROGMEM = "image/png";
const char contentTypeico[] PROGMEM = "image/x-icon";

  
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
