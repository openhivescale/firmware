#ifndef __global__
#define __global__


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <FS.h>

#define modemSerial Serial


bool wakeUpByRTCAlarm, wakeUpForConfig;

//byte wakeUpPeriod; //minutes
byte sigfoxSendEnable;




#endif
