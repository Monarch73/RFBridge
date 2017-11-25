// 
// 
// 

#include "WebInterface.h"

char HTML_HEADER[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>RFBridge</title><link rel=\"stylesheet/less\" type=\"text/css\" href=\"http://www.monarch.de/c64-theme/css/style.css\" />"
"<script src=\"http://www.monarch.de/c64-theme/js/less-1.3.0.min.js\" type=\"text/javascript\"></script><script>function changeFont(font) {document.getElementById('font-div').className = font; }"
"function changeFontSize(fontsize) { document.getElementById('font-size-div').className = fontsize; }</script><link rel=\"shortcut icon\" href=\"http://www.monarch.de/c64-theme/images/favicon.ico\">"
"</head><body><div class=\"container\"><div id=\"font-div\" class=\"c64u\">";

char HTML_HEADER_SETUP[] PROGMEM = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"utf-8\"><title>RFBridge</title>"
"</head><body>";


char HTML_SSID[] PROGMEM =
"<h1>RFBridge SETUP</h1>"
"<p><form method=\"POST\" action=\"/setup\">SSID:<input type=\"TEXT\" name=\"ssid\" /> Password:<input type=\"TEXT\" name=\"password\" /><input type=\"checkbox\" id=\"format\" name=\"format\" value=\"format\" /><label for=\"format\">initialize (format) Storage</lable><input type=\"submit\" name=\"send\" value=\"save\" /></form></p>"
"</body>";

char HTML_SSIDOK[] PROGMEM =
"<h1>Configuration OK - Device restarting in a couple of seconds.</h1>"
"</body>";

char HTML_TITLE[] PROGMEM =
"<h1>RFBridge</h1>";

char HTML_CONTROL[] PROGMEM =
"<p><form method=\"POST\" action=\"/send\"><table width=\"80%\" border=\"1\"><thead><tr><th colspan=\"2\">Instant</th></tr><tr><td>Housecode</td><td><input type=\"TEXT\" name=\"house\" /></td></tr><td>Code</td><td><input type=\"TEXT\" name=\"code\" /></td></tr><tr><td>on/off(0/1)</td><td><input type=\"TEXT\" name=\"onoff\" /></td></tr><tr><td>Tristate</td><td><input type=\"text\" name=\"tri\"></td></tr><tr><td colspan=\"2\" align=\"center\"><input type=\"submit\" name=\"send\" value=\"action\" /></td></tr></table></form></p>"
"<p><form method=\"POST\" action=\"/estore\"><table width=\"80%\" border=\"5\"><thead><tr><th colspan=\"2\">Switch Configuration</th></tr></thead><tr><td>Name</td><td><input type=\"TEXT\" name=\"name\" /></td></tr><tr><td>Housecode</td><td><input type=\"TEXT\" name=\"house\" /></td></tr><tr><td>Code</td><td><input type=\"TEXT\" name=\"code\" /></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr><tr><td>Tristate on</td><td><input type=\"text\" name=\"tri1\"></td></tr><tr><td>Tristate off</td><td><input type=\"text\" name=\"tri2\"></td></tr><tr><td colspan=\"2\">&nbsp;</td></tr><tr><td>URL on</td><td><input type=\"text\" name=\"url1\"></td></tr><td>URL off</td><td><input type=\"text\" name=\"url2\"></td></tr><td>&nbsp;</td><td align=\"center\"><input type=\"submit\" name=\"speichern\" value=\"save\" /></td></tr></table></form></p>"
"<p><a href=\"/download\"><button>Download Config</button></a>&nbsp;<a href=\"/upload\"><button>Upload Config</button></a></p>"
"</div></div></body>";

char HTML_DEVCONTROL_1[] PROGMEM =
"<a href=\"esocket?no=";

char HTML_DEVCONTROL_2[] PROGMEM =
"&sw=1\"> <button>ON</button></a>&nbsp;<a href=\"esocket?no=";
char HTML_DEVCONTROL_3[] PROGMEM =
"&sw=0\"><button>OFF</button></a>&nbsp;<a href=\"edelete?no=";
char HTML_DEVCONTROL_4[] PROGMEM =
"&sw=0\"><button>delete</button></a></p>";


EStore *WebInterface::estore;
bool WebInterface::rebuildHTML;
volatile bool WebInterface::reset = false;
char *WebInterface::outputbuffer=NULL;
RCSwitch *WebInterface::_mySwitch;
WemosDevices *WebInterface::_myWemos=NULL;

volatile char *WebInterface::_nameToDelete;
volatile char *WebInterface::_urlToCall;

void WebInterface::HandleSetupRoot(AsyncWebServerRequest *request)
{
	char setupoutputbuffer[sizeof(HTML_HEADER_SETUP)+sizeof(HTML_SSID)+5];
	strcpy_P(setupoutputbuffer, HTML_HEADER_SETUP);
	strcat_P(setupoutputbuffer, HTML_SSID);
	request->send(200, "text / plain", setupoutputbuffer);
}

void WebInterface::handleSetupSSID(AsyncWebServerRequest *request)
{
	char setupoutputbuffer[sizeof(HTML_HEADER_SETUP)+sizeof(HTML_SSIDOK)+5];
	String a = request->arg("ssid");
	String b = request->arg("password");
	String c = request->arg("format");

	bool format = false;
	if (c.compareTo("format") == 0)
	{
		format = true;
	}

	strcpy(WebInterface::estore->ssid, a.c_str());
	strcpy(WebInterface::estore->password, b.c_str());

	WebInterface::estore->wifiSave(format);

	strcpy_P(setupoutputbuffer, HTML_HEADER_SETUP);
	strcat_P(setupoutputbuffer, HTML_SSIDOK);
	request->send(200, "text / plain", setupoutputbuffer);
	WebInterface::reset = true;
}

void WebInterface::HandleFormat(AsyncWebServerRequest *request)
{
	WebInterface::rebuildHTML = true;
	WebInterface::estore->wifiSave(true);
	WebInterface::HandleRoot(request);
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
				memorycou += (strlen(dp.name)+2 + sizeof(HTML_DEVCONTROL_1) + sizeof(HTML_DEVCONTROL_2) + sizeof(HTML_DEVCONTROL_3) + sizeof(HTML_DEVCONTROL_4) + 20);
			}
		}

		memorycou += sizeof(HTML_CONTROL);
		outputbuffer = (char *)malloc(memorycou);
		if (outputbuffer == NULL)
		{
			Serial.println("out of memory");
			while (true); //Let the wdt reset the device
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
				strcat(outputbuffer, ":");
				strcat_P(outputbuffer, HTML_DEVCONTROL_1);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_2);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_3);
				strcat(outputbuffer, zahl);
				strcat_P(outputbuffer, HTML_DEVCONTROL_4);
				strcat(outputbuffer, "</p>");
			}
		}

		strcat_P(outputbuffer, HTML_CONTROL);
		rebuildHTML = false;
	}
	request->send(200, "text/html",outputbuffer);
	//outputbuffer is static. Don't free it here.
}

void WebInterface::SetDevices(RCSwitch *myswitch, WemosDevices *myWemos, char *nameToDelete, char *urlToCall)
{
	_mySwitch = myswitch;
	_myWemos = myWemos;
	_nameToDelete = nameToDelete;
	_urlToCall = urlToCall;
}

void WebInterface::TurnOn(void * arg)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;
	int *no = (int*)arg;
	estore->dipSwitchLoad(*no, &dp);
	if (strlen(dp.tri1) > 2)
	{
		_mySwitch->sendTriState(dp.tri1);
	}
	else if (strlen(dp.urlOn) > 2)
	{
		WebInterface::SetUrlToCall(strdup(dp.urlOn));
	}
	else
	{
		_mySwitch->switchOn(dp.housecode, dp.code);
	}
	if (_myWemos != NULL)
	{
		_myWemos->SetStateDevice((char *)dp.name, 1);
	}


}

void WebInterface::TurnOff(void * arg)
{
	typedef struct dipswitches_struct dipswitch;
	dipswitch dp;

	int *no = (int*)arg;

	estore->dipSwitchLoad(*no, &dp);
	if (strlen(dp.tri2) > 2)
	{
		_mySwitch->sendTriState(dp.tri2);
	}
	else  if (strlen(dp.urlOff) > 2)
	{
		WebInterface::SetUrlToCall(strdup(dp.urlOff));
	}
	else
	{
		_mySwitch->switchOff(dp.housecode, dp.code);
	}
	if (_myWemos != NULL)
	{
		_myWemos->SetStateDevice((char *)dp.name, 0);
	}

}

