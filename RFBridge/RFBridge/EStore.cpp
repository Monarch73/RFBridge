// 
// 
// 
#include <EEPROM.h>
#include "EStore.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}


typedef struct dipswitches_struct dipswitch;
char EStore::ssid[N_CHAR_SSID];
char EStore::password[N_CHAR_PASSWORD];

EStore::EStore()
{
}

EStore::~EStore()
{
}

void EStore::setupEeprom(bool doit)
{
	size_t memorysize = (sizeof(dipswitches_struct)*N_DIPSWITCHES) + 4 + N_CHAR_PASSWORD + N_CHAR_SSID;
	if (memorysize > SPI_FLASH_SEC_SIZE)
	{
		Serial.println("OUT OF MEMORY");
		while (-1);
	}
	EEPROM.begin(memorysize);
	if (doit || EEPROM.read(0) != 'N' || EEPROM.read(1) != 'H' || EEPROM.read(2) != N_DIPSWITCHES || EEPROM.read(3) != (uint8_t)sizeof(dipswitches_struct))
	{
		uint pos = 0;
		EEPROM.write(pos++, 'N');
		EEPROM.write(pos++, 'H');
		EEPROM.write(pos++, N_DIPSWITCHES);
		EEPROM.write(pos++, (uint8_t)sizeof(dipswitches_struct));
		for (int cou = 0; cou < N_DIPSWITCHES; cou++)
		{
			for (uint cou2 = 0; cou2 < sizeof(dipswitches_struct); cou2++)
			{
				EEPROM.write(pos++, 0);
			}
		}

		for (int cou = 0; cou<N_CHAR_PASSWORD + N_CHAR_SSID; cou++)
		{
			EEPROM.write(pos++, 0);
		}

		EEPROM.commit();
		Serial.println("Initialized " + String(pos) + " bytes of EEPRom memory!");
	}

	int eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t*)ssid;
	for (int cou = 0; cou<N_CHAR_SSID; cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

	eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct)) + N_CHAR_SSID;
	bytepointer = (uint8_t*)password;
	for (int cou = 0; cou<N_CHAR_SSID; cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

}
void EStore::dipSwitchLoad(int no, dipswitches_struct *dswitch)
{
	uint8_t *bytepointer;
	bytepointer = (uint8_t*)dswitch;
	int eeprompos = 4 + (no * sizeof(dipswitches_struct));
	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

	return;
}

void EStore::dipSwitchDelete(int no)
{
	EEPROM.write(4 + (no * sizeof(dipswitches_struct)), 0);
	EEPROM.commit();
	Serial.println("switch removed from EEPROM");
}

void EStore::wifiSave(void)
{
	int eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t *)ssid;

	for (int cou = 0; cou < N_CHAR_SSID; cou++)
	{
		EEPROM.write(eeprompos++, *bytepointer);
		bytepointer++;
	}

	eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct)) + N_CHAR_SSID;
	bytepointer = (uint8_t *)password;

	for (int cou = 0; cou < N_CHAR_PASSWORD; cou++)
	{
		EEPROM.write(eeprompos++, *bytepointer);
		Serial.print(String(*bytepointer) + " ");
		bytepointer++;
	}

	EEPROM.commit();

}

void EStore::dipSwitchSave(int no, dipswitches_struct *dswitch)
{
	int eeprompos = 4 + (no * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t *)dswitch;

	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		EEPROM.write(eeprompos++, *bytepointer);
		bytepointer++;
	}
	EEPROM.commit();
}

int EStore::dipSwitchFindFree(void)
{
	int eeprompos = 4;
	for (int cou = 0; cou < N_DIPSWITCHES; cou++)
	{
		if (EEPROM.read(eeprompos) == 0)
		{
			return cou;
		}

		eeprompos += sizeof(dipswitches_struct);

	}

	return -1;
}