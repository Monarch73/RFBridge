// 
// 
// 
#include <EEPROM.h>
#include <FS.h>
#include "EStore.h"

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
#include "os_type.h"
#include "osapi.h"
#include "spi_flash.h"
}

#define USE_SPIFFS	1

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
	uint8_t dipswitchsize = (uint8_t)((uint)sizeof(dipswitches_struct));
#ifndef USE_SPIFFS

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

		for (int cou = 0; cou < N_CHAR_PASSWORD + N_CHAR_SSID; cou++)
		{
			EEPROM.write(pos++, 0);
		}

		EEPROM.commit();
		Serial.println("Initialized " + String(pos) + " bytes of EEPRom memory!");
	}

	int eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t*)ssid;
	for (int cou = 0; cou < N_CHAR_SSID; cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

	eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct)) + N_CHAR_SSID;
	bytepointer = (uint8_t*)password;
	for (int cou = 0; cou < N_CHAR_SSID; cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

#else
	if (doit)
	{
		SPIFFS.format();
	}

	if (SPIFFS.begin() == false)
	{
		Serial.println("mounting SPIFFS failed...formatting");
		if (SPIFFS.format() == false)
		{
			Serial.println("formatting SPIFFS failed...");
			return;
		}
		else
		{
			if (SPIFFS.begin() == false)
			{
				Serial.println("SPIFFS failed...giving up");
				return;
			}
		}
	}

	FSInfo fsinfo;
	if (SPIFFS.info(fsinfo) == false)
	{
		Serial.println("Error. NO SPIFFS Info.");
		return;
	}

	if (!SPIFFS.exists("/EEPROM.TXT") && memorysize > fsinfo.totalBytes)
	{
		Serial.println("Not enough SPIFFS memory");
		return;
	}
	else
	{
		////Serial.print("Selected FlashChipSize: "); Serial.println(ESP.getFlashChipSize());
		////Serial.print("FlashChipRealSize: "); Serial.println(ESP.getFlashChipRealSize());
		////Serial.print("SketchSize:"); Serial.println(ESP.getSketchSize());
		////Serial.print("FreeSketchSpace:"); Serial.println(ESP.getFreeSketchSpace());
		////Serial.print("FreeHeap:"); Serial.println(ESP.getFreeHeap());
		////Serial.print("SPIFFS Bytes Total ");
		////Serial.println(fsinfo.totalBytes);
		////Serial.print("SPIFFS Bytes Used  ");
		////Serial.println(fsinfo.usedBytes);
		////Serial.print("SPIFFS Bytes Requ. ");
		////Serial.println(memorysize);
	}

	File fs = SPIFFS.open("/EEPROM.TXT", doit == true ? "w+" : "r+");
	if (!fs)
	{
		fs = SPIFFS.open("/EEPROM.TXT", "w+");
		if (!fs)
		{
			Serial.println("SPIFFS unable to open storage");
			return;
		}
	}

	if (fs.size() != memorysize)
	{
		Serial.println("Filesize failed");
		goto InitializeSpiffs;
	}


	if (fs.seek(memorysize - 1, SeekSet) == false)
	{
		Serial.println("Seek memsize failed.");
		goto InitializeSpiffs;
	}


	if (fs.seek(0, SeekSet) == false)
	{
		Serial.println("Seek start failed");
		goto InitializeSpiffs;
	}

	if (fs.seek(0, SeekSet) == false)
	{
		Serial.println("Seek start failed");
		goto InitializeSpiffs;
	}

	if (fs.read() != 'N')
	{
		Serial.println("N check failed");
		goto InitializeSpiffs;
	}

	if (fs.read() != 'H')
	{
		Serial.println("H check failed");
		goto InitializeSpiffs;
	}

	if (fs.read() != N_DIPSWITCHES)
	{
		Serial.println("N_DIPSWITCH failed");
		goto InitializeSpiffs;
	}

	if (fs.read() != dipswitchsize)
	{
		Serial.println("sizeof check failed");
		goto InitializeSpiffs;
	}

	goto SkipInitializeSpiffs;

InitializeSpiffs:

	Serial.println("Initializing file contents");
	fs.seek(0, SeekSet);
	fs.write('N');
	fs.write('H');
	fs.write(N_DIPSWITCHES);
	fs.write(dipswitchsize);
	for (uint cou = 0; cou < memorysize - 4; cou++)
	{
		fs.write(0);
	}

SkipInitializeSpiffs:

	if (!doit)
	{
		int eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct));
		fs.seek(eeprompos, SeekSet);

		uint8_t *bytepointer;
		bytepointer = (uint8_t*)ssid;
		for (int cou = 0; cou < N_CHAR_SSID; cou++)
		{
			*bytepointer = fs.read();
			bytepointer++;
		}

		eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct)) + N_CHAR_SSID;
		fs.seek(eeprompos);
		bytepointer = (uint8_t*)password;
		for (int cou = 0; cou < N_CHAR_SSID; cou++)
		{
			*bytepointer = fs.read();
			bytepointer++;
		}
	}
	fs.flush();
	fs.close();
#endif // !USE_SPIFFS

}
void EStore::dipSwitchLoad(int no, dipswitches_struct *dswitch)
{
	uint8_t *bytepointer;
	bytepointer = (uint8_t*)dswitch;
	int eeprompos = 4 + (no * sizeof(dipswitches_struct));

#ifndef USE_SPIFFS
	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		*bytepointer = EEPROM.read(eeprompos + cou);
		bytepointer++;
	}

	return;
#else

	File fs = SPIFFS.open("/EEPROM.TXT", "r");
	if (!fs)
	{
		Serial.println("SPIFFS unable to open storage");
		return;
	}

	if (fs.seek(eeprompos, SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return;
	}

	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		*bytepointer = fs.read();
		bytepointer++;
	}

	fs.close();

#endif // !USE_SPIFFS

}

void EStore::dipSwitchDelete(int no)
{
#ifndef USE_SPIFFS
	EEPROM.write(4 + (no * sizeof(dipswitches_struct)), 0);
	EEPROM.commit();
	Serial.println("switch removed from EEPROM");
#else

	File fs = SPIFFS.open("/EEPROM.TXT", "r+");
	if (!fs)
	{
		Serial.println("SPIFFS unable to open storage");
		return;
	}

	if (fs.seek(4 + (no * sizeof(dipswitches_struct)), SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return;
	}

	fs.write(0);
	fs.flush();
	fs.close();

#endif // !USE_SPIFFS

}

void EStore::wifiSave(bool format)
{
	uint32_t eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t *)ssid;

#ifndef USE_SPIFFS

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
		bytepointer++;
	}

	EEPROM.commit();
#else
	if (format == true)
	{
		SPIFFS.end();
		EStore::setupEeprom(true);
	}


	File fs = SPIFFS.open("/EEPROM.TXT", "r+");
	if (!fs)
	{
		Serial.println("SPIFFS unable to open storage");
		return;
	}

	if (fs.seek(eeprompos, SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return;
	}

	for (int cou = 0; cou < N_CHAR_SSID; cou++)
	{
		fs.write(*bytepointer);
		bytepointer++;
	}

	eeprompos = 4 + (N_DIPSWITCHES * sizeof(dipswitches_struct)) + N_CHAR_SSID;
	bytepointer = (uint8_t *)password;

	if (fs.seek(eeprompos, SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return;
	}

	for (int cou = 0; cou < N_CHAR_PASSWORD; cou++)
	{
		fs.write(*bytepointer);
		bytepointer++;
	}

	fs.flush();
	fs.close();

#endif


}

void EStore::dipSwitchSave(int no, dipswitches_struct *dswitch)
{
	int eeprompos = 4 + (no * sizeof(dipswitches_struct));
	uint8_t *bytepointer;
	bytepointer = (uint8_t *)dswitch;

#ifndef USE_SPIFFS

	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		EEPROM.write(eeprompos++, *bytepointer);
		bytepointer++;
	}
	EEPROM.commit();
#else
	File fs = SPIFFS.open("/EEPROM.TXT", "r+");
	if (!fs)
	{
		Serial.println("SPIFFS unable to open storage");
		return;
	}

	if (fs.seek(eeprompos, SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return;
	}

	for (uint cou = 0; cou < sizeof(dipswitches_struct); cou++)
	{
		fs.write(*bytepointer);
		bytepointer++;
	}

	fs.flush();
	fs.close();


#endif
}

int EStore::dipSwitchFindFree(void)
{
	int eeprompos = 4;
#ifndef USE_SPIFFS

	for (int cou = 0; cou < N_DIPSWITCHES; cou++)
	{
		if (EEPROM.read(eeprompos) == 0)
		{
			return cou;
		}

		eeprompos += sizeof(dipswitches_struct);

	}
#else
	File fs = SPIFFS.open("/EEPROM.TXT", "r");
	if (!fs)
	{
		Serial.println("SPIFFS unable to open storage");
		return -1;
	}

	if (fs.seek(eeprompos, SeekSet) == false)
	{
		Serial.println("Positioning failed");
		return -1;
	}

	for (int cou = 0; cou < N_DIPSWITCHES; cou++)
	{
		if (fs.read() == 0)
		{
			fs.close();
			return cou;
		}

		eeprompos += sizeof(dipswitches_struct);
		if (fs.seek(eeprompos, SeekSet) == false)
		{
			Serial.println("Positioning failed");
			return -1;
		}

	}
	fs.close();

#endif // !USE_SPIFFS

	return -1;
}