#include "gsm.h"

void GsmHttpSend() {
  debug("Send GSM HTTP.....");
  File logfile = SPIFFS.open("/gsmlog.txt", "a");
  logfile.println("gsmhttpsend");
  
  // URL, path & port (for example: arduino.cc)
  const char server[] = "www.pierrebeck.fr";
  String resource = "/dumbpost.php?text=" + String(ESP.getChipId()) + "_" + String(weightRaw);
  const int port = 80; // port 80 is the default for HTTP

  
  //String apn = 
  char apn[100];
  readSetting("gsmGprsAPN").toCharArray(apn,100);
  String URL = readSetting("gsmGprsURL");
 
 
  URL.replace("{chipID}",String(ESP.getChipId()));
  URL.replace("{weightRaw}",String(weightRaw));

  // Your GPRS credentials
  // Leave empty, if missing user or pass
  //const char apn[]  = "mobiledata";
  //const char user[] = "";
  //const char pass[] = "";

  TinyGsm modem(modemSerial);
  TinyGsmClient client(modem);
  HttpClient http(client, server, port);
  //HttpClient http(URL);
    
  RTCReadReg();

  logfile.print(nowStr() + " : ");
  logfile.println("Initializing modem...");

  modem.init();

  String modemInfo = modem.getModemInfo();
  logfile.println(nowStr());
  logfile.print("Modem: ");
  logfile.println(modemInfo);


  logfile.println(nowStr());
  logfile.print(F("Waiting for network..."));
 
  if (!modem.waitForNetwork()) {
    logfile.println(nowStr());
    logfile.println(" fail");
    delay(10000);
    return;
  }
  logfile.print(nowStr() + ":");  
  logfile.println(" OK");
  
  logfile.println(nowStr());
  logfile.print(F("Connecting to "));
  logfile.print(apn);
  
  //if (!modem.gprsConnect(apn, user, pass)) {
  if (!modem.gprsConnect(apn)) {
    logfile.println(nowStr());
    logfile.println(" fail");
    delay(10000);
    return;
  }
  logfile.println(nowStr());
  logfile.println(" OK");



  logfile.print(nowStr()+" : ");
  logfile.println(F("Performing HTTP GET request... "));

  
  int err = http.get(resource);
  if (err != 0) {
    logfile.println("failed to connect");
    delay(10000);
    return;
  }

  int status = http.responseStatusCode();
  logfile.print(nowStr()+ " : ");
  logfile.println(status);
  if (!status) {
    delay(10000);
    return;
  }

  int length = http.contentLength();
  if (length >= 0) {
    logfile.println(String("Content length is: ") + length);
  }


  String body = http.responseBody();
  logfile.print(nowStr() + ":");
  logfile.println("Response:");
  logfile.println(body);
  logfile.println(String("Body length is: ") + body.length());


  // Shutdown
  http.stop();

  modem.gprsDisconnect();
  logfile.print(nowStr() + " : ");
  logfile.println("GPRS disconnected");

  logfile.close();
}



void GsmSmsSend() {
  TinyGsm modem(modemSerial);
  //TinyGsmClient client(modem);
  
  RTCReadReg();
  File logfile = SPIFFS.open("/smslog.txt", "a");
  
  
  String message = readSetting("gsmSmsMessage");
  message.replace("{chipID}",String(ESP.getChipId()));
  message.replace("{weightRaw}",String(weightRaw));  


  logfile.println("Initializing modem...");
  modem.init();

  String modemInfo = modem.getModemInfo();
  logfile.print("Modem: ");
  logfile.println(modemInfo);


  logfile.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    logfile.println(" fail");
    delay(10000);
    return;
  }
  logfile.println(" OK");
    
  modem.sendSMS(readSetting("gsmSmsPhoneNum"), message);


  

  logfile.close();
  
}

