docker build -t rfbridge .
id=$(docker run -d -t rfbridge) 
docker cp $id://tmp/mkESP/RFBridge_d1_mini/RFBridge.bin ./rfbridge.bin
docker stop $id