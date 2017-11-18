// WemosDevices.h

#ifndef _WEMOSDEVICES_h
#define _WEMOSDEVICES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif
// "ESPAsyncUDP" requires updated arduino-core. Follow instructions at 
// http://www.monarch.de/redmine/projects/hueskeo/wiki/Update_arduino_esp8266-core

#include "ESPAsyncUDP.h"
#include "AlexaClient.h"
#include "HttpServer.h"

#define N_SERVER	16
#define INPUTBUFFERSIZE	2048

class WemosDevices
{
public:
	WemosDevices();
	~WemosDevices();
	void Start();
	void Handle();
	void AddDevice(char *name,callbacktype methodOn, callbacktype methodOff, void *arg=0);

	void RemoveDevice(volatile char * name);
	void SetStateDevice(volatile char *name, int state);

private:
	char *_inputBuffers;
	AsyncUDP _udp;
	AlexaClient **_servers;
};


#endif

