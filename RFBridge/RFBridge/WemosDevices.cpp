// 
// 
// 
#include "WemosDevices.h"
#include <ESP8266WiFi.h>

// this requires updated arduino-core. Follow instructions at 
// http://www.monarch.de/redmine/projects/hueskeo/wiki/Update_arduino_esp8266-core
#include "ESPAsyncUDP.h"
#include "HttpServer.h"
#include "WeMo.h"

void WemosDevices::Start()
{
	this->_inputBuffers = (char *)malloc(INPUTBUFFERSIZE);
	if (_udp.listenMulticast(IPAddress(239, 255, 255, 250), 1900))
	{
		Serial.println(WiFi.localIP());

		_udp.onPacket([this](AsyncUDPPacket packet) {
			int udpPattern = 0;
			int len = packet.length() - 1 > INPUTBUFFERSIZE ? INPUTBUFFERSIZE : packet.length() - 1;

			memcpy(this->_inputBuffers, packet.data(), packet.length());
			
			Serial.print("UDP Packet Type: ");
			Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast" : "Unicast");
			Serial.print(", From: ");
			Serial.print(packet.remoteIP());
			Serial.print(":");
			Serial.println(packet.remotePort());
			char * p = (char *)this->_inputBuffers;
			p[len+1] = 0;

			if (strstr(p, UDP_SEARCH_PATTERN) == this->_inputBuffers) 
			{
				if (strstr(p, UDP_DEVICE_PATTERN_1) != NULL) udpPattern = 1;
				if (strstr(p, UDP_DEVICE_PATTERN_2) != NULL) udpPattern = 2;
				if (strstr(p, UDP_DEVICE_PATTERN_3) != NULL) udpPattern = 3;
				if (strstr(p, UDP_ROOT_DEVICE) != NULL) udpPattern = 3;
				if (udpPattern) 
				{
					for (int i = 0; i < N_SERVER; i++)
					{
						if (_servers[i] != NULL)
						{
							_servers[i]->SendUdpResponse(&packet, udpPattern);
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

void WemosDevices::RemoveDevice(volatile char *name)
{
	for (int i = 0; i < N_SERVER; i++)
	{
		if (this->_servers[i] != NULL)
		{
			if (this->_servers[i]->_name != NULL && strlen(this->_servers[i]->_name) == strlen((char *)name) && strcmp(this->_servers[i]->_name,(char *) name) == 0)
			{
				this->_servers[i]->Stop();
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

void WemosDevices::SetStateDevice(volatile char *name, int state)
{
	for (int i = 0; i < N_SERVER; i++)
	{
		if (this->_servers[i] != NULL)
		{
			if (this->_servers[i]->_name != NULL && strlen(this->_servers[i]->_name) == strlen((char *)name) && strcmp(this->_servers[i]->_name, (char *)name) == 0)
			{
				Serial.print("Setting state ");
				Serial.println(state);
				this->_servers[i]->SetState(state);
			}
		}
	}
}
