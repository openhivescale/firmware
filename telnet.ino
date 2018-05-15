#include "telnet.h"


void telnetHandle() {
  //check if there are any new clients
  if (telnetServer.hasClient()) {

    //find free/disconnected spot
    if (!telnetClient || !telnetClient.connected()) {
      if (telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();
      debug("New telnet client");
    }

    //no free/disconnected spot so reject
    WiFiClient telnetClient2 = telnetServer.available();
    telnetClient2.stop();
  }

  //check clients for data
  if (telnetClient && telnetClient.connected()) {
    if (telnetClient.available()) {
      //get data from the telnet client and push it to the UART
      while (telnetClient.available()) modemSerial.write(telnetClient.read());
    }
  }

  //check UART for data
  if (modemSerial.available()) {
    size_t len = modemSerial.available();
    uint8_t sbuf[len];
    modemSerial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    if (telnetClient && telnetClient.connected()) {
      telnetClient.write(sbuf, len);
      delay(1);
    }

  }
}
