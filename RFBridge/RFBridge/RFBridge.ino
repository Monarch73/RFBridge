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
#include <ESP8266HTTPClient.h>

WemosDevices wemos=WemosDevices();
//EStore estore = EStore();
RCSwitch mySwitch = RCSwitch();
AsyncWebServer *server;
int deletepause = 0;
volatile int resetPause = 0;

void initDevices()
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	for (int i = 0; i < N_DIPSWITCHES; i++)
	{
		WebInterface::estore->dipSwitchLoad(i, &dp);
		if (dp.name[0] != 0)
		{
			wemos.AddDevice(dp.name, WebInterface::TurnOn, WebInterface::TurnOff, new int(i));
		}
	}
}


void setup() 
{
	Serial.begin(115200);
	mySwitch.enableTransmit(2);
	int zahl = analogRead(A0);
	Serial.println(zahl);

	WebInterface::estore = new EStore(); // &estore2;
	if (zahl > 350)
	{
		WebInterface::estore->setupEeprom(true);
	}
	else
	{
		WebInterface::estore->setupEeprom();
	}

	WebInterface::SetDevices(&mySwitch, &wemos,NULL,NULL);
	if (WebInterface::estore->ssid[0] == 0)
	{
		Serial.println("No ssid defined");
		/* You can remove the password parameter if you want the AP to be open. */
		WiFi.softAP("EasyAlexa");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		server = new AsyncWebServer(80);
		server->on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server->on("/setup", HTTP_POST, WebInterface::handleSetupSSID);
		server->begin();
		return;
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(WebInterface::estore->ssid, WebInterface::estore->password);
	zahl = 0;
	if (WiFi.waitForConnectResult() != WL_CONNECTED) 
	{
		Serial.println("WiFi Failed.Entering setup mode.");
		Serial.println("No ssid defined");
		/* You can remove the password parameter if you want the AP to be open. */
		WiFi.softAP("EasyAlexa");

		IPAddress myIP = WiFi.softAPIP();
		Serial.print("AP IP address: ");
		Serial.println(myIP);
		server = new AsyncWebServer(80);
		server->on("/", HTTP_GET, WebInterface::HandleSetupRoot);
		server->on("/setup", HTTP_POST, WebInterface::handleSetupSSID);
		server->begin();

		return;
	}

	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
		Serial.println("Error setting up MDNS responder!");
	}

	wemos.Start();
	initDevices();
	server = new AsyncWebServer(80);
	server->on("/", HTTP_GET, WebInterface::HandleRoot);



	server->on("/heap", HTTP_GET, [](AsyncWebServerRequest * request) {
		request->send(200, "text/plain", String(ESP.getFreeHeap()));
	});

	server->on("/send", HTTP_POST, WebInterface::HandleSpecificArg);
	server->on("/esocket", HTTP_GET, WebInterface::HandleEsocket);
	server->on("/edelete", HTTP_GET, WebInterface::HandleEDelete);
	server->on("/estore", HTTP_POST, WebInterface::HandleEStore);
	server->on("/format", HTTP_GET, WebInterface::HandleFormat);


	server->onNotFound([](AsyncWebServerRequest * request) {
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

	server->begin();

	// Port defaults to 8266
	// ArduinoOTA.setPort(8266);

	// Hostname defaults to esp8266-[ChipID]
	// ArduinoOTA.setHostname("esp8266");

	// No authentication by default
	// ArduinoOTA.setPassword("admin");

	// Password can be set with it's md5 value as well
	// MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
	//ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
		else // U_SPIFFS
			type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		__asm__("nop\n\t");
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();
	Serial.println("Ready");
}

void loop() {
	volatile char *nameToDelete;
	volatile char *urlToCall2;

	ArduinoOTA.handle();
	// put your main code here, to run repeatedly:
	wemos.Handle();

	if (resetPause != 0)
	{
		if (--resetPause != 0)
		{
			delay(100);
		}
		else
		{
			resetPause = 0;
			ESP.restart();
		}
	}

	if (WebInterface::reset == true)
	{
		WebInterface::reset = false;
		resetPause = 50;
	}

	if ((nameToDelete = WebInterface::GetNameToDelete())!=NULL)
	{
		if (deletepause++ < 5)
		{
			delay(100);
		}
		else
		{
			deletepause = 0;
			Serial.print("deleting ");
			Serial.println((char *)nameToDelete);

			wemos.RemoveDevice(nameToDelete);
			WebInterface::SetNameToDelete(NULL);
			free((void *)nameToDelete);
		}
	}

	if ((urlToCall2 = WebInterface::GetUrlToCall()) != NULL)
	{
		Serial.print("Calling ");
		Serial.print((char *)urlToCall2);
		HTTPClient http;
		bool ret = http.begin((char *)urlToCall2);
		Serial.println(ret);
		int ret2 = http.GET();
		Serial.print(" ");
		Serial.println(ret2);
		http.end();
		WebInterface::SetUrlToCall(NULL);
		free((void *)urlToCall2);
	}

}