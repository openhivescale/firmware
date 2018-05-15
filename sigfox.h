#ifndef __sigfox__
#define __sigfox__



void sigfoxSendCommand(String command, File logfile);
String sigfoxGetAnswer(int timeout, File logfile);
String sigfoxSend();

#endif
