// HttpServer.h

#ifndef _HTTPSERVER_h
#define _HTTPSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <WiFiServer.h>

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
	SETUP
};

class HttpServer
{
public:
	HttpServer();
	~HttpServer();
	void Start(int port, char *devicename, char *uuid);
	void SendTcpResponse(WiFiClient *client);

	void Handle();

private:
	WiFiServer *_server;
	char *_name;
	char *_uuid;
	REQUESTPAGE _requestedPage;
	char *inputBuffer;
};

#endif

