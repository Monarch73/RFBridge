// 
// 
// 

#include "HttpServer.h"
#include <WiFiClient.h>
#include "HelperClass.h"

HttpServer::HttpServer()
{
	this->inputBuffer = (char *)malloc(N_INPUTBUFFER);
	if (this->inputBuffer == NULL)
	{
		Serial.println("out of memory");
		while (-1);
	}
}

void HttpServer::Start(int port, char *devicename, char *uuid)
{
	Serial.println("Starting Server on Port "+ String(port));
	this->_server = new WiFiServer(port);
	this->_name = devicename;
	this->_uuid = uuid;
	this->_server->begin();
}

void HttpServer::Handle()
{
	// listen for incoming clients
	WiFiClient client = this->_server->available();
	if (client) {
		Serial.println("new client");
		// an http request ends with a blank line
		while (client.connected()) {
			if (client.available()) {
				char buf[10];
				int len = client.readBytes(inputBuffer, N_INPUTBUFFER);
				Serial.write(inputBuffer, len);
				////for (int i = 0; i < len; i++)
				////{
				////	sprintf(buf, "%x", *(inputBuffer + i));
				////	Serial.print(buf);
				////}
				////Serial.println("Test");
				if (HelperClass::sstrstr(inputBuffer, "setup.xml", len) != NULL)
				{
					Serial.println("Setuppage detected");
					this->_requestedPage = SETUP;
				}
				// if you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				// so you can send a reply
				if (HelperClass::strends(inputBuffer,"\n\n",len)==0 || HelperClass::strends(inputBuffer, "\r\n\r\n", len)==0) {
					// send a standard http response header
					switch (this->_requestedPage)
					{
					case SETUP:
						this->SendTcpResponse(&client);
						break;
					default:
						break;
					}
					break;
				}
			}
		}
		// give the web browser time to receive the data
		delay(1);

		// close the connection:
		client.stop();
		Serial.println("client disonnected");
	}
}

void HttpServer::SendTcpResponse(WiFiClient * client)
{
	char outputbuffer1[sizeof(SETUP_TEMPLATE) + 50];
	char outputbuffer2[sizeof(HEADERS) + sizeof(SETUP_TEMPLATE) + 50];
	sprintf_P(outputbuffer1, SETUP_TEMPLATE, this->_name, this->_uuid);

	sprintf_P(outputbuffer2, HEADERS, strlen(outputbuffer1), outputbuffer1);
	client->write(outputbuffer2, strlen(outputbuffer2));
}
