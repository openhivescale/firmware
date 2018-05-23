#ifndef __memory__
#define __memory__

#include "global.h"
#include <EEPROM.h>
#include <ArduinoJson.h>



// Configuration that we'll store on disk
/*struct Config {
  String wifiSlaveMaster;
  String chipId;
  String sendingMode;
  String wifiHotspotSSID;
  String wifiHotspotPassword;
  String wifiHotspotURL;
  String measurementFrequency;
};

Config config;*/

struct SlaveList {
  uint8_t num;
  uint32_t id[16];
  uint8_t numReceived;
  bool received[16];
  int32_t weightRaw[16];
};

SlaveList slaveList;


    


      
void memoryInit();
void StoreEEPROM();
void ReadEEPROM();
void SPIFFSFormat();

String readSetting(String setting);
void jsonSave();

#endif
