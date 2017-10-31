#ifndef _DIPSWITCHES_h
#define _DIPSWITCHES_h
struct dipswitches_struct
{
	char name[20];
	char housecode[6];
	char code[6];
	char tri1[16];
	char tri2[16];
	char urlOn[91];
	char urlOff[91];
};

#endif