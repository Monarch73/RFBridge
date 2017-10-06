# RFBridge
This Project uses an esp8266 board as a hub to control RF-Switches via Amazon Echo (Alexa) by emulating Wemos Network Devices 

Its basically a more stable reimplementation, enhanced version of fauxmoESP by https://bitbucket.org/xoseperez/

work in progress....

make -f /esp8266/makeEspArduino/makeEspArduino.mk ESP_ROOT=/esp8266/Arduino/ BOARD=d1_mini SKETCH=/esp8266/RFBridge/RFBridge/RFBridge/RFBridge.ino LIBS="/esp8266/Arduino/./libraries/ESP8266WiFi/src/ /esp8266/Arduino/./libraries/EEPROM/ /esp826
6/ESPAsyncTCP/src/ /esp8266/ESPAsyncUDP/src/ /esp8266/ESPAsyncWebServer/src/ /esp8266/Arduino/libraries/Hash/src/ /esp8266/rc-switch/ /esp8266/Arduino/libraries/ESP8266mDNS/ /esp8266/Arduino/libraries/"
