#ifndef ___time__
#define ___time__

#include "global.h"
#include <WiFiUdp.h>
#include <Wire.h>
#include <TimeLib.h>


#define RTCsda 4
#define RTCscl 5

byte RTCReg[20];

#define debugRTCReg 0


//IPAddress timeServerIP(192,168,4,2);
const char* ntpServerName = "0.pool.ntp.org";

WiFiUDP udpNtpClient;
unsigned int udpNtpClientPort = 2390;      // local port to listen for UDP packets

WiFiUDP udpNtpServer;


// Time Server Port
#define NTP_PORT 123
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets


void timeInit();
void RTCReset();
void RTCConfigureTimer();
void RTCSetTime();
void RTCReadTime();
void RTCClearInterrupt() ;
void RTCReadReg();
unsigned long sendNTPpacket(IPAddress& address, WiFiUDP udp);
void GetNTPConfigureRTC(IPAddress& address);
void ntpServerInit();
void ntpServerProcess();

String nowStr();

#endif
