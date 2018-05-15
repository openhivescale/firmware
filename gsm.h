#ifndef __gsm__
#define __gsm__

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

// Increase RX buffer
#define TINY_GSM_RX_BUFFER 512


//#define DUMP_AT_COMMANDS
//#define TINY_GSM_DEBUG Serial
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

void GsmHttpSend();

#endif
