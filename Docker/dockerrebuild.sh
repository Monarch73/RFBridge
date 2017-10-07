docker build -t rf .
id=$(docker run -d -t rf) 
docker cp ../RFBridge/RFBridge/.  /esp/RFBridge/RFBridge/RFBridge/
docker exec /rebuild.sh
docker cp $id://tmp/mkESP/RFBridge_d1_mini/RFBridge.bin ./rfbridge.bin
docker stop $id