//53, 861, 964, 965, 975, 980, 1016, 1079

//-obsolete- for file in `ls -A1`; do curl -F "file=@$PWD/$file" 192.168.4.1/edit; done
// for file in `ls -A1`; do xxd -i $file ../webEmbed_$file.h; sed -i -- 's/unsigned char/const char/g; s/\[\] =/\[\] PROGMEM =/g' ../webEmbed_$file.h; done
// git add -u && git commit -m "embedded html/png files, curl upload no more needed" && git push

///home/pbk/Téléchargements/arduino-1.8.2/hardware/esp8266com/esp8266/tools/esptool/esptool -vv -cd ck -cb 230400 -cp /dev/ttyUSB0 -ca 0x00000 -cf ~/Documents/Arduino/openhivescale_v4.8/openhivescale_v4.8.ino.bin


//erase flash and wifi settings
///home/pbk/Téléchargements/arduino-1.8.2/hardware/esp8266com/esp8266/tools/esptool/esptool -vv -cd ck -cb 115200 -cp /dev/ttyUSB0 -ca 0x0 -cz 0x400000 -ca 0x00000 -cf /tmp/arduino_build_811978/openhivescale_v4.8.ino.bin



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
#include "modem.h"
#include "wifi.h"


#include <ESP8266HTTPClient.h>



const char compile_date[] = "Compile time : " __DATE__ " " __TIME__;
const char firmVer[] = "1.02";

//const char* host = "openhivescale";


void setup(void) {
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

  String sendingMode = "";
  debug("serial open " + String(SPIFFS.exists("/sendingMode.txt")));
  if (SPIFFS.exists("/sendingMode.txt")) {
    sendingMode = readSetting("sendingMode");    
    debug(sendingMode);
    //Hardware serial is connected to modem daughter board
    if (sendingMode == "lora") {
      debug("open serial at 57600");  
      pinMode(1,OUTPUT);
      digitalWrite(1,LOW);
      delay(10);
      modemSerial.begin(57600);
      modemSerial.println((char)0x55);
    }
    
    if (sendingMode == "sigfox" || sendingMode == "gsmGprs" || sendingMode == "gsmSms") {
      debug("open serial at 9600");
      modemSerial.begin(9600);    
    }

  }
  

  timeInit();

  #ifdef serialDebug
    Serial.println("Time initiated...");
    delay(500);
  #endif
    
  debug("hello " + nowStr());
  debug(compile_date);
  debug(firmVer);
  
  #ifdef serialDebug
    Serial.println("hello " + nowStr());
  #endif


  File bootlog = SPIFFS.open("/bootlog.txt", "a");
  bootlog.println("");
  bootlog.println("boot...");
  bootlog.println(nowStr());
  bootlog.println("wakeUpByRTCAlarm : " + String(wakeUpByRTCAlarm));
 


  if (wakeUpByRTCAlarm) {
    #ifdef serialDebug
      Serial.println("wakeUpByRTCAlarm...");
    #endif
    
    //debug("timer interrupt");

    //debug(String(millis()));


    if (ESP.getChipId() == 13442931 || ESP.getChipId() == 13441947 || ESP.getChipId() == 15060469 || ESP.getChipId() == 12778880 || ESP.getChipId() == 12777014)
      weightRaw = millis();
    else
      weightRaw = rechercheEquilibre();
       
    debug("fin équilibre : ", true);
    debug(weightRaw);
    debug(String(millis()));

    //motorTarget = 0;
    while (motorTarget != motorPosition) delay(10);
    StoreEEPROM();
    ReadEEPROM();
    //weightRaw = millis();

    
    bootlog.println("sendingMode : " + String(sendingMode));

    
    
    if (sendingMode == "sigfox" || sendingMode == "lora" || sendingMode == "gsmGprs" || sendingMode == "gsmSms") {
      unsigned long timeOutStart = millis();

      if (slaveList.num > 0) {
        wifiStartAP();
        webConfigInit();
        ntpServerInit();
      }
      
      while ((slaveList.numReceived < slaveList.num) && ((millis() - timeOutStart) < 30000)) {
          server.handleClient();
          ntpServerProcess();

          bootlog.println("master waiting " + nowStr() + " received : " + String(slaveList.numReceived));
          delay(500);
          
      }


      if (sendingMode == "sigfox") {
        bootlog.println("sigfox send");
        String sigfoxResult = sigfoxSend();
      }


      if (sendingMode == "lora") {
        bootlog.println("lora send");
        String sigfoxResult = loraSend();
      }
      
      if (sendingMode == "gsmGprs") {
        bootlog.println("gprs send");
        bootlog.println("heap : " + String(ESP.getFreeHeap()));
        GsmHttpSend();
      }
      
      if (sendingMode == "gsmSms") {
        bootlog.println("sms send");
        GsmSmsSend();
      }
      
    }

    
    if (sendingMode == "wifiHotspot") {
          // http://www.openhivescale.org/monitor/index.php?r=post-measure%2Findex&weight_raw={weightRaw}&esp_id={chipID}
          HTTPClient http;
          wifiStart();

          String URL = readSetting("wifiHotspotURL");
          URL.replace("{chipID}",String(ESP.getChipId()));
          URL.replace("{weightRaw}",String(weightRaw));
    
          if (WiFi.status() == WL_CONNECTED) {
            int retryCount = 0;
            int okCnt = 0;
            int ret;
            
            while (okCnt < 1 && retryCount < 10) {
              //http.begin("www.pierrebeck.fr", 80, "/dumbpost.php?text=openhivescale_" + String(ESP.getChipId()) + "_weight_" + String(weightRaw)); //HTTP
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

    if (sendingMode == "wifiSlave") {
      File confFile;
      char masterSSID[50];
      unsigned long timeOutStart;
      readSetting("wifiSlaveSSID").toCharArray(masterSSID,50);
   
      WiFi.mode(WIFI_STA);
      WiFi.disconnect();
      WiFi.begin(masterSSID);

      timeOutStart = millis();
    
      while ((WiFi.status() != WL_CONNECTED) && ((millis() - timeOutStart) < 30000)) {
        delay(500);
        bootlog.println("slave wifi connecting..." + nowStr());
      }

      if (WiFi.status() == WL_CONNECTED) {
        bootlog.println("slave wifi NTP... " + nowStr());
        IPAddress ap(192, 168, 4, 1);
        GetNTPConfigureRTC(ap);
        bootlog.println("slave wifi NTP. " + nowStr());

          
        HTTPClient http;
        String URL = "http://192.168.4.1/wifiSlavePost?chipId=" + String(ESP.getChipId()) + "&weightRaw=" + String(weightRaw);
            
        int retryCount = 0;
        int okCnt = 0;
        int ret;
        while (okCnt < 1 && ((millis() - timeOutStart) < 30000)) {
          bootlog.println("slave wifi http... " + nowStr());
          http.begin(URL);
          ret = http.GET();
          if (ret == 200) {
            bootlog.println("slave wifi http 200 " + nowStr());
            okCnt++;
          }
          http.end();
          delay(1000);
        }

      }
    }
    
    
    delay(500);
    
    RTCSetAlarm();

    bootlog.println("switch off " + nowStr());
    bootlog.close();
    
    RTCClearAlarmInterrupt(); // => switch OFF
    
  } //else {
  
    wifiStartAP();
    //telnet
    telnetServer.begin();
    telnetServer.setNoDelay(true);

    webConfigInit();

    ntpServerInit();
   //}
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
