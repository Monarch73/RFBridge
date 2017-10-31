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

const char UDP_TEMPLATE[] PROGMEM =
"HTTP/1.1 200 OK\r\n"
"CACHE-CONTROL: max-age=86400\r\n"
"DATE: Sun, 20 Nov 2016 00:00:00 GMT\r\n"
"EXT:\r\n"
"LOCATION: http://%s:%d/setup.xml\r\n"
"OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
"01-NLS: %s\r\n"
"SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
"ST: urn:Belkin:device:**\r\n"
"USN: uuid:Socket-1_0-%s::urn:Belkin:device:**\r\n\r\n";



class AlexaClient
{
public:
	AlexaClient(char *name,int id, callbacktype methodOn, callbacktype methodOff, void *arg);
	~AlexaClient();
	void SendUdpResponse(AsyncUDPPacket *udp);
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

