# firmware
mainboard firmware (Arduino/esp8266)

To compile firmware : 
Follow these instructions (arduino 1.8.2+git version):
https://github.com/esp8266/Arduino#using-git-version

Add libraries : 
Time (Michael Margolis)
ArduinoHttpClient
TinyGsm

Choose Generic ESP8266 module, and !!!VERY IMPORTANT!!! flash size 4M (3M SPIFFS) !!!
(if you miss that, you'll brick the OTA updater, and you'll need UART adapter)

For initial flash (if you make your own board), you'll need a usb-uart adapter, use the RX/TX/GND from the daughter board pins, put a jumper on the flash side of the 2x2 pins block.

After initial flash, simply compile (with flash size 4M(3M) !!), get the .bin, and go through the config portal http://192.168.4.1/update

Push the files on the web server present in the data subdirectory
under linux : 
for file in `ls -A1`; do curl -F "file=@$PWD/$file" 192.168.4.1/edit; done
