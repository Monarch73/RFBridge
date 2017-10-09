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

	if (_udp.listenMulticast(IPAddress(239, 255, 255, 250), 1900))
	{
		Serial.println(WiFi.localIP());

		_udp.onPacket([this](AsyncUDPPacket packet) {
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
//			Serial.write(packet.data(), packet.length());
			Serial.println();
			//reply to the client
			//packet.printf("Got %u bytes of data", packet.length());
			if (strstr((char *)data, UDP_SEARCH_PATTERN) != NULL)
			{
				if (strstr((char *)data, UDP_DEVICE_PATTERN) != NULL)
				{
					Serial.println("Ich glaub, ich sollte antworten.");
					for (int i = 0; i < N_SERVER; i++)
					{
						if (_servers[i] != NULL)
						{
							_servers[i]->SendUdpResponse(&packet);
						}
					}
				}
			}

		});
	}
	else
	{
		Serial.println("Failed to install multicast listener");
	}
}

void WemosDevices::AddDevice(char *name, callbacktype methodOn,callbacktype methodOff, void *arg)
{
	for (int i = 0; i < N_SERVER; i++)
	{
		if (this->_servers[i] == NULL)
		{
			this->_servers[i] = new AlexaClient(name, i, methodOn, methodOff,arg);
			return;
		}
	}
}

void WemosDevices::RemoveDevice(char *name)
{
	for (int i = 0; i < N_SERVER; i++)
	{
		if (this->_servers[i] != NULL)
		{
			if (this->_servers[i]->_name != NULL && strlen(this->_servers[i]->_name) == strlen(name) && strcmp(this->_servers[i]->_name, name) == 0)
			{
				Serial.println("found it");
				this->_servers[i]->Stop();
				Serial.println("stopped");
				delete this->_servers[i];

				this->_servers[i] = NULL;

			}
		}
	}
}


void WemosDevices::Handle()
{
	for (int i = 0; i < N_SERVER; i++)
	{
		if (this->_servers[i])
		{
			this->_servers[i]->Handle();
		}
	}
}

WemosDevices::WemosDevices()
{
	_servers = new AlexaClient*[N_SERVER];
	for (int i = 0; i < N_SERVER; i++)
	{
		_servers[i] = NULL;
	}
}

WemosDevices::~WemosDevices()
{
}

