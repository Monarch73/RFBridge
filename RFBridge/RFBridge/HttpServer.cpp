// 
// 
// 

#include "HttpServer.h"
#include "HelperClass.h"

HttpServer::HttpServer()
{
	this->_objId = 0x0c1a0;
	this->_client = NULL;
	this->inputBuffer = (char *)malloc(N_INPUTBUFFER);
	if (this->inputBuffer == NULL)
	{
		Serial.println("out of memory");
		while (-1);
	}
}

void HttpServer::onPoll(void *obj, AsyncClient* c) {
	//os_printf("p:%lu\n", millis());
}

void HttpServer::onError(void *obj, AsyncClient* c, int8_t error) {
	HttpServer *tthis = (HttpServer *)obj;
	os_printf("e:%d\n", error);
}

void HttpServer::onAck(void *obj, AsyncClient* c, size_t len, uint32_t time) {
	HttpServer *tthis = (HttpServer *)obj;
	os_printf("a:%u:%u\n", len, time);
}

void HttpServer::onDisconnect(void *obj, AsyncClient* c) {
	HttpServer *tthis = (HttpServer *)obj;

	os_printf("d\n");
	c->free();
	delete c;
	tthis->_client = NULL;
}

void HttpServer::onTimeout(void *obj, AsyncClient* c, uint32_t time) {
	HttpServer *tthis = (HttpServer *)obj;

	os_printf("t:%u\n", time);
	tthis->_client->close();
}

void HttpServer::onData(void *obj, AsyncClient* c, void *buf, size_t len) {
	HttpServer *tthis = (HttpServer *)obj;
	if (tthis->_objId == 0x0c1a0)
	{
		char *inputBuffer = (char *)buf;

		if (HelperClass::sstrstr(inputBuffer, "setup.xml", len) != NULL)
		{
			Serial.println("Setuppage detected");
			tthis->_requestedPage = SETUP;
		}
		else if (HelperClass::sstrstr(inputBuffer, "<BinaryState>1</BinaryState>",len) != NULL)
		{
			Serial.println("on detected");
			tthis->_requestedPage = SWITCHON;
		}
		else if (HelperClass::sstrstr(inputBuffer, "<BinaryState>0</BinaryState>",len) != NULL)
		{
			Serial.println("off detected");
			tthis->_requestedPage = SWITCHOFF;
		}

		switch (tthis->_requestedPage)
		{
		case SETUP:

			tthis->SendTcpResponse(c);
			break;
		case SWITCHON:
			tthis->SendTcpResponseOK(c);
			if (tthis->_methodOn != NULL && tthis->_arg != NULL)
			{
				tthis->_methodOn(tthis->_arg);
			}
			break;
		case SWITCHOFF:
			Serial.println("onData5");
			if (tthis->_methodOff != NULL && tthis->_arg != NULL)
			{
				tthis->_methodOff(tthis->_arg);
			}

			tthis->SendTcpResponseOK(c);

			break;
		default:
			break;
		}


		tthis->_requestedPage = NONE;

		c->close();

		c->free();

		tthis->_client = NULL;
	}
	else
	{
		Serial.println("onData invalid reference");
	}


}

void HttpServer::onClient(void *obj, AsyncClient* c) {
	Serial.println("onClient1");
	HttpServer *tthis = (HttpServer *)obj;
	Serial.println("onClient2");
	if (tthis->_objId == 0x0c1a0)
	{
		if (tthis->_client)
		{
			Serial.println("onClient3");

			if (tthis->_client->connected())
			{
				Serial.println("onClient4");

				tthis->_client->close();
			}
		}
	}
	else
	{
		Serial.println("Invalid this reference");
		return;
	}

	Serial.println("onClient5");

	tthis->_client = c;
	tthis->_client->onError(HttpServer::onError,tthis);
	tthis->_client->onAck(HttpServer::onAck, tthis);
	tthis->_client->onDisconnect(HttpServer::onDisconnect,tthis);
	tthis->_client->onTimeout(HttpServer::onTimeout,tthis);
	tthis->_client->onData(HttpServer::onData,tthis);
	//client->onPoll(onPoll);

}

void HttpServer::Start(int port, char *devicename, char *uuid, callbacktype methodOn, callbacktype methodOff, void *arg)
{
	Serial.println("Starting Server on Port "+ String(port));
	this->_server = new AsyncServer(port);
	this->_server->onClient(HttpServer::onClient, this);
	this->_name = devicename;
	this->_uuid = uuid;
	this->_server->begin();
	this->_methodOn = methodOn;
	this->_methodOff = methodOff;
	this->_arg = arg;
}

void HttpServer::Stop()
{
	Serial.println("httpserver ending");
	this->_server->end();
	Serial.println("httpserver ended");
	delete this->_server;
	Serial.println("httpserver deleted");
}

HttpServer::~HttpServer()
{

}

void HttpServer::Handle()
{
}

void HttpServer::SendTcpResponse(AsyncClient * client)
{
	char outputbuffer1[sizeof(SETUP_TEMPLATE) + 50];
	char outputbuffer2[sizeof(HEADERS) + sizeof(SETUP_TEMPLATE) + 50];
	sprintf_P(outputbuffer1, SETUP_TEMPLATE, this->_name, this->_uuid);

	sprintf_P(outputbuffer2, HEADERS, strlen(outputbuffer1), outputbuffer1);
	client->write(outputbuffer2, strlen(outputbuffer2));
	Serial.write(outputbuffer2, strlen(outputbuffer2));
}

void HttpServer::SendTcpResponseOK(AsyncClient *client)
{
	Serial.println("sending ok");
		const char *response = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Length: 0\r\n\r\n";

	client->write(response, strlen(response));
}
