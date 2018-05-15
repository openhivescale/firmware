/*=======================================================
  =================   $SIGFOX   ===========================
  =========================================================*/
void sigfoxSendCommand(String command, File logfile) {
  logfile.print(millis());
  logfile.print(" >> ");
  logfile.println(command);



  modemSerial.flush();
  modemSerial.println(command);

}

String sigfoxGetAnswer(int timeout, File logfile) {
  char cr = '\r';
  char lf = '\n';
  String ret;
  unsigned long timeoutAbs;
  timeoutAbs = millis() + timeout;
  modemSerial.setTimeout(timeout);


  //ret = modemSerial.readStringUntil(lf);
  ret = modemSerial.readStringUntil(cr);
  modemSerial.readStringUntil(lf);


  logfile.print(millis());

  logfile.print(" overflow ");
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
    sigfoxSendCommand("AT", logfile);
    ret = sigfoxGetAnswer(1000, logfile);
  }

  if (ret == "OK") {

    /*
      sigfoxSendCommand("ATE0", logfile);
      sigfoxGetAnswer(1000, logfile);
      sigfoxSendCommand("ATV1", logfile);
      sigfoxGetAnswer(1000, logfile);
      sigfoxSendCommand("ATQ0", logfile);
      sigfoxGetAnswer(1000, logfile);
    */


    /*sigfoxSendCommand("ATI26", logfile);
      String tempStr = sigfoxGetAnswer(1000, logfile);
      int temp = tempStr.toInt();*/

    sigfoxSendCommand("AT$T?", logfile);
    String tempStr = sigfoxGetAnswer(1000, logfile);
    float temp = tempStr.toInt() / 10.0;


    debug("temp = " + String(temp));
    sigfoxGetAnswer(1000, logfile);


    String msgPesee = "0000" + String((unsigned long)pesee, HEX);
    String msgTemp = "00" + String((unsigned byte)temp, HEX);
    String msg = msgPesee.substring(msgPesee.length() - 4, msgPesee.length()) + msgTemp.substring(msgTemp.length() - 2, msgTemp.length());

    //sigfoxSendCommand("AT$SS=" + msg, logfile);
    sigfoxSendCommand("AT$SF=" + msg, logfile);
    result = sigfoxGetAnswer(10000, logfile);

    /*sigfoxSendCommand("ATI28", logfile);
      result = result + "_ATI28_" + sigfoxGetAnswer(1000, logfile);*/
    sigfoxSendCommand("AT$V?", logfile);
    result = result + "_AT$V?_" + sigfoxGetAnswer(1000, logfile);

    sigfoxGetAnswer(100, logfile);
    sigfoxGetAnswer(100, logfile);
    sigfoxGetAnswer(100, logfile);
    sigfoxGetAnswer(100, logfile);
  }

  logfile.close();
  
  return result;
}

