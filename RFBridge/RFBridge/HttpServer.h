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
const char SETUP_TEMPLATE[] PROGMEM =
"<?xml version=\"1.0\"?>"
"<root><device>"
"<deviceType>urn:Belkin:device:controllee:1</deviceType>"
"<friendlyName>%s</friendlyName>"
"<manufacturer>Belkin International Inc.</manufacturer>"
"<modelName>FauxmoESP</modelName>"
"<modelNumber>2.0.0</modelNumber>"
"<UDN>uuid:Socket-1_0-%s</UDN>"
"</device></root>";

const char HEADERS[] PROGMEM =
"HTTP/1.1 200 OK\r\n"
"CONTENT-LENGTH: %d\r\n"
"CONTENT-TYPE: text/xml\r\n"
"DATE: Sun, 01 Jan 2017 00:00:00 GMT\r\n"
"LAST-MODIFIED: Sat, 01 Jan 2017 00:00:00 GMT\r\n"
"SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
"X-USER-AGENT: redsonic\r\n"
"CONNECTION: close\r\n\r\n"
"%s\r\n";

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

private:
	AsyncServer *_server;
	AsyncClient *_client;
	char *_name;
	char *_uuid;
	REQUESTPAGE _requestedPage;
	uint16_t _objId;
	char *inputBuffer;
	void SendTcpResponse(AsyncClient *client);
	void SendTcpResponseOK(AsyncClient *client);
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

