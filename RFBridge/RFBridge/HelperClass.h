// HelperClass.h

#ifndef _HELPERCLASS_h
#define _HELPERCLASS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class HelperClass
{
public:
	static char *sstrstr(char *haystack, char *needle, int length);
	static int strends(char *heystack, char *needle, int len);

private:

};


#endif

