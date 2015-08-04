#include "MegaED X.h"

LPBYTE _MMXCore::DataRLE(int pointer, LPDWORD size)
{
	BYTE dec[0x10000];
    int writeIndex = 0;

    while (bRom[pointer] != 0xFF)
    {
        int count = bRom[pointer] % 0x80;
        bool increment = bRom[pointer++] >> 7 == 0;

        byte data = bRom[pointer++];
        for (int i = 0; i < count; i++)
        {
            dec[writeIndex++] = data;
            if (increment) data++;
        }
    }
	if (size != NULL) *size = writeIndex;
	LPBYTE data;
	data = (LPBYTE)malloc(writeIndex);
	memcpy(data, dec, writeIndex);
	return data;
}
void* _MMXCore::GFXRLE(int pointer, int size)
{
	LPBYTE dec = (LPBYTE)malloc(size);
	int writeIndex = 0;
	for(int i=0; i<size>>3; i++)
	{
		byte control = bRom[pointer++];
		byte data = bRom[pointer++];
		for(int j=0; j<8; j++)
		{
			if (control & 0x80)
				dec[writeIndex++] = bRom[pointer++];
			else
				dec[writeIndex++] = data;
			control <<= 1;
		}
	}
	return dec;
}

// MMXCORE
DWORD _MMXCore::GetFontPointer()
{
	return snes2pc(ReadWord(snes2pc(0x86F744)) + 0x9C0000);
}
DWORD _MMXCore::GetPalettePointer(BYTE level)
{
	int configurationPointer = snes2pc(ReadWord(0x30133 + level * sizeof(short) + 0x60) | 0x860000);
	int colorsToLoad = bRom[configurationPointer++];
	return snes2pc(ReadWord(configurationPointer) | 0x850000);
}
DWORD _MMXCore::GetGfxPointer(BYTE level)
{
	return snes2pc(ReadWord(0x3756F + level*sizeof(short) + 4) | 0x86 << 16);
}
DWORD _MMXCore::GetTilesPointer(BYTE level)
{
	DWORD asd1 = snes2pc(0x86A780 + (level<<1));
	DWORD asd2 = snes2pc(0x86A780 + ReadWord(asd1));
	return snes2pc(0x86A780 + ReadWord(asd2));
}
DWORD _MMXCore::GetMapsPointer(BYTE level)
{
	return snes2pc(ReadDWord(0x30E71 + level*3));
}
DWORD _MMXCore::GetBlocksPointer(BYTE level)
{
	return snes2pc(ReadDWord(0x30E02 + level*3));
}
DWORD _MMXCore::GetScenesPointer(BYTE level)
{
	return snes2pc(ReadDWord(0x30D93 + level*3));
}
LPBYTE _MMXCore::GetLayoutPointer(BYTE level)
{
	return bRom + snes2pc(ReadDWord(0x30D24 + level*3));
}
LPBYTE _MMXCore::GetCheckPointPointer(BYTE level, BYTE point)
{
	int first = ReadWord(0x32780 + level*2) + point*2;
	int second = ReadWord(0x32780 + first);
	return (LPBYTE)0x32780 + second;
}

void _MMXCore::SetLevel(BYTE level, BYTE point)
{
	iLevel = level;
	iPoint = point;
	checkpointInfo = (CheckPointInfo*)(bRom + (int)GetCheckPointPointer(level, iPoint));
}
void _MMXCore::LoadLevel(BYTE level)
{
	SetLevel(level, 0);
	memset(vram, 0, 0x8000);
	pPalette = GetPalettePointer(level);
	pMaps = GetMapsPointer(level);
	pBlocks = GetBlocksPointer(level);
	pScenes = GetScenesPointer(level);
	pLayout = GetLayoutPointer(level);

	for(int i=0; i<0x100; i++)
		palCache[i] = Get16Color(i);
	
	DWORD pConfigGfx = GetGfxPointer(level);
	BYTE gfxID = bRom[pConfigGfx];
	WORD gfxSize = ReadWord(pConfigGfx+1);
	WORD vramDest = (ReadWord(pConfigGfx+3)<<1) - 0x2000;
	pGfx = snes2pc(ReadDWord(0x376F7 + gfxID * 5 + 2));
	memcpy(vram, vrambase, 0x200);
	void *gfxdec = GFXRLE(pGfx, gfxSize);
	if (vramDest+gfxSize<=0x8000)
		memcpy(vram+vramDest, gfxdec, gfxSize);
	free(gfxdec);
	
	LoadObjects();
	LoadTiles();
	LoadPalette();
	levelWidth  = *pLayout++;
	levelHeight = *pLayout++;
	levelScenes = *pLayout++;
	layoutStruct = DataRLE(snes2pc(ReadDWord(0x30D24 + level*3))+3, NULL);
}
void _MMXCore::LoadObjects()
{
	unsigned short baseIndex = ReadWord(0x32CEE + iLevel*2) + iPoint*2;
	unsigned short mainIndex = ReadWord(0x32CEE + baseIndex);
	unsigned char secondIndex = bRom[0x32CEE + mainIndex];

}
void _MMXCore::LoadTiles()
{
	BYTE tileSelect = checkpointInfo->tileLoad;
	
	int baseIndex = ReadWord(0x321D5 + iLevel*2) + tileSelect*2;
	int mainIndex = ReadWord(0x321D5 + baseIndex);

	WORD tSize = ReadWord(0x321D5 + mainIndex);
	if (tSize == NULL) return;
	WORD tDest = (ReadWord(0x321D5 + mainIndex + 2)<<1) -0x2000;
	DWORD tSource = snes2pc(ReadDWord(0x321D5 + mainIndex + 4) & 0xFFFFFF);
	decTileOffset = tSource;
	decTileIndex = tDest >> 5;
	
	//ZeroMemory(vram + tDest + tSize, 0x8000 - tDest - tSize);
	if (tDest + tSize > (WORD)0x8000)
	{
		MessageBox(NULL, "VRAM overflow.", "MMXCore::LoadTiles()", MB_ICONERROR);
	}
	memcpy(vram+tDest, bRom+tSource, tSize);
}
void _MMXCore::LoadPalette()
{
	BYTE palSelect = checkpointInfo->palLoad;

	if (iPoint == 4)
		iPoint = iPoint; // DUMMY
	if (iPoint == 6)
		iPoint = iPoint; // DUMMY
	int baseIndex = ReadWord(0x32260 + iLevel*2) + palSelect*2;
	int mainIndex = 0;
	int writeTo = 0;
	int colorPointer = 0;
	mainIndex = ReadWord(0x32260 + baseIndex);
	while(true)
	{
		colorPointer = ReadWord(0x32260 + mainIndex);
		if (colorPointer == 0xFFFF)
			return;
		writeTo = (ReadWord(0x32262 + mainIndex) & 0xFF);

		for(int i=0; i<0x10; i++)
		{
			palCache[writeTo + i] = Convert16Color(ReadWord(snes2pc(0x850000 | colorPointer + i*2)));
		}
		mainIndex += 3;
	}
}