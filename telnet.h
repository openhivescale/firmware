#ifndef __telnet__
#define __telnet__

#include "global.h"
#include "modem.h"


WiFiServer telnetServer(23);
WiFiClient telnetClient;

static void telnetHandle();


#endif
