id=$(docker run -d -t rfbridge) 
docker cp ../RFBridge/RFBridge/.  $id:/esp/RFBridge/RFBridge/RFBridge/
docker exec $id /rebuild.sh
docker cp $id://tmp/mkESP/RFBridge_d1_mini/RFBridge.bin ./rfbridge.bin
docker stop $id