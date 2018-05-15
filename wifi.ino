#include "wifi.h"


void wifiStart() {
  unsigned long wifiStart;

  if (!wifiStarted) {
    
    wifiStarted = 1;

    wifiStart = millis();
    debug("Connecting");

    //Connexion au wifi
    char ssid[50];
    char pw[50];
    
    String ssidStr = readSetting("wifiHotspotSSID");
    ssidStr.toCharArray(ssid, 50);
    
    String pwStr = readSetting("wifiHotspotPassword");
    pwStr.toCharArray(pw, 50);
          

    WiFi.begin(ssid,pw);
    
    
    debug("Avant wifi start : " + String(WiFi.status()));

    


    while ((WiFi.status() != WL_CONNECTED) && ((millis() - wifiStart) < 20000))
    {
      delay(100);
      debug(String(WiFi.status()), true);
    }
    debug("");

    if (WiFi.status() != WL_CONNECTED) {
      debug("Wifi connection failed.... " + String(WiFi.status()));
      //RTCClearInterrupt(); // => switch OFF

    }

    debug("Fin wifi start : " + String(WiFi.status()));

    File logfile = SPIFFS.open("/wifilog.txt", "a");
    logfile.println("");
    logfile.println(nowStr());
    logfile.println("Fin wifi start : " + String(WiFi.status()));
    logfile.close();


    unsigned long wifiConnected;
    wifiConnected = millis();

    debug(wifiConnected);

  }
}

void wifiStop() {
  if (wifiStarted == 1) {
    // Insert whatever code here to turn off all your web-servers and clients and whatnot
    WiFi.disconnect();
    WiFi.forceSleepBegin();
    delay(1); 
    //For some reason the modem won't go to sleep unless you do a delay(non-zero-number) -- no delay, no sleep and delay(0), no sleep
  }
}
