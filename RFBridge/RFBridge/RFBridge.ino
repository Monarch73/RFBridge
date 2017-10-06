#include <ESPAsyncWebServer.h>
#include <AsyncEventSource.h>
#include <RCSwitch.h>
#include "EStore.h"
#include "WebInterface.h"
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <async_config.h>
#include <AsyncPrinter.h>
#include "HelperClass.h"
#include "Client.h"
#include "WemosDevices.h"
#include "HttpServer.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

const char * ssid = "Datenpuste";
const char * password = "lidenise";

WemosDevices wemos;
EStore estore2 = EStore();
RCSwitch mySwitch = RCSwitch();
AsyncWebServer server(80);


void setup() {
	Serial.begin(115200);
	estore2.setupEeprom();
	WebInterface::estore = &estore2;
	if (estore2.ssid[0] == 0)
	{
		Serial.println("No ssid defined");
		/* You can remove the password parameter if you want the AP to be open. */
		WiFi.softAP("EasyAlexa");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		server.on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server.on("/setup", HTTP_GET, WebInterface::handleSetupSSID);
		server.begin();
		return;
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("WiFi Failed");
		while (1) {
			delay(1000);
		}
	}

	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
		Serial.println("Error setting up MDNS responder!");
	}
	Serial.println("mDNS responder started");

	wemos.Start();
	wemos.AddDevice("decke",NULL,NULL,NULL);
	wemos.AddDevice("Herd",NULL,NULL,NULL);

	server.on("/", HTTP_GET, WebInterface::HandleRoot);



	server.on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
		request->send(200, "text/plain", String(ESP.getFreeHeap()));
	});

	server.on("/send", HTTP_POST, WebInterface::HandleSpecificArg);
	server.on("/esocket", HTTP_GET, WebInterface::HandleEsocket);
	server.on("/edelete", HTTP_GET, WebInterface::HandleEDelete);
	server.on("/estore", HTTP_POST, WebInterface::HandleEStore);


	server.onNotFound([](AsyncWebServerRequest * request) {
		Serial.printf("NOT_FOUND: ");
		if (request->method() == HTTP_GET)
			Serial.printf("GET");
		else if (request->method() == HTTP_POST)
			Serial.printf("POST");
		else if (request->method() == HTTP_DELETE)
			Serial.printf("DELETE");
		else if (request->method() == HTTP_PUT)
			Serial.printf("PUT");
		else if (request->method() == HTTP_PATCH)
			Serial.printf("PATCH");
		else if (request->method() == HTTP_HEAD)
			Serial.printf("HEAD");
		else if (request->method() == HTTP_OPTIONS)
			Serial.printf("OPTIONS");
		else
			Serial.printf("UNKNOWN");
		Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

		if (request->contentLength()) {
			Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
			Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
		}
		int headers = request->headers();
		int i;
		for (i = 0; i < headers; i++) {
			AsyncWebHeader* h = request->getHeader(i);
			Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
		}

		int params = request->params();
		for (i = 0; i < params; i++) {
			AsyncWebParameter* p = request->getParam(i);
			if (p->isFile()) {
				Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
			}
			else if (p->isPost()) {
				Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
			}
			else {
				Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
			}
		}

		request->send(404);
	});

}

void loop() {
	ArduinoOTA.handle();
	// put your main code here, to run repeatedly:
	wemos.Handle();
}