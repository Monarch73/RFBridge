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

char HTML_TITLE[] PROGMEM =
"<h1>Steckdosensteuerung</h1>";

char HTML_CONTROL[] PROGMEM =
"<p><form method=\"POST\" action=\"/send\">Housecode:<input type=\"TEXT\" name=\"house\" /> Code:<input type=\"TEXT\" name=\"code\" /> Ein/aus:<input type=\"TEXT\" name=\"onoff\" /> oder Tristate:<input type=\"text\" name=\"tri\"><input type=\"submit\" name=\"send\" value=\"schalten\" /></form></p>"
"<p><form method=\"POST\" action=\"/estore\">Name:<input type=\"TEXT\" name=\"name\" /> Housecode:<input type=\"TEXT\" name=\"house\" /> Code:<input type=\"TEXT\" name=\"code\" /> Room:<input type=\"TEXT\" name=\"roomname\" /> TristateON:<input type=\"text\" name=\"tri1\"> TristateOff:<input type=\"text\" name=\"tri2\"><input type=\"submit\" name=\"speichern\" value=\"speichern\" /></form></p>"
"</center></body>";

char HTML_DEVCONTROL_1[] PROGMEM =
"<a href=\"esocket?no=";

char HTML_DEVCONTROL_2[] PROGMEM =
"&sw=1\"><button>AN</button></a>&nbsp;<a href=\"esocket?no=";
char HTML_DEVCONTROL_3[] PROGMEM =
"&sw=0\"><button>AUS</button></a><a href=\"edelete?no=";
char HTML_DEVCONTROL_4[] PROGMEM =
"&sw=0\"><button>Loeschen</button></a></p>";


EStore *WebInterface::estore;
bool WebInterface::rebuildHTML;
char *WebInterface::outputbuffer=NULL;
RCSwitch *WebInterface::_mySwitch;
WemosDevices *WebInterface::_myWemos;

volatile char *WebInterface::_nameToDelete;

void WebInterface::HandleSetupRoot(AsyncWebServerRequest *request)
{
	char setupoutputbuffer[sizeof(HTML_HEADER)+sizeof(HTML_SSID)+5];
	strcpy_P(setupoutputbuffer, HTML_HEADER);
	strcat_P(setupoutputbuffer, HTML_SSID);
	request->send(200, "text / plain", setupoutputbuffer);
}

void WebInterface::handleSetupSSID(AsyncWebServerRequest *request)
{
	char setupoutputbuffer[sizeof(HTML_HEADER)+sizeof(HTML_SSIDOK)+5];
	String a = request->arg("ssid");
	String b = request->arg("password");

	strcpy(WebInterface::estore->ssid, a.c_str());
	strcpy(WebInterface::estore->password, b.c_str());

	WebInterface::estore->wifiSave();

	strcpy_P(setupoutputbuffer, HTML_HEADER);
	strcat_P(setupoutputbuffer, HTML_SSIDOK);
	request->send(200, "text / plain", setupoutputbuffer);
}

void WebInterface::HandleRoot(AsyncWebServerRequest * request)
{
	if (outputbuffer == NULL || rebuildHTML == true)
	{
		char zahl[5];

		if (outputbuffer != NULL)
		{
			free(outputbuffer);
		}
		// if you want to modify body part of html start here
		// socket names and buttons are created dynamical

		int memorycou = sizeof(HTML_HEADER)+ sizeof(HTML_TITLE);

		typedef struct dipswitches_struct dipswitch;
		dipswitch dp;

		for (int i = 0; i < N_DIPSWITCHES; i++)
		{
			estore->dipSwitchLoad(i, &dp);
			if (dp.name[0] != 0)
			{
				memorycou += (strlen(dp.name) + sizeof(HTML_DEVCONTROL_1) + sizeof(HTML_DEVCONTROL_2) + sizeof(HTML_DEVCONTROL_3) + sizeof(HTML_DEVCONTROL_4) + 20);
			}
		}

		memorycou += sizeof(HTML_CONTROL);
		outputbuffer = (char *)malloc(memorycou);
		if (outputbuffer == NULL)
		{
			Serial.println("kein speicher");
			while (true);
		}

		strcpy_P(outputbuffer, HTML_HEADER);
		strcat_P(outputbuffer, HTML_TITLE);
		for (int i = 0; i < N_DIPSWITCHES; i++)
		{
			estore->dipSwitchLoad(i, &dp);
			if (dp.name[0] != 0)
			{
				sprintf(zahl, "%d", i);
				strcat(outputbuffer, "<p>");
				strcat(outputbuffer, dp.name);
				strcat_P(outputbuffer, HTML_DEVCONTROL_1);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_2);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_3);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_4);
			}
		}

		strcat_P(outputbuffer, HTML_CONTROL);
		rebuildHTML = false;
	}
	request->send(200, "text/html", outputbuffer);
}

void WebInterface::SetDevices(RCSwitch *myswitch, WemosDevices *myWemos, char *nameToDelete)
{
	_mySwitch = myswitch;
	_myWemos = myWemos;
	_nameToDelete = nameToDelete;
}

void WebInterface::TurnOn(void * arg)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	int *no = (int*)arg;
	Serial.print(*no);
	Serial.println(" turn on");
	estore->dipSwitchLoad(*no, &dp);
	if (strlen(dp.tri1) > 2)
	{
		_mySwitch->sendTriState(dp.tri1);
	}
	else
	{

		_mySwitch->switchOff(dp.housecode, dp.code);
	}

}

void WebInterface::TurnOff(void * arg)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	int *no = (int*)arg;
	Serial.print(*no);
	Serial.println(" turn off");

	estore->dipSwitchLoad(*no, &dp);
	if (strlen(dp.tri2) > 2)
	{
		_mySwitch->sendTriState(dp.tri2);
	}
	else
	{
		_mySwitch->switchOff(dp.housecode, dp.code);
	}
}

void WebInterface::SetNameToDelete(char * nameToDelete)
{
	WebInterface::_nameToDelete = nameToDelete;
}

volatile char * WebInterface::GetNameToDelete()
{
	return WebInterface::_nameToDelete;
}

void WebInterface::HandleSpecificArg(AsyncWebServerRequest * request)
{
	String a = request->arg("house");
	String b = request->arg("code");
	String c = request->arg("onoff");
	String d = request->arg("tri");
	char aa[20];
	char bb[20];

	Serial.println("Nehme argumente: " + a + b + c);

	if (a == "" || b == "" || c == "") 
	{ //Parameter not found
		if (d.length() > 2)
		{
			_mySwitch->sendTriState(d.c_str());
		}
	}
	else {     //Parameter found
		a.toCharArray(aa, sizeof(aa));
		b.toCharArray(bb, sizeof(bb));
		if (c == "0")
		{
			_mySwitch->switchOff(aa, bb);
			Serial.println("OFF " + a + b);
			delay(100);

		}
		else
		{
			_mySwitch->switchOn(aa, bb);
			Serial.println("ON " + a + b);
			delay(100);
		}
	}

	HandleRoot(request);
	
}

void WebInterface::HandleEsocket(AsyncWebServerRequest * request)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	String a = request->arg("no");
	String b = request->arg("sw");
	int no = atoi(a.c_str());
	if (no < N_DIPSWITCHES)
	{
		estore->dipSwitchLoad(no, &dp);
		if (b == "0")
		{
			if (strlen(dp.tri2) > 2)
			{
				_mySwitch->sendTriState(dp.tri2);
			}
			else
			{
				_mySwitch->switchOff(dp.housecode, dp.code);
			}
		}
		else
		{
			if (strlen(dp.tri1) > 2)
			{
				_mySwitch->sendTriState(dp.tri1);
			}
			else
			{
				_mySwitch->switchOn(dp.housecode, dp.code);
			}
		}
		delay(100);
		Serial.println(b == "0" ? " off" : " on");
	}

	HandleRoot(request);
}

void WebInterface::HandleEDelete(AsyncWebServerRequest * request)
{
	String a = request->arg("no");
	int no = atoi(a.c_str());
	if (no < N_DIPSWITCHES)
	{
		typedef struct dipswitches_struct dipswitch;
		dipswitch dp;
		estore->dipSwitchLoad(no, &dp);
		Serial.print("Loaded switch "); Serial.println(no);
		if (dp.name[0] != 0)
		{
			Serial.println(String(dp.name) + " remove");
			//fauxmo.removeDevice(dp.name);
			//_myWemos->RemoveDevice(dp.name);
			if (_nameToDelete == NULL)
			{
				_nameToDelete = dp.name;
			}
				
		}
		estore->dipSwitchDelete(no);
		rebuildHTML = true;

	}
	HandleRoot(request);
}

void WebInterface::HandleEStore(AsyncWebServerRequest * request)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	String a = request->arg("name");
	String b = request->arg("house");
	String c = request->arg("code");
	String c2 = request->arg("tri1");
	String c3 = request->arg("tri2");

	String d = request->arg("roomname");
	int no = estore->dipSwitchFindFree();
	Serial.println("Free slot to store " + String(no));
	if (no >= 0)
	{
		memcpy(dp.name, (char *)a.c_str(), sizeof(dp.name));
		memcpy(dp.housecode, (char *)b.c_str(), sizeof(dp.housecode));
		memcpy(dp.code, (char *)c.c_str(), sizeof(dp.code));
		memcpy(dp.roomname, (char *)d.c_str(), sizeof(dp.roomname));
		memcpy(dp.tri1, (char*)c2.c_str(), sizeof(dp.tri1));
		memcpy(dp.tri2, (char *)c3.c_str(), sizeof(dp.tri2));
		dp.name[sizeof(dp.name) - 1] = 0;
		dp.housecode[sizeof(dp.housecode) - 1] = 0;
		dp.code[sizeof(dp.code) - 1] = 0;
		dp.tri1[sizeof(dp.tri1) - 1] = 0;
		dp.tri2[sizeof(dp.tri2) - 1] = 0;
		estore->dipSwitchSave(no, &dp);
		Serial.println(String(dp.name) + " adding");
		_myWemos->AddDevice(dp.name, WebInterface::TurnOn, WebInterface::TurnOff, new int(no));
		rebuildHTML = true;
	}

	HandleRoot(request);
}

