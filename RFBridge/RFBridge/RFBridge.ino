#include <ESP8266mDNS.h>
#include <tcp_axtls.h>
#include <SyncClient.h>
#include <ESPAsyncTCPbuffer.h>
#include <ESPAsyncTCP.h>
#include <async_config.h>
#include <AsyncPrinter.h>
#include "HelperClass.h"
#include "Client.h"
#include "WemosDevices.h"
#include "HttpServer.h"
#include <ESP8266WiFi.h>
#include <ArduinoOTA\ArduinoOTA.h>

const char * ssid = "Datenpuste";
const char * password = "lidenise";

WemosDevices wemos;

void setup() {
	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("WiFi Failed");
		while (1) {
			delay(1000);
		}
	}

	if (HelperClass::strends("niels", "ls", 5) != 0)
	{
		Serial.println("check failed");
		while (1);
	}

	if (!MDNS.begin("esp8266")) {             // Start the mDNS responder for esp8266.local
		Serial.println("Error setting up MDNS responder!");
	}
	Serial.println("mDNS responder started");

	wemos.Start();
	wemos.AddDevice("decke",NULL,NULL,NULL);
	wemos.AddDevice("Herd",NULL,NULL,NULL);
}

void loop() {
	ArduinoOTA.handle();
	// put your main code here, to run repeatedly:
	wemos.Handle();
}