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

#define N_MESSAGE_LOG 8

struct UdpMessageLog
{
	uint32_t timestamp;
	uint8_t senderIP0;
	uint8_t senderIP1;
	uint8_t senderIP2;
	uint8_t senderIP3;
	uint16_t port;
	uint8_t udpPattern;
};


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
	static UdpMessageLog* messageLog;
	static uint8_t messageLogPo;
	static void AddLog(UdpMessageLog *logEntry);
	static bool HasLog(UdpMessageLog *logEntry);

	char *_inputBuffers;
	AsyncUDP _udp;
	AlexaClient **_servers;
};


#endif

