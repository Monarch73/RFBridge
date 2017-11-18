// AlexaClient.h

#ifndef _ALEXACLIENT_h
#define _ALEXACLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "ESPAsyncUDP.h"
#include "HttpServer.h"
#include "CallbackType.h"

class AlexaClient
{
public:
	AlexaClient(char *name,int id, callbacktype methodOn, callbacktype methodOff, void *arg);
	~AlexaClient();
	void SendUdpResponse(AsyncUDPPacket *udp, int udpPattern);
	void Handle();
	void Stop();
	void SetState(int state);
	char *_name;


private:
	int _id;
	int _port;
	char *_uuid;
	char *_ip;
	HttpServer *_server;
	callbacktype _methodOn;
	callbacktype _methodOff;
	void *_arg;
};


#endif