void WebInterface::SetNameToDelete(char * nameToDelete)
{
	WebInterface::_nameToDelete = nameToDelete;
}

void WebInterface::SetUrlToCall(char *urlToCall)
{
	WebInterface::_urlToCall = urlToCall;
}

volatile char * WebInterface::GetNameToDelete()
{
	return WebInterface::_nameToDelete;
}

volatile char * WebInterface::GetUrlToCall()
{
	return WebInterface::_urlToCall;
}


void WebInterface::HandleSpecificArg(AsyncWebServerRequest * request)
{
	String a = request->arg("house");
	String b = request->arg("code");
	String c = request->arg("onoff");
	String d = request->arg("tri");
	char aa[20];
	char bb[20];

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
			delay(100);

		}
		else
		{
			_mySwitch->switchOn(aa, bb);
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
		Serial.print(strlen(dp.tri2));
		Serial.print(" ");
		Serial.println(strlen(dp.urlOff));
		if (b == "0")
		{
			if (strlen(dp.tri2) > 2)
			{
				_mySwitch->sendTriState(dp.tri2);
			}
			else   if (strlen(dp.urlOff) > 2)
			{
				Serial.print("Off ");
				Serial.println(dp.urlOff);
				WebInterface::SetUrlToCall(strdup(dp.urlOff));
			}
			else
			{
				_mySwitch->switchOff(dp.housecode, dp.code);
			}

			if (_myWemos != NULL)
			{
				_myWemos->SetStateDevice((char *)dp.name, 0);
			}
			
		}
		else
		{
			if (strlen(dp.tri1) > 2)
			{
				_mySwitch->sendTriState(dp.tri1);
			}
			else if (strlen(dp.urlOn) > 2)
			{
				Serial.print("On ");
				Serial.println(dp.urlOn);
				WebInterface::SetUrlToCall(strdup(dp.urlOn));
			}
			else
			{
				_mySwitch->switchOn(dp.housecode, dp.code);
			}
			if (_myWemos != NULL)
			{
				_myWemos->SetStateDevice((char *)dp.name, 1);
			}
		}

		delay(100);
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
		if (dp.name[0] != 0)
		{
			if (_nameToDelete == NULL)
			{
				WebInterface::SetNameToDelete(strdup(dp.name));
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
	String d1 = request->arg("url1");
	String d2 = request->arg("url2");
	String d = request->arg("roomname");
	int no = estore->dipSwitchFindFree();
	if (no >= 0)
	{
		memcpy(dp.name, (char *)a.c_str(), sizeof(dp.name));
		memcpy(dp.housecode, (char *)b.c_str(), sizeof(dp.housecode));
		memcpy(dp.code, (char *)c.c_str(), sizeof(dp.code));
		//memcpy(dp.roomname, (char *)d.c_str(), sizeof(dp.roomname));
		memcpy(dp.tri1, (char*)c2.c_str(), sizeof(dp.tri1));
		memcpy(dp.tri2, (char *)c3.c_str(), sizeof(dp.tri2));
		memcpy(dp.urlOn, (char *)d1.c_str(), sizeof(dp.urlOn));
		memcpy(dp.urlOff, (char *)d2.c_str(), sizeof(dp.urlOff));
		dp.name[sizeof(dp.name) - 1] = 0;
		dp.housecode[sizeof(dp.housecode) - 1] = 0;
		dp.code[sizeof(dp.code) - 1] = 0;
		dp.tri1[sizeof(dp.tri1) - 1] = 0;
		dp.tri2[sizeof(dp.tri2) - 1] = 0;
		dp.urlOff[sizeof(dp.urlOff) - 1] = 0;
		dp.urlOn[sizeof(dp.urlOn) - 1] = 0;
		estore->dipSwitchSave(no, &dp);
		_myWemos->AddDevice(dp.name, WebInterface::TurnOn, WebInterface::TurnOff, new int(no));
		rebuildHTML = true;
	}

	HandleRoot(request);
}

