#ifndef __memory__
#define __memory__

#include "global.h"
#include <EEPROM.h>

void memoryInit();
void StoreEEPROM();
void ReadEEPROM();
void SPIFFSFormat();
String readSetting(String setting);

#endif
