#include "MegaED X.h"

void Core::LoadRom(void *rom)
{
	bRom = (LPBYTE)rom;
	wRom = (LPWORD)rom;
	dwRom = (LPDWORD)rom;
}
void Core::FreeMemory()
{
	if (bRom != NULL)
	{
		free(bRom);
		bRom = NULL;
	}
}
WORD Core::rgb216(DWORD rgb)
{
	return ((rgb>>3)&0x1F) | ((rgb>>6)&0x3E0) | ((rgb>>9)&0x7C00);
}