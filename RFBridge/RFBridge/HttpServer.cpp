// 
// 
// 

#include "HttpServer.h"

HttpServer::HttpServer()
{

}

void HttpServer::Start(int port, char *devicename)
{
	ESP8266WebServer server(port);
	_server = &server;
	_name = devicename;
}

void HttpServer::Handle()
{
	_server->handleClient();
}

