#!/bin/bash

apt-get update
apt-get install -y make git python2.7

mkdir /esp
cd /esp
git clone https://github.com/esp8266/Arduino.git
git clone https://github.com/sui77/rc-switch.git
git clone https://github.com/me-no-dev/ESPAsyncUDP.git
git clone https://github.com/me-no-dev/ESPAsyncWebServer.git
git clone https://github.com/Monarch73/ESPAsyncTCP.git
git clone https://github.com/Monarch73/RFBridge.git
git clone https://github.com/plerup/makeEspArduino.git

cd Arduino/tools
python2.7 ./get.py

make -f /esp/makeEspArduino/makeEspArduino.mk ESP_ROOT=/esp/Arduino/ BOARD=d1_mini SKETCH=/esp/RFBridge/RFBridge/RFBridge/RFBridge.ino LIBS="/esp/Arduino/./libraries/ESP8266WiFi/src/ /esp/Arduino/./libraries/EEPROM/ /esp/ESPAsyncTCP/src/ /esp/ESPAsyncUDP/src/ /esp/ESPAsyncWebServer/src/ /esp/Arduino/libraries/Hash/src/ /esp/rc-switch/ /esp/Arduino/libraries/ESP8266mDNS/ /esp/Arduino/libraries/"
