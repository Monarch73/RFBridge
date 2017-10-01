#include <ESP8266WiFi.h>
// this requires updated arduino-core. Follow instructions at 
// http://www.monarch.de/redmine/projects/hueskeo/wiki/Update_arduino_esp8266-core
#include "ESPAsyncUDP.h"

const char * ssid = "Datenpuste";
const char * password = "lidenise";

AsyncUDP udp;

void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("WiFi Failed");
		while (1) {
			delay(1000);
		}
	}
	if (udp.listenMulticast(IPAddress(239, 255, 255, 250), 1900)) {
		Serial.print("UDP Listening on IP: ");
		Serial.println(WiFi.localIP());
		udp.onPacket([](AsyncUDPPacket packet) {
			Serial.print("UDP Packet Type: ");
			Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
			Serial.print(", From: ");
			Serial.print(packet.remoteIP());
			Serial.print(":");
			Serial.print(packet.remotePort());
			Serial.print(", To: ");
			Serial.print(packet.localIP());
			Serial.print(":");
			Serial.print(packet.localPort());
			Serial.print(", Length: ");
			Serial.print(packet.length());
			Serial.print(", Data: ");
			Serial.write(packet.data(), packet.length());
			Serial.println();
			//reply to the client
			packet.printf("Got %u bytes of data", packet.length());
		});
		//Send multicast
		udp.print("Hello!");
	}
}

void loop() {
	// put your main code here, to run repeatedly:

}