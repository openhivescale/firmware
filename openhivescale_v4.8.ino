//53, 861, 964, 965, 975, 980, 1016, 1079

//for file in `ls -A1`; do curl -F "file=@$PWD/$file" 192.168.4.1/edit; done

//#define serialDebug

#include "global.h"
//#include "wifi.h"
#include "debug.h"
#include "telnet.h"
#include "_time.h"
#include "memory.h"
#include "scale.h"
#include "webConfig.h"

#include "gsm.h"
#include "sigfox.h"
#include "wifi.h"

#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>



const char compile_date[] = "Compile time : " __DATE__ " " __TIME__;
const char* host = "openhivescale";



void setup(void) {
  //wakeUpPeriod = 1;
  
  #ifdef serialDebug
    delay(2000);
    Serial.begin(9600);
    Serial.println("Boot...");
    delay(500);
  #endif
  
  //motor control pins initialization
  scaleInit();
  
  #ifdef serialDebug
    Serial.println("Scale initiated...");
    delay(500);
  #endif

  
  //Ensure wifi is disconnected while we don't need it
  WiFi.disconnect();
   
  #ifdef serialDebug
    Serial.println("Wifi disconnected...");
    delay(500);
  #endif



  memoryInit();
    
  //Hardware serial is connected to modem daughter board
  modemSerial.begin(9600);
  
  /*pinMode(1,OUTPUT);
  digitalWrite(1,LOW);
  delay(10);
  modemSerial.begin(57600);
  modemSerial.println((char)0x55);*/
  
  //modemSerial.begin(74880);

  timeInit();

  #ifdef serialDebug
    Serial.println("Time initiated...");
    delay(500);
  #endif
    
  debug("hello " + nowStr());
  debug(compile_date);

  #ifdef serialDebug
    Serial.println("hello " + nowStr());
  #endif


  

  if (wakeUpByRTCAlarm) {
    #ifdef serialDebug
      Serial.println("wakeUpByRTCAlarm...");
    #endif
    
    debug("timer interrupt");

    debug(String(millis()));


    if (ESP.getChipId() == 13442931)
      pesee = 12;
    else
       pesee = rechercheEquilibre();
       
    debug("fin équilibre : ", true);
    debug(pesee);
    debug(String(millis()));

    //motorTarget = 0;
    while (motorTarget != motorPosition) delay(10);
    StoreEEPROM();
    ReadEEPROM();
    //pesee = millis();

    String sendingMode = readSetting("sendingMode");
    
    if (sendingMode == "sigfox") {
      String sigfoxResult = sigfoxSend();
    }

    
    if (sendingMode == "wifiHotspot") {
          HTTPClient http;
          wifiStart();

          String URL = readSetting("wifiHotspotURL");
          URL.replace("{chipID}",String(ESP.getChipId()));
          URL.replace("{weightRaw}",String(pesee));
    
          if (WiFi.status() == WL_CONNECTED) {
            int retryCount = 0;
            int okCnt = 0;
            int ret;
            
            while (okCnt < 1 && retryCount < 10) {
              //http.begin("www.pierrebeck.fr", 80, "/dumbpost.php?text=openhivescale_" + String(ESP.getChipId()) + "_weight_" + String(pesee)); //HTTP
              http.begin(URL);
              ret = http.GET();
              http.end();
              debug(ret);
      
              if (ret == 200) okCnt++;
              retryCount++;
            }      
            File logfile = SPIFFS.open("/httplog.txt", "a");
            logfile.println("");
            logfile.println(nowStr());
     
            logfile.println("HTTP post : " + String(ret));
            logfile.close();
            wifiStop();
          }
    }
    
    

    /*if (WiFi.status() == WL_CONNECTED) {
      retryCount = 0;
      okCnt = 0;
      while (okCnt < 1 && retryCount < 10) {
        RTCReadReg();
        String timePcf = String(bcd2int(hours)) + ":" + String(bcd2int(minutes)) + ":" + String(bcd2int(seconds));

        http.begin("www.pierrebeck.fr", 80, "/dumbpost.php?text=Balance_45_" + timePcf + "_Sigfoxreturn_" + sigfoxResult); //HTTP
        ret = http.GET();
        http.end();
        debug(ret);

        if (ret == 200) okCnt++;
        retryCount++;
      }
      wifiStop();
      }*/

    
    /*GsmHttpSend();


    debug("Switch off....  " + nowStr());
    ReadEEPROM();

    File logfile = SPIFFS.open("/pesee_automatique.txt", "a");
    logfile.println(debugText + "\r\n\r\n");
    logfile.close();
    delay(500);*/



    /*WiFi.mode(WIFI_STA);
    File logfile;
    File confFile = SPIFFS.open("/wifiMaster.txt", "r");

    char ssid[50];
    String ssidStr = confFile.readStringUntil('\n');
    ssidStr = ssidStr.substring(0,ssidStr.length() - 1);
    ssidStr.toCharArray(ssid, 50);

    
    confFile.close();

    delay(1000);
    WiFi.begin(ssid);
    
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      logfile = SPIFFS.open("/wifiNTP.txt", "a");
      logfile.println(".");
      logfile.println(nowStr());
      logfile.close();
    }


    IPAddress apclient(192, 168, 4, 1);
    GetNTPConfigureRTC(apclient);*/
    

    delay(500);
    RTCSetAlarm();
    RTCClearAlarmInterrupt(); // => switch OFF
  }


  /*====================================================================*/
  //wifiStart();

  #ifdef serialDebug
    Serial.println("start soft wifi");
  #endif


  char ssid[100];
  sprintf(ssid, "openhivescale_%d", ESP.getChipId());

  WiFi.softAP(ssid);

  IPAddress myIP = WiFi.softAPIP();

  #ifdef serialDebug
    Serial.println("AP IP address: ");
    Serial.println(myIP);
  #endif

  debug("AP IP address: ");
  debug(myIP);





  //telnet
  telnetServer.begin();
  telnetServer.setNoDelay(true);



  webConfigInit();

  ntpServerInit();
}



int delayedAfterBoot = 1;

void loop(void) {

  if ((millis() > 10000) && delayedAfterBoot) {
    File logfile = SPIFFS.open("/boot10sec.txt", "a");
    logfile.print("boot...");
    logfile.println(nowStr());
    logfile.close();
    delayedAfterBoot = 0;
    
    /*WiFi.mode(WIFI_AP_STA);
    

    
    File confFile = SPIFFS.open("/wifiMaster.txt", "r");

    char ssid[50];
    String ssidStr = confFile.readStringUntil('\n');
    ssidStr = ssidStr.substring(0,ssidStr.length() - 1);
    ssidStr.toCharArray(ssid, 50);

    
    confFile.close();

    delay(1000);
    WiFi.begin(ssid);*/
  }


  //ArduinoOTA.handle();

  server.handleClient();

  telnetHandle();

  ntpServerProcess();

  if ((millis() - lastOpticalLoop) > 50) {
    lastOpticalLoop = millis();
    opticalLoop();
    //debug(v0);
  }


  if ((millis() - lastReadReg) > 1000) {
    lastReadReg = millis();



    RTCReadReg();
    if (RTCReg[1] & 0b01001000) {
      debug("timer interrupt");
      RTCSetAlarm();
      RTCClearAlarmInterrupt(); // => switch OFF
    }

    //debug("Wifi status : " + String(WiFi.status()));
    //debug(ESP.getFreeHeap());
    RTCReadTime();


  }
}
