// 
// 
// 

#include <ESP8266WiFi.h>
#include "AlexaClient.h"
#include "WeMo.h"

AlexaClient::AlexaClient(char *name,int id, callbacktype methodOn, callbacktype methodOff, void *arg)
{
	char uuid[15];
	sprintf(uuid, "444556%06X%02X", ESP.getChipId(), id); // "DEV" + CHIPID + DEV_ID

	char buffer[16];
	IPAddress ip = WiFi.localIP();
	sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);


	this->_id = id;
	this->_port = 81 + id;
	this->_name = strdup(name);
	this->_uuid = strdup(uuid);
	this->_ip = strdup(buffer);
	this->_methodOn = methodOn;
	this->_methodOff = methodOff;
	this->_arg = arg;
	this->_server = new HttpServer();
	this->_server->Start(this->_port, this->_name, this->_uuid,this->_methodOn,this->_methodOff,this->_arg);
}

void AlexaClient::Stop()
{
	//ESP.restart();
	if (this->_server != NULL)
	{
		Serial.println("AlexaClient stop");
		this->_server->Stop();
		Serial.println("AlexaClient delete");
		delete this->_server;
		Serial.println("AlexaClient deleted");

	}
}

void AlexaClient::SendUdpResponse(AsyncUDPPacket *udp, int udpPattern)
{
	// neu
	char buffer[16];
	IPAddress ip = WiFi.localIP();
	sprintf(buffer, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

	uint8_t response[strlen(UDP_TEMPLATE) + 128];
	snprintf_P((char *)response, sizeof(response), UDP_TEMPLATE,
		buffer,
		this->_port,
		this->_uuid,
		udpPattern == 1 ? UDP_DEVICE_PATTERN_1 : udpPattern == 2 ? UDP_DEVICE_PATTERN_2 : UDP_DEVICE_PATTERN_3,
		this->_uuid,
		udpPattern == 1 ? UDP_DEVICE_PATTERN_1 : udpPattern == 2 ? UDP_DEVICE_PATTERN_2 : UDP_DEVICE_PATTERN_3
	);

	udp->write(response, strlen((char *)response));
}


void AlexaClient::Handle()
{
	this->_server->Handle();
}


AlexaClient::~AlexaClient()
{
}

void AlexaClient::SetState(int state)
{
	if (this->_server != NULL)
	{
		this->_server->state = state;
	}
}