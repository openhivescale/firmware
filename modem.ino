/*=======================================================
  =================   $SIGFOX   ===========================
  =========================================================*/
void modemSendCommand(String command, File logfile) {
  //logfile.print(millis());
  logfile.print(nowStr() + " (" + String(millis()) + ") : ");
  
  logfile.print(" >> ");
  logfile.println(command);



  modemSerial.flush();
  modemSerial.println(command);

}

String modemGetAnswer(int timeout, File logfile) {
  char cr = '\r';
  char lf = '\n';
  String ret;
  unsigned long timeoutAbs;
  timeoutAbs = millis() + timeout;
  modemSerial.setTimeout(timeout);


  //ret = modemSerial.readStringUntil(lf);
  ret = modemSerial.readStringUntil(cr);
  modemSerial.readStringUntil(lf);


  logfile.print(nowStr() + " (" + String(millis()) + ") : ");
  //logfile.print(millis()nowStr());

  //logfile.print(" overflow ");
  //logfile.print(modemSerial.overflow());

  logfile.print(" << ");
  logfile.print(ret.length());
  logfile.print(" : ");
  logfile.println(ret);

  /*Serial.readStringUntil(lf);
    Serial.print("//");
    Serial.print(millis());
    Serial.print(" : ");
    Serial.print(ret.length());
    Serial.print(" ");
    Serial.println(ret);*/

  return ret;
}


String sigfoxSend() {
  String result = "";


  RTCReadReg();
  File logfile = SPIFFS.open("/sigfoxlog.txt", "a");

  logfile.println("");
  logfile.println(nowStr());

  debug("Send sigfox.....");
  while (modemSerial.available()) modemSerial.read();
  String ret;

  for (int i = 0; i < 5 && ret != "OK"; i++) {
    modemSendCommand("AT", logfile);
    ret = modemGetAnswer(1000, logfile);
  }

  if (ret == "OK") {

    /*
      modemSendCommand("ATE0", logfile);
      modemGetAnswer(1000, logfile);
      modemSendCommand("ATV1", logfile);
      modemGetAnswer(1000, logfile);
      modemSendCommand("ATQ0", logfile);
      modemGetAnswer(1000, logfile);
    */


    /*modemSendCommand("ATI26", logfile);
      String tempStr = modemGetAnswer(1000, logfile);
      int temp = tempStr.toInt();*/

    modemSendCommand("AT$T?", logfile);
    String tempStr = modemGetAnswer(1000, logfile);
    float temp = tempStr.toInt() / 10.0;


    debug("temp = " + String(temp));
    modemGetAnswer(1000, logfile);


    String msgPesee = "0000" + String((unsigned long)weightRaw, HEX);
    String msgTemp = "00" + String((unsigned byte)temp, HEX);
    String msg = msgTemp.substring(msgTemp.length() - 2, msgTemp.length()) + "00" + msgPesee.substring(msgPesee.length() - 4, msgPesee.length());

    for (int i=0; i<slaveList.num && i<4;i++) {
      if (slaveList.received[i])
        msgPesee = "0000" + String((unsigned long)slaveList.weightRaw[i], HEX);
      else
        msgPesee = "0000";
        
      msg = msg + msgPesee.substring(msgPesee.length() - 4, msgPesee.length());
    }

    /*if (ESP.getChipId() == 13442931 || ESP.getChipId() == 13441947) {
      logfile.println("");
      logfile.println("Simulation");
      logfile.println(nowStr());
      logfile.println(msg);
    } else {*/
      
      //modemSendCommand("AT$SS=" + msg, logfile);
      modemSendCommand("AT$SF=" + msg, logfile);
      result = modemGetAnswer(10000, logfile);
    /*modemSendCommand("ATI28", logfile);
      result = result + "_ATI28_" + modemGetAnswer(1000, logfile);*/
      modemSendCommand("AT$V?", logfile);
      result = result + "_AT$V?_" + modemGetAnswer(1000, logfile);
  
      modemGetAnswer(100, logfile);
      modemGetAnswer(100, logfile);
      modemGetAnswer(100, logfile);
      modemGetAnswer(100, logfile);      
    //}


  }

  logfile.close();
  
  return result;
}






String loraSend() {
  String result = "";


  RTCReadReg();
  File logfile = SPIFFS.open("/loralog.txt", "a");

  logfile.println("");
  logfile.println(nowStr());

  debug("Send lora.....");
  while (modemSerial.available()) modemSerial.read();
  String ret;

  for (int i = 0; i < 5 && !ret.startsWith("RN2483"); i++) {
    modemSendCommand("sys get ver", logfile);
    ret = modemGetAnswer(1000, logfile);
  }

  if (ret.startsWith("RN2483")) {

    String msgPesee = "0000" + String((unsigned long)weightRaw, HEX);
    //String msgTemp = "00" + String((unsigned byte)temp, HEX);
    String msg = /*msgTemp.substring(msgTemp.length() - 2, msgTemp.length()) + "00"*/ "0000" + msgPesee.substring(msgPesee.length() - 4, msgPesee.length());

    for (int i=0; i<slaveList.num && i<4;i++) {
      if (slaveList.received[i])
        msgPesee = "0000" + String((unsigned long)slaveList.weightRaw[i], HEX);
      else
        msgPesee = "0000";
        
      msg = msg + msgPesee.substring(msgPesee.length() - 4, msgPesee.length());
    }

    modemSendCommand("mac set appeui 0004A30B00209F37", logfile);
    result = modemGetAnswer(500, logfile);
    modemSendCommand("mac set appkey 00112233445566778899aabbccddeeff", logfile);
    result = modemGetAnswer(500, logfile);
    modemSendCommand("mac join otaa", logfile);
    result = modemGetAnswer(1000, logfile);
    result = modemGetAnswer(20000, logfile);

    if (result == "accepted") {
      modemSendCommand("mac tx uncnf 1 " + msg, logfile);
      ret = modemGetAnswer(1000, logfile);
      ret = modemGetAnswer(10000, logfile);
    }
  
    modemGetAnswer(100, logfile);
    modemGetAnswer(100, logfile);
    modemGetAnswer(100, logfile);
    modemGetAnswer(100, logfile);      
    


  }

  logfile.close();
  
  return result;
}
