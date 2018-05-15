#ifndef __wifi__
#define __wifi__



/*IPAddress staticIP(192, 168, 0, 47);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);
const byte bssid[] = {0xde, 0xf3, 0x49, 0xaa, 0x53, 0x68};*/


int wifiStarted = 0;
void wifiStart();
void wifiStop();

#endif
