#include "memory.h"
#include "scale.h"



void memoryInit() {
  
  
  EEPROM.begin(9);
  ReadEEPROM();

  bool spiffsBegin = SPIFFS.begin();
  
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    //debug(String(fileName.c_str()) + " : " + String(formatBytes(fileSize).c_str()));
  }


  if (!SPIFFS.exists("/chipId.txt")) {
    File f = SPIFFS.open("/chipId.txt", "w");
    f.print(String(ESP.getChipId()));
    f.close();

    ESP.eraseConfig();
    //ESP.restart();
  }


  slaveList.num = 0;
  if (SPIFFS.exists("/slaveList.txt")) {
    File f = SPIFFS.open("/slaveList.txt", "r");
    while (f.available()) {
      slaveList.id[slaveList.num++] = f.parseInt();
      debug(f.readStringUntil(10));//pass separator
      
    }

    for (int i=0;i<slaveList.num;i++) debug("slaveList[" + String(i) + "]=" + String(slaveList.id[i]));
  }

}

/*void jsonLoad() {
  if (SPIFFS.exists("/config.json")) {
    File jsonFile = SPIFFS.open("/config.json", "r");
    
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& jsonRoot = jsonBuffer.parseObject(jsonFile);
    jsonFile.close();

    config.wifiSlaveMaster = jsonRoot["wifiSlaveMaster"];
    config.chipId = jsonRoot["chipId"];
    config.sendingMode = jsonRoot["sendingMode"];
    config.wifiHotspotSSID = jsonRoot["wifiHotspotSSID"];
    config.wifiHotspotPassword = jsonRoot["wifiHotspotPassword"];
    config.wifiHotspotURL = jsonRoot["wifiHotspotURL"];
    config.measurementFrequency = jsonRoot["measurementFrequency"];
    

  } else {
    config.wifiSlaveMaster = "";
    config.chipId = String(ESP.getChipId());
    config.sendingMode = "wifiHotspot";
    config.wifiHotspotSSID = "";
    config.wifiHotspotPassword = "";
    config.wifiHotspotURL = "";
    config.measurementFrequency = "24";

    jsonSave();

  }
  
}*/


/*void jsonSave() {
  SPIFFS.remove("/config.json");
  File jsonFile = SPIFFS.open("/slaves.json", "w");
  StaticJsonBuffer<512> jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();
  
  jsonRoot["wifiSlaveMaster"] = config.wifiSlaveMaster ;
  jsonRoot["chipId"] = config.chipId ;
  jsonRoot["sendingMode"] = config.sendingMode ;
  jsonRoot["wifiHotspotSSID"] = config.wifiHotspotSSID ;
  jsonRoot["wifiHotspotPassword"] = config.wifiHotspotPassword ;
  jsonRoot["wifiHotspotURL"] = config.wifiHotspotURL ;
  jsonRoot["measurementFrequency"] = config.measurementFrequency ;
  
  
  jsonRoot.printTo(jsonFile);
  jsonFile.close();
}*/

void StoreEEPROM() {
  for (int i = 0; i < 4; i++)
    EEPROM.write(i + 2, ((byte*)&motorPosition)[i]);

  debug("EEPROM Write : stepIdx = " + String(stepIdx));
  EEPROM.write(6, stepIdx);

  //EEPROM.write(7, wakeUpPeriod);
  //EEPROM.write(8, sigfoxSendEnable);

  EEPROM.commit();
}

void ReadEEPROM() {
  for (int i = 0; i < 4; i++)
    ((byte*)&motorPosition)[i] = EEPROM.read(i + 2);

  stepIdx = EEPROM.read(6);
  debug("EEPROM Read : motorPosition = " + String(motorPosition) + ", stepIdx = " + String(stepIdx));

  //wakeUpPeriod = EEPROM.read(7);
  //sigfoxSendEnable = EEPROM.read(8);

  motorTarget = motorPosition;
}

void SPIFFSFormat() {
  SPIFFS.format();
}

String readSetting(String setting) {
  File f;
  
  if (!SPIFFS.exists("/" + setting + ".txt")) {
    String defaultSetting = "";
        
    if (setting == "measurementFrequency") defaultSetting = "1440";
    if (setting == "sendingMode") defaultSetting = "wifiHotspot";
    if (setting == "wifiHotspotURL") defaultSetting = "http://www.openhivescale.org/monitor/index.php?r=post-measure%2Findex&weight_raw={weightRaw}&esp_id={chipID}";
    if (setting == "gsmGprsURL") defaultSetting = "http://www.openhivescale.org/monitor/index.php?r=post-measure%2Findex&weight_raw={weightRaw}&esp_id={chipID}";    

    f = SPIFFS.open("/" + setting + ".txt", "w");
    f.print(defaultSetting);
    f.close();

    return defaultSetting;
  }
  else
  {
    f = SPIFFS.open("/" + setting + ".txt", "r");
    f.setTimeout(1);
    if (f) {
        String t;
        if (f.size()) {
          t = f.readString();
        } else {
          t = "";  
        }
       
        return t;
    }
    f.close();
  }
}


