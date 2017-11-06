// 
// 
// 

#include "HttpServer.h"
#include "HelperClass.h"

HttpServer::HttpServer()
{
	this->_objId = 0x0c1a0;
	this->_client = NULL;
}

void HttpServer::onPoll(void *obj, AsyncClient* c) {
	//os_printf("p:%lu\n", millis());
}

void HttpServer::onError(void *obj, AsyncClient* c, int8_t error) {
	os_printf("e:%d\n", error);
}

void HttpServer::onAck(void *obj, AsyncClient* c, size_t len, uint32_t time) {
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

#ifdef _DEBUG
		for (uint cou = 0; cou < len; cou++)
			Serial.print(*(inputBuffer+cou));
		Serial.println("");
#endif // DEBUG


		if (HelperClass::sstrstr(inputBuffer, (char*)"setup.xml", len) != NULL)
		{
			tthis->_requestedPage = SETUP;
		}
		else if (HelperClass::sstrstr(inputBuffer, (char*)"u:GetBinaryState", len) != NULL)
		{
			tthis->_requestedPage = SWITCHSTATE;
		}
		else if (HelperClass::sstrstr(inputBuffer, (char*)"<BinaryState>1</BinaryState>",len) != NULL)
		{
			tthis->_requestedPage = SWITCHON;
		}
		else if (HelperClass::sstrstr(inputBuffer, (char*)"<BinaryState>0</BinaryState>",len) != NULL)
		{
			tthis->_requestedPage = SWITCHOFF;
		}

#ifdef _DEBUG
		Serial.print("Action:");
		Serial.println(tthis->_requestedPage);
#endif // _DEBUG

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
			if (tthis->_methodOff != NULL && tthis->_arg != NULL)
			{
				tthis->_methodOff(tthis->_arg);
			}

			tthis->SendTcpResponseOK(c);
			break;

		case SWITCHSTATE:
			tthis->SendTcpResponseOKGetBinaryState(c);
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
	Serial.println("ON CLIENT CALLED");
	HttpServer *tthis = (HttpServer *)obj;
	if (tthis->_objId == 0x0c1a0)
	{
		if (tthis->_client)
		{
			Serial.println("removing old client");
			if (tthis->_client->connected())
			{
				tthis->_client->close();
			}
		}
	}
	else
	{
		Serial.println("Invalid this reference");
		return;
	}

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
	this->state = 0;
}

void HttpServer::Stop()
{
	if (this->_client != NULL)
	{
		Serial.println("Client unlinking");
		this->_client->onAck(NULL, NULL);
		this->_client->onConnect(NULL, NULL);
		this->_client->onData(NULL, NULL);
		this->_client->onDisconnect(NULL, NULL);
		this->_client->onError(NULL, NULL);
		this->_client->onPoll(NULL, NULL);
		this->_client->onTimeout(NULL, NULL);
		Serial.println("Client Unlinked");
		if (this->_client != NULL && this->_client->freeable())
		{

			Serial.println("feeing client");
			this->_client->close(true);
			this->_client->free();
			delete this->_client;
			this->_client = NULL;
		}
	}

	if (this->_server != NULL)
	{	
		Serial.println("Httpserver ending");
		this->_server->end();
		Serial.println("Httpserver deleting");

		delete this->_server;
		Serial.println("Httpserver deleted");

		this->_server = NULL;
	}
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
}

void HttpServer::SendTcpResponseOK(AsyncClient *client)
{
	const char *response = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/plain\r\n"
	"Content-Length: 0\r\n\r\n";

	client->write(response, strlen(response));
}

void HttpServer::SendTcpResponseOKGetBinaryState(AsyncClient *client)
{

	const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/plain\r\n"
		"Content-Length: %d\r\n\r\n%s";

	const char *xmlResponse = "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
	"<s:Body>"
	"<u:GetBinaryStateResponse xmlns:u=\"urn:Belkin:service:basicevent:1\">"
		"<BinaryState>%d</BinaryState>"
	"</u:GetBinaryStateResponse>"
		"</s:Body>"
		"</s:Envelope>";

	char *xmlresponsespace = (char*)malloc(strlen(xmlResponse)+10);
	sprintf(xmlresponsespace, xmlResponse, this->state);
	char *resonsespace = (char *)malloc(strlen(response) + strlen(xmlresponsespace) + 10);
	sprintf(resonsespace, response, strlen(xmlResponse), xmlresponsespace);
	//Serial.println(resonsespace);
	client->write(resonsespace, strlen(resonsespace));

	free(resonsespace);
	free(xmlresponsespace);

}
