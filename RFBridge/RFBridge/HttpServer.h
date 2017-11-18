// HttpServer.h

#ifndef _HTTPSERVER_h
#define _HTTPSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESPAsyncTCP.h>
#include "CallbackType.h"

#define N_INPUTBUFFER 1024

enum REQUESTPAGE
{
	NONE = 0,
	SETUP,
	SWITCHON,
	SWITCHOFF,
	SWITCHSTATE
};

class HttpServer
{
public:
	HttpServer();
	~HttpServer();
	void Start(int port, char *devicename, char *uuid, callbacktype methodOn, callbacktype methodOff, void *arg);
	void Stop();
	void Handle();
	int state;

private:
	void _handleSetup(AsyncClient *c, char *buf, size_t len);
	void _handleEventService(AsyncClient *c, char *buf, size_t len);
	void _handleMetaInfoService(AsyncClient *c, char *buf, size_t len);
	void _handleControl(AsyncClient *c, char *buf, size_t len);
	AsyncServer *_server;
	AsyncClient *_client;
	char *_name;
	char *_uuid;
	REQUESTPAGE _requestedPage;
	uint16_t _objId;
	void SendTcpResponse(AsyncClient *client);
	void SendTcpResponseOK(AsyncClient *client);
	void SendTcpResponseOKGetBinaryState(AsyncClient *client);
	static void onClient(void *obj, AsyncClient* c);
	static void onData(void *obj, AsyncClient* c, void *buf, size_t len);
	static void onTimeout(void *obj, AsyncClient* c, uint32_t time);
	static void onDisconnect(void *obj, AsyncClient* c);
	static void onAck(void *obj, AsyncClient* c, size_t len, uint32_t time);
	static void onError(void *obj, AsyncClient* c, int8_t error);
	static void onPoll(void *obj, AsyncClient* c);
	callbacktype _methodOn;
	callbacktype _methodOff;
	void *_arg;
};

#endif

