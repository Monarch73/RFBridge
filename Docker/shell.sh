#!/bin/bash

apt-get update
apt-get upgrade
apt-get install -y make git python2.7
apt-get clean

mkdir /esp
cd /esp
git clone --recursive https://github.com/Monarch73/RFBridge.git
git clone https://github.com/plerup/makeEspArduino.git

cd RFBridge/hardware/esp8266/esp8266/tools
python2.7 ./get.py

make -f /esp/makeEspArduino/makeEspArduino.mk ESP_ROOT=/esp/RFBridge/hardware/esp8266/esp8266 BOARD=d1_mini SKETCH=/esp/RFBridge/RFBridge/RFBridge/RFBridge.ino LIBS="/esp/RFBridge/hardware/esp8266/esp8266/libraries/ESP8266WiFi/src/ /esp/RFBridge/hardware/esp8266/esp8266/libraries/EEPROM/ /esp/RFBridge/libraries/ESPAsyncTCP/src/ /esp/RFBridge/libraries/ESPAsyncUDP/src/ /esp/RFBridge/libraries/ESPAsyncWebServer/src/ /esp/RFBridge/hardware/esp8266/esp8266/libraries/Hash/src/ /esp/RFBridge/libraries/rc-switch/ /esp/RFBridge/hardware/esp8266/esp8266/libraries/ESP8266mDNS/ /esp/RFBridge/hardware/esp8266/esp8266/libraries/"
