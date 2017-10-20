// WebInterface.h

#ifndef _WEBINTERFACE_h
#define _WEBINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "EStore.h"

#include <ESPAsyncWebServer.h>
#include "EStore.h"
#include "RCSwitch.h"
#include "WemosDevices.h"

class WebInterface
{
public:
	static EStore *estore;
	static void HandleSetupRoot(AsyncWebServerRequest *request);
	static void handleSetupSSID(AsyncWebServerRequest *request);
	static void HandleRoot(AsyncWebServerRequest *request);
	static void	HandleSpecificArg(AsyncWebServerRequest *request);
	static void HandleEsocket(AsyncWebServerRequest *request);
	static void HandleEDelete(AsyncWebServerRequest *request);
	static void HandleEStore(AsyncWebServerRequest *request);
	static void SetDevices(RCSwitch *myswitch, WemosDevices *myWemos, char *nameToDelete);
	static void TurnOn(void *arg);
	static void TurnOff(void *arg);
	static void SetNameToDelete(char *nameToDelete);
	volatile static char * GetNameToDelete();

private:
	static char *outputbuffer;
	static bool rebuildHTML;
	static RCSwitch *_mySwitch;
	static WemosDevices *_myWemos;
	volatile static char *_nameToDelete;

};


#endif

