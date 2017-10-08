#!/bin/bash
rm -rf /tmp/mkESP
make -f /esp/makeEspArduino/makeEspArduino.mk ESP_ROOT=/esp/Arduino/ BOARD=d1_mini SKETCH=/esp/RFBridge/RFBridge/RFBridge/RFBridge.ino LIBS="/esp/Arduino/./libraries/ESP8266WiFi/src/ /esp/Arduino/./libraries/EEPROM/ /esp/ESPAsyncTCP/src/ /esp/ESPAsyncUDP/src/ /esp/ESPAsyncWebServer/src/ /esp/Arduino/libraries/Hash/src/ /esp/rc-switch/ /esp/Arduino/libraries/ESP8266mDNS/ /esp/Arduino/libraries/"
