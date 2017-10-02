// HttpServer.h

#ifndef _HTTPSERVER_h
#define _HTTPSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <ESP8266WebServer.h>

class HttpServer
{
public:
	HttpServer();
	~HttpServer();
	void Start(int port, char *devicename);
	void Handle();

private:
	ESP8266WebServer *_server;
	char *_name;
};

#endif

