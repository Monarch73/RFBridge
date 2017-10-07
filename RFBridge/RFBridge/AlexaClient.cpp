// 
// 
// 

#include <ESP8266WiFi.h>
#include "AlexaClient.h"

AlexaClient::AlexaClient(char *name,int id, callbacktype methodOn, callbacktype methodOff, void *arg)
{
	char uuid[15];
	sprintf(uuid, "444556%06X%02X\0", ESP.getChipId(), id); // "DEV" + CHIPID + DEV_ID

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
	this->_server->Stop();
	delete this->_server;
}

void AlexaClient::SendUdpResponse(AsyncUDPPacket *udp)
{
	uint8_t response[strlen(UDP_TEMPLATE) + 40];
	sprintf_P((char*)response, UDP_TEMPLATE,
		this->_ip,
		this->_port,
		this->_uuid,
		this->_uuid
	);

	Serial.println((char *)response);
	udp->write(response, strlen((char *)response));

}


void AlexaClient::Handle()
{
	this->_server->Handle();
}


AlexaClient::~AlexaClient()
{
}