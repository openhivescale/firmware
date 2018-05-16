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
  }

}




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
  File f = SPIFFS.open("/" + setting + ".txt", "r");
  if (f && f.size()) {
      String t = f.readString();
      debug("read : " + t);
      return t;
  }
  f.close();
}


