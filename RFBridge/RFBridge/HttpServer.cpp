// 
// 
// 

#include "HttpServer.h"
#include "HelperClass.h"
#include "WeMo.h"

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

#ifdef DEBUG
		for (uint cou = 0; cou < len; cou++)
			Serial.print(*(inputBuffer+cou));
		Serial.println("");
#endif // DEBUG

		const char match[] = { "GET /setup.xml HTTP/1.1" };
		const char matchService[] = { "GET /eventservice.xml HTTP/1.1" };
		const char matchInfoService[] = { "GET /metainfoservice.xml HTTP/1.1" };
		const char matchControl[] = { "POST /upnp/control/basicevent1 HTTP/1.1" };

		if (memcmp(inputBuffer, match, strlen(match) - 1) == 0)
		{
			tthis->_handleSetup(c,  inputBuffer, len);
			return;
		}

		if (memcmp(inputBuffer, matchService, strlen(match) - 1) == 0) 
		{
			tthis->_handleEventService(c, inputBuffer, len);
			return;
		}

		if (memcmp(inputBuffer, matchInfoService, strlen(match) - 1) == 0) 
		{
			tthis->_handleMetaInfoService(c, inputBuffer, len);
			return;
		}

		if (memcmp(inputBuffer, matchControl, strlen(match) - 1) == 0) 
		{
			tthis->_handleControl(c, inputBuffer, len);
			return;
		}

		Serial.println("unhandeled");


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

void HttpServer::SendTcpResponseOKGetBinaryState(AsyncClient *client)
{

	const char *response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/xml\r\n"
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
	Serial.println(resonsespace);
	client->write(resonsespace, strlen(resonsespace));

	free(resonsespace);
	free(xmlresponsespace);

}

void HttpServer::_handleSetup(AsyncClient * c, char *data, size_t len)
{
	char response[strlen_P(SETUP_XML_TEMPLATE) + 20];
	sprintf_P(response, SETUP_XML_TEMPLATE, this->_name, this->_uuid);

	char headers[strlen_P(HEADERS) + 10];
	sprintf_P(headers, HEADERS, strlen(response));

	c->write(headers, strlen(headers));
	c->write(response, strlen(response));
}

void HttpServer::_handleEventService(AsyncClient * c, char *data, size_t len)
{
	char response[strlen_P(EVENTSERVICE_TEMPLATE)];
	sprintf_P(response, EVENTSERVICE_TEMPLATE);

	char headers[strlen_P(HEADERS) + 10];
	sprintf_P(headers, HEADERS, strlen(response));

	c->write(headers, strlen(headers));
	c->write(response, strlen(response));
}

void HttpServer::_handleMetaInfoService(AsyncClient * c, char *data, size_t len)
{
	char response[strlen_P(EMPTYSERVICE_TEMPLATE)];
	sprintf_P(response, EMPTYSERVICE_TEMPLATE);

	char headers[strlen_P(HEADERS) + 10];
	sprintf_P(headers, HEADERS, strlen(response));

	c->write(headers, strlen(headers));
	c->write(response, strlen(response));
}

void HttpServer::_handleControl(AsyncClient * c, char *data, size_t len)
{
	char content[len + 1];
	memcpy(content, data, len);
	content[len] = 0;
	// The default template is the one for GetBinaryState queries
	const char * response_template = GETSTATE_TEMPLATE;

	if (strstr(content, "SetBinaryState") != NULL) 
	{

		if (strstr(content, "<BinaryState>0</BinaryState>") != NULL) 
		{
			this->_methodOff(this->_arg);
		}

		if (strstr(content, "<BinaryState>1</BinaryState>") != NULL) 
		{
			this->_methodOn(this->_arg);
		}

		// Use a specific response template for SetBinaryState action
		response_template = SETSTATE_TEMPLATE;

	}

	// Update current state

	// Send response
	char response[strlen_P(response_template)];
	sprintf_P(response, response_template, this->state);

	char headers[strlen_P(HEADERS) + 10];
	sprintf_P(headers, HEADERS, strlen(response));

	c->write(headers, strlen(headers));
	c->write(response, strlen(response));

}