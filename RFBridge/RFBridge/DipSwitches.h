#ifndef _DIPSWITCHES_h
#define _DIPSWITCHES_h
struct dipswitches_struct
{
	char name[20];
	char housecode[6];
	char code[6];
	char tri1[16];
	char tri2[16];
	char urlOn[160];
	char urlOff[160];
};

#endif