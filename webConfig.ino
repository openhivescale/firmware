#include "webConfig.h"


void webConfigInit() {
  // Port defaults to 8266
  //ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  //ArduinoOTA.setHostname(host);
  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"123");

  //ArduinoOTA.begin();
  httpUpdater.setup(&server);


  //MDNS.begin(host);
  /*debug("Open http://", true);
  debug(host, true);
  debug(".local/edit to see the file browser");*/


  initWebServer();
    
}
void initWebServer() {
  //SERVER INIT
  //list directory
  server.on("/list", HTTP_GET, handleFileList);
  //load editor
  /*server.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
  });*/
  //create file
  server.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  server.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  server.on("/edit", HTTP_POST, []() {
    server.send(200, "text/plain", "");
  }, handleFileUpload);



  
  server.on("/", HTTP_GET, []() {
    server.send_P(200,contentTypehtml,index_htm,index_htm_len);
  });
  
  server.on("/index.htm", HTTP_GET, []() {
    server.send_P(200,contentTypehtml,index_htm,index_htm_len);
  });

  server.on("/edit.htm", HTTP_GET, []() {
    server.send_P(200,contentTypehtml,edit_htm,edit_htm_len);
  });
  
  server.on("/edit", HTTP_GET, []() {
    server.send_P(200,contentTypehtml,edit_htm,edit_htm_len);
  });
  
  server.on("/favicon.ico", HTTP_GET, []() {
    server.send_P(200,contentTypeico,favicon_ico,favicon_ico_len);
  });

  server.on("/graphs.js", HTTP_GET, []() {
    server.send_P(200,contentTypejs,graphs_js,graphs_js_len);
  });

  server.on("/icone57.png", HTTP_GET, []() {
    server.send_P(200,contentTypepng,icone57_png,icone57_png_len);
  });

  server.on("/logo.png", HTTP_GET, []() {
    server.send_P(200,contentTypepng,logo_png,logo_png_len);
  });

  
  
  //called when the url is not defined here
  //use it to load content from SPIFFS
  server.onNotFound([]() {
    if (!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });






  //get heap status, analog input value and all GPIO statuses in one json call
  server.on("/all", HTTP_GET, []() {
    String json = "{";
    json += "\"millis\":" + String(millis());
    json += ",\"opticalValue\":" + String(opticalValue);
    json += ",\"optDiff\":" + String(optDiff);
    json += ",\"diffMin\":" + String(diffMin);
    json += ",\"diffMax\":" + String(diffMax);
    json += ",\"diffCC\":" + String(diffCC);
    json += ",\"motorPosition\":" + String(motorPosition);
    //json += ",\"motorLastBalance\":" + String(motorLastBalance);
    json += ",\"motorTarget\":" + String(motorTarget);
    //json += ",\"wakeUpPeriod\":" + String(wakeUpPeriod);
    json += ",\"sigfoxSendEnable\":" + String(sigfoxSendEnable);
    json += ",\"motorCheck\":" + String(motorCheck);
    json += ",\"nowStr\":\"" + nowStr() + "\"";
    json += ",\"chipId\":\"" + String(ESP.getChipId()) + "\"";
    json += ",\"scanComplete\":\"" + String(WiFi.scanComplete()) + "\"";
    json += "}";
    server.send(200, "text/json", json);
    json = String();
    //debug("/all : ",true);
    //debug(millis());
  });

  server.on("/liveValues", HTTP_GET, []() {
    String json = "{";
    json += "\"opticalValue\":" + String(opticalValue);
    json += ",\"motorPosition\":" + String(motorPosition);
    json += ",\"nowStr\":\"" + nowStr() + "\"";
    json += "}";
    server.send(200, "text/json", json);
    json = String();
    //debug("/all : ",true);
    //debug(millis());
  });


  server.on("/debug", HTTP_GET, []() {
    server.send(200, "text/json", debugText );
  });
    
  
  server.on("/motorTarget", HTTP_GET, []() {
    String motorTargetStr = server.arg(0);
    motorTarget = motorTargetStr.toInt();
    debug("motorTarget : ", true);
    debug(motorTarget);

    while (motorTarget != motorPosition) {
      delayWithWebHandle(10);
    opticalLoop();
    }
    StoreEEPROM();

    server.send(200, "text/plain", "OK");
  });


  server.on("/equilibre", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    rechercheEquilibre();
  });

  server.on("/motorCheck", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    rechercheMotorCheck();
  });

  server.on("/RTCSetAlarm", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    RTCSetAlarm();
  });

  server.on("/debugRTCReg", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    debugRTCReg();
  });
    
  server.on("/tare", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    motorPosition = 0;
    motorTarget = 0;
    StoreEEPROM();
  });



  server.on("/SPIFFSFormat", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    SPIFFSFormat();
  });


  server.on("/timeset", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    webTimeSet();
  });

  server.on("/testGSM", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    GsmHttpSend();
  });

  server.on("/sigfoxATCommand", HTTP_GET, []() {
    String ATCommand = server.arg(0);

    debug("sigfoxATCommand : ", true);
    debug(ATCommand, true);


    File logfile = SPIFFS.open("/sigfoxlogmanual.txt", "a");

    sigfoxSendCommand(ATCommand, logfile);
    String ATAnswer = sigfoxGetAnswer(1000, logfile);

    logfile.close();
    debug(" >> ", true);
    debug(ATAnswer);

    server.send(200, "text/plain", ATAnswer);
  });



  server.on("/testGSM", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    GsmHttpSend();
  });


  server.on("/wifiScan", HTTP_GET, []() {
    String htmlOut = "{\"wifi\":[";

    WiFi.scanDelete();
    delay(100);

    int numScan = WiFi.scanNetworks(true);
    while ((numScan = WiFi.scanComplete()) < 0) delay(100);

    for (int i = 0; i < numScan; i++) htmlOut = htmlOut + "\"" + WiFi.SSID(i) + "\",";

    htmlOut = htmlOut.substring(0, htmlOut.length() - 1) + "]}";

    server.send(200, "text/plain", htmlOut);

  });

  server.on("/RTCSetAlarm", HTTP_GET, []() {
    server.send(200, "text/plain", "OK");
    RTCSetAlarm();
  });


  server.on("/wifiSlaveConfig", HTTP_GET, []() {
    File confFile;
    char masterSSID[50];
    unsigned long timeOutStart;
    server.arg(0).toCharArray(masterSSID,50);
  
    /*confFile = SPIFFS.open("/wifiSlaveSSID.txt", "w");
    confFile.println(masterSSID);
    confFile.close();*/

    server.send(200, "text/plain", "OK");
    delay(100);



    File logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
    logfile.println("wifiSlaveConfig call");
    logfile.println(masterSSID);
    logfile.println(WiFi.localIP());
    logfile.println(nowStr());
    
    WiFi.mode(WIFI_STA);
    delay(1000);
    
        
    WiFi.disconnect();
    //delay(1000);
    logfile.println("disconnect 1");
    logfile.println(WiFi.localIP());
    logfile.println(nowStr());
    logfile.close();



    delay(1000);
    WiFi.begin(masterSSID);


    logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
    logfile.println("after begin");
    logfile.println(WiFi.localIP());
    logfile.println(nowStr());
    logfile.close();

    logfile.print(nowStr());
    timeOutStart = millis();
    
    while ((WiFi.status() != WL_CONNECTED) && ((millis() - timeOutStart) < 10000)) {
      delay(500);
      logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
      logfile.print(WiFi.status());
      logfile.close();
    }
    logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
    logfile.println("");
    logfile.println(nowStr());
    logfile.println(WiFi.localIP());
    logfile.close();    

    if (WiFi.status() == WL_CONNECTED) {
      logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
      logfile.println("connected");
      logfile.println(WiFi.localIP());
      logfile.close();
  

      const char* remote_host = "192.168.4.1";
      debug("pinging .... " + String(remote_host));
    
      if(Ping.ping(remote_host)) {
        debug("Success!!");
      } else {
        debug("Error :(");
      }

  
      HTTPClient http;
      String URL = "http://192.168.4.1/wifiSlaveRegister?chipId=" + String(ESP.getChipId());
          
      logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
      logfile.println(URL);
      logfile.close();

      int retryCount = 0;
      int okCnt = 0;
      int ret;
      while (okCnt < 1 && retryCount < 2) {
        logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");

        //http.begin(URL)
        logfile.println(http.begin("192.168.4.1",80,"/wifiSlaveRegister?chipId=" + String(ESP.getChipId())));
        
        ret = http.GET();
        
        logfile.println(ret);
        logfile.close();

        if (ret == 200) {
          okCnt++;
          confFile = SPIFFS.open("/measurementFrequency.txt", "w");
          confFile.print(http.getString());
          confFile.close();
          
          RTCSetAlarm();
        }
        
        http.end();
        retryCount++;
      }

      IPAddress ap(192, 168, 4, 1);
      GetNTPConfigureRTC(ap);


      
    
    } else {
      logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
      logfile.println("not connected");
      logfile.close();
    }


    WiFi.mode(WIFI_AP);
    delay(100);
    WiFi.disconnect();
    delay(100);
    WiFi.softAP(localAPssid);
    IPAddress myIP = WiFi.softAPIP();
    
    logfile = SPIFFS.open("/logWifiSlaveConfig.txt", "a");
    logfile.println("AP IP address: " + String(myIP));
    logfile.close();
    
  });
  
  server.on("/wifiSlaveRegister", HTTP_GET, []() {    
    String chipIdStr = server.arg(0);

    File logfile = SPIFFS.open("/logWifiSlaveRegister.txt", "a");
    logfile.println(nowStr());
    logfile.println(chipIdStr);
    logfile.close();
    
    uint32_t chipId = chipIdStr.toInt();
    uint8_t idx;
    
    //if (SPIFFS.exists("/slaveList.txt")) {
    
    for (idx=0; ((idx<=slaveList.num) && (chipId != slaveList.id[idx])) ; idx++);
    
    if (idx > slaveList.num) {
      File f = SPIFFS.open("/slaveList.txt", "a");
      f.println(server.arg(0));
      idx = slaveList.num;
      slaveList.id[idx] = chipId;
      slaveList.num++;
    } else {
      debug("found : " + String(idx));
    }


    int measurementFrequency = readSetting("measurementFrequency").toInt();
    
    
    server.send(200, "text/plain", String(measurementFrequency));
  });


  //http://192.168.4.1/wifiSlavePost?chipId=13442931&weightRaw=123
  server.on("/wifiSlavePost", HTTP_GET, []() {  
    String slaveIdStr = server.arg(0);    
    String weightRawStr = server.arg(1);
    uint32_t slaveId = slaveIdStr.toInt();
    uint32_t weightRaw = weightRawStr.toInt();
    int8_t idx = -1;

    for (idx=0; idx<slaveList.num && slaveList.id[idx] != slaveId; idx++); //lookup

    if (slaveId == slaveList.id[idx]) { //found?
      slaveList.numReceived++;
      slaveList.received[idx] = true;
      slaveList.weightRaw[idx] = weightRaw;

      server.send(200, "text/plain", String(idx));    
    } else server.send(200, "text/plain", "unknown chip id");    

    
  });

  
  server.begin();
  debug("HTTP server started");
}





void webTimeSet() {
  String epoch = server.arg("epoch");

  time_t t = epoch.toInt();


  /*if (isDst(t))
    t += 7200; // GMT+2
    else
    t += 3600; // GMT+1*/

  setTime(t);
  RTCSetTime();
  RTCSetAlarm();

  server.send(200, "text/html", "timeset ok");
}









////////////////////////////////////////////////
//// Unchanged code from example : ESP8266WebServer/FSBrowser



String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

String getContentType(String filename) {
  if (server.hasArg("download")) return "application/octet-stream";
  else if (filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".xml")) return "text/xml";
  else if (filename.endsWith(".pdf")) return "application/x-pdf";
  else if (filename.endsWith(".zip")) return "application/x-zip";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path) {
  //debug("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {
    if (SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

void handleFileUpload() {
  if (server.uri() != "/edit") return;
  HTTPUpload& upload = server.upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = upload.filename;
    if (!filename.startsWith("/")) filename = "/" + filename;
    debug("handleFileUpload Name: ", true); debug(filename);
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    //debug("handleFileUpload Data: ",true); debug(upload.currentSize);
    if (fsUploadFile)
      fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile)
      fsUploadFile.close();
    //debug("handleFileUpload Size: ", true); debug(upload.totalSize);
  }
}

void handleFileDelete() {
  if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  debug("handleFileDelete: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (!SPIFFS.exists(path))
    return server.send(404, "text/plain", "FileNotFound");
  SPIFFS.remove(path);
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileCreate() {
  if (server.args() == 0)
    return server.send(500, "text/plain", "BAD ARGS");
  String path = server.arg(0);
  debug("handleFileCreate: " + path);
  if (path == "/")
    return server.send(500, "text/plain", "BAD PATH");
  if (SPIFFS.exists(path))
    return server.send(500, "text/plain", "FILE EXISTS");
  File file = SPIFFS.open(path, "w");
  if (file)
    file.close();
  else
    return server.send(500, "text/plain", "CREATE FAILED");
  server.send(200, "text/plain", "");
  path = String();
}

void handleFileList() {
  if (!server.hasArg("dir")) {
    server.send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = server.arg("dir");
  debug("handleFileList: " + path);
  Dir dir = SPIFFS.openDir(path);
  path = String();

  String output = "[";
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") output += ',';
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    output += String(entry.name()).substring(1);
    output += "\"}";
    entry.close();
  }

  output += "]";
  server.send(200, "text/json", output);
}


void delayWithWebHandle(int d) {
  unsigned long tIn, tOut, lastHandle;
  tIn = millis();
  tOut = tIn + d;
  
  server.handleClient();
  lastHandle = millis();

  while (millis() < tOut) {
    if (millis() - lastHandle > 5) {
      server.handleClient();
      lastHandle = millis();
    }
  }
  return;
}

