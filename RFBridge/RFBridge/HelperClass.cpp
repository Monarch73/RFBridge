// 
// 
// 

#include "HelperClass.h"

char * HelperClass::sstrstr(char *haystack, char *needle, int length)
{
	int needle_length = strlen(needle);
	int i;

	for (i = 0; i < length; i++)
	{
		if (i + needle_length > length)
		{
			return NULL;
		}

		if (strncmp(&haystack[i], needle, needle_length) == 0)
		{
			return &haystack[i];
		}
	}
	return NULL;
}

int HelperClass::strends(char *heystack, char *needle, int len)
{
	int stringlen = strlen(needle);

	if (len> stringlen + 1)
	{
		return strncmp(heystack + (len - stringlen), needle, stringlen);
	}

	return -1;
}

