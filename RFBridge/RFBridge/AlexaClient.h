// AlexaClient.h

#ifndef _ALEXACLIENT_h
#define _ALEXACLIENT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class AlexaClient
{
public:
	AlexaClient(char *name);
	~AlexaClient();

private:

};


#endif

