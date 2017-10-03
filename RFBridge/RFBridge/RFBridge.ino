#include "HelperClass.h"
#include "Client.h"
#include "WemosDevices.h"
#include "HttpServer.h"
#include <ESP8266WiFi.h>

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

	wemos.Start();
	wemos.AddDevice("Radio");
}

void loop() {
	// put your main code here, to run repeatedly:
	wemos.Handle();
}