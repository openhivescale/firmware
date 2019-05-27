# firmware
mainboard firmware (Arduino/esp8266)

To compile firmware :
Follow these instructions (Boards Manager chapter):
https://github.com/esp8266/Arduino#installing-with-boards-manager

27/05/2019 : Compiled with Arduino 1.8.9 and esp8266 2.5.2


Add libraries : 
* Time (Michael Margolis) (tested with 1.5.0)
* ArduinoHttpClient (tested with 0.3.1)
* TinyGsm (tested with 0.3.5)


Choose Generic ESP8266 module, and !!!VERY IMPORTANT!!! flash size 4M (3M SPIFFS) !!!
(if you miss that, you'll brick the OTA updater, and you'll need UART adapter)

For initial flash (if you make your own board), you'll need a usb-uart adapter, use the RX/TX/GND from the daughter board pins, put a jumper on the flash side of the 2x2 pins block.

After initial flash, simply compile (with flash size 4M(3M) !!), get the .bin, and go through the config portal http://192.168.4.1/update
