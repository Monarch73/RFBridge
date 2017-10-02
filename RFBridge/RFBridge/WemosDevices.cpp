// 
// 
// 

#include "WemosDevices.h"
#include <ESP8266WiFi.h>

// this requires updated arduino-core. Follow instructions at 
// http://www.monarch.de/redmine/projects/hueskeo/wiki/Update_arduino_esp8266-core
#include "ESPAsyncUDP.h"
#include "HttpServer.h"

void WemosDevices::Start()
{
	_servers = new AlexaClient*[N_SERVER];
	for (int i = 0; i < N_SERVER; i++)
	{
		_servers[i] = NULL;
	}

	this->_web = new HttpServer();
	this->_web->Start(80, "test");

	if (_udp.listenMulticast(IPAddress(239, 255, 255, 250), 1900))
	{
		Serial.println(WiFi.localIP());

		_udp.onPacket([](AsyncUDPPacket packet) {
			char *data = (char *)packet.data();
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
			if (strstr((char *)data, UDP_SEARCH_PATTERN) != NULL)
			{
				if (strstr((char *)data, UDP_DEVICE_PATTERN) != NULL)
				{
					Serial.println("Ich glaub, ich sollte antworten.");
				}
			}

		});
	}
	else
	{
		Serial.print("Failed to install multicast listener");
	}
}

void WemosDevices::Handle()
{
	this->_web->Handle();
}

WemosDevices::WemosDevices()
{
}

WemosDevices::~WemosDevices()
{
}
