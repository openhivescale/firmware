#ifndef __modem__
#define __modem__



void modemSendCommand(String command, File logfile);
String modemGetAnswer(int timeout, File logfile);
String sigfoxSend();

#endif
