// 
// 
// 

#include "WebInterface.h"

char HTML_HEADER[] PROGMEM = "<!DOCTYPE html> <html> <head> <title>Steckdosensteuerung</title><meta charset=\"UTF-8\"><style>"
"body {background-color:#ffffff; color: #000000; font-family: 'Century Gothic', CenturyGothic, AppleGothic, sans-serif;}h1 {font-size: 2em;}\r\n"
"button {"
"background: #3498db;"
"background - image: -webkit - linear - gradient(top, #3498db, #2980b9);"
"background - image: -moz - linear - gradient(top, #3498db, #2980b9);"
"background - image: -ms - linear - gradient(top, #3498db, #2980b9);"
"background - image: -o - linear - gradient(top, #3498db, #2980b9);"
"background - image: linear - gradient(to bottom, #3498db, #2980b9);"
"-webkit - border - radius: 28;"
"-moz - border - radius: 28;"
"border - radius: 28px;"
"font - family: Arial;"
"color: #ffffff;"
"font - size: 20px;"
"padding: 10px 20px 10px 20px;"
"text - decoration: none;"
"}"
"button:hover{"
"background: #3cb0fd;"
"  background - image: -webkit - linear - gradient(top, #3cb0fd, #3498db);"
" background - image: -moz - linear - gradient(top, #3cb0fd, #3498db);"
" background - image: -ms - linear - gradient(top, #3cb0fd, #3498db);"
" background - image: -o - linear - gradient(top, #3cb0fd, #3498db);"
" background - image: linear - gradient(to bottom, #3cb0fd, #3498db);"
" text - decoration: none;"
"}"
"</style></head><body><center>";

char HTML_SSID[] PROGMEM =
"<h1>Steckdosensteuerung SETUP</h1>"
"<p><form method=\"POST\" action=\"/setup\">SSID:<input type=\"TEXT\" name=\"ssid\" /> Password:<input type=\"TEXT\" name=\"password\" /> <input type=\"submit\" name=\"send\" value=\"speichern\" /></form></p>"
"</center></body>";

char HTML_SSIDOK[] PROGMEM =
"<h1>Steckdosensteuerung OK - Please reset device.</h1>"
"</center></body>";


EStore *WebInterface::estore;

void WebInterface::HandleSetupRoot(AsyncWebServerRequest *request)
{
	char outputbuffer[sizeof(HTML_HEADER)+sizeof(HTML_SSID)+5];
	strcpy_P(outputbuffer, HTML_HEADER);
	strcat_P(outputbuffer, HTML_SSID);
	request->send(200, "text / plain", outputbuffer);
}

void WebInterface::handleSetupSSID(AsyncWebServerRequest *request)
{
	char outputbuffer[sizeof(HTML_HEADER)+sizeof(HTML_SSIDOK)+5];
	String a = request->arg("ssid");
	String b = request->arg("password");

	strcpy(WebInterface::estore->ssid, a.c_str());
	strcpy(WebInterface::estore->password, b.c_str());

	WebInterface::estore->wifiSave();

	strcpy_P(outputbuffer, HTML_HEADER);
	strcat_P(outputbuffer, HTML_SSIDOK);
	request->send(200, "text / plain", outputbuffer );
}

void WebInterface::HandleRoot(AsyncWebServerRequest * request)
{
}

void WebInterface::HandleSpecificArg(AsyncWebServerRequest * request)
{
}

void WebInterface::HandleEsocket(AsyncWebServerRequest * request)
{
}

void WebInterface::HandleEDelete(AsyncWebServerRequest * request)
{
}

void WebInterface::HandleEStore(AsyncWebServerRequest * request)
{
}

