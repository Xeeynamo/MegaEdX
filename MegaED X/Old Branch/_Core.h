#include "stdafx.h"
#include "MegaED X.h"

class Core
{
	// protected
public:
	LPBYTE bRom;
	LPWORD wRom;
	LPDWORD dwRom;
	DWORD romSize;
	LPBYTE vram;

	STRING filePath;

	// CPU-Based
	void LoadRom(void *rom);
	void FreeMemory();

	// Graphic
	WORD rgb216(DWORD rgb);
};