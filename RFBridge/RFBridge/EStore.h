// EStore.h

#ifndef _ESTORE_h
#define _ESTORE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include "DipSwitches.h"
#define N_DIPSWITCHES 16
#define N_CHAR_SSID   20
#define N_CHAR_PASSWORD 80
class EStore
{
public:
	static char ssid[N_CHAR_SSID];
	static char password[N_CHAR_PASSWORD];
	EStore();
	~EStore();
	void setupEeprom(bool = false);
	void dipSwitchLoad(int, dipswitches_struct *);
	void dipSwitchDelete(int);
	void dipSwitchSave(int, dipswitches_struct *);
	void wifiSave(void);
	int dipSwitchFindFree(void);
private:

};


#endif

