# firmware
mainboard firmware (Arduino/esp8266)

To compile firmware :
Follow these instructions (git version chapter):
https://github.com/esp8266/Arduino#using-git-version

28/01/2019 : I can't compile with most recent version of arduino, and/or last version of esp8266 board extension.
Here is a working set :
Arduino 1.8.2 (1.8.8 doesn't work)
This commit of board extension (originally downloaded on 5/5/2018)
https://github.com/esp8266/Arduino/archive/bd87970aaec8c10ab6fb47e088360bcc9db83be3.zip

Add libraries : 
  Time (Michael Margolis) (tested with 1.5.0)
  ArduinoHttpClient (tested with 0.3.1)
  TinyGsm (tested with 0.3.5)


Choose Generic ESP8266 module, and !!!VERY IMPORTANT!!! flash size 4M (3M SPIFFS) !!!
(if you miss that, you'll brick the OTA updater, and you'll need UART adapter)

For initial flash (if you make your own board), you'll need a usb-uart adapter, use the RX/TX/GND from the daughter board pins, put a jumper on the flash side of the 2x2 pins block.

After initial flash, simply compile (with flash size 4M(3M) !!), get the .bin, and go through the config portal http://192.168.4.1/update

