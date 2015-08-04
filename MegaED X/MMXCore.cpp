/*
MegaED X - Megaman X SNES level editor
Copyright (C) 2015  Luciano Ciccariello (Xeeynamo)
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "MegaED X.h"

#define ReadWord(offset) (*((WORD*)(rom+offset)))
#define ReadDWord(offset) (*((DWORD*)(rom+offset)))
const long p_layout[3] = {0x868D24, 0x868888, 0x8689B3};
const long p_scenes[3] = {0x868D93, 0x8688F7, 0x868A22};
const long p_blocks[3] = {0x868E02, 0x868966, 0x868A91};
const long p_maps  [3] = {0x868E71, 0x8689D5, 0x8609C5};
const long p_collis[3] = {0x868EE0, 0x868A44, 0x8609C2};
const long p_checkp[3] = {0x86A780, 0x86A4C5, 0x86A8E4};
const long p_palett[3] = {0x868133, 0x86817A, 0x868180};
const long p_font  [3] = {0x86F744, 0x86FA4D, 0x86F742}; // ...Maybe
const long p_unknow[3] = {0x86A1D5,     NULL,     NULL}; // Unknow
const long p_gfxcfg[3] = {0x86F56F,     NULL,	  NULL};
const long p_gfxpos[3] = {0x86F6F7,     NULL,	  NULL};


BYTE MMXCore::CheckROM()
{
	LoadHeader();
	if (*(LPDWORD)(header->title+0) == 0x4147454D)
		if (*(LPDWORD)(header->title+4) == 0x204E414D)
		{
			switch(*(LPWORD)(header->title+8))
			{
			case 0x2058:
				type = 0;
				break;
			case 0x3258:
				type = 1;
				break;
			case 0x3358:
				type = 2;
				break;
			default:
				type = 0xFF;
				return 0;
			}
			return type+1;
		}
	type = 0xFF;
	return 0;
}
DWORD MMXCore::GetFontPointer()
{
	return snes2pc(ReadWord(snes2pc(p_font[type])) + 0x9C0000);
}
DWORD MMXCore::GetCheckPointPointer()
{
	return snes2pc(p_checkp[type] + SReadWord(p_checkp[type] + SReadWord(p_checkp[type] + level*2) + point*2));
}

void MMXCore::LoadFont()
{
	BYTE textCache[0x1000];
	GFXRLE(rom, textCache, GetFontPointer(), 0x1000);

	for(int i=0; i<0x20; i++) // Decompress the 32 colors
		fontPalCache[i] = Get16Color(0x2D140 + i*2);
	for(int i=0; i<0x100; i++) // Decompress all 256 tiles in ram
		tile2bpp2raw(textCache + (i*0x10), fontCache + (i*0x40) + 0x400);
	return;
}
void MMXCore::SetLevel(WORD iLevel, WORD iPoint)
{
	level = iLevel;
	point = iPoint;
	checkpointInfo = (CheckPointInfo*)(rom + GetCheckPointPointer());
	tileCmpSize = 0;
	tileDecSize = 0;
}
DWORD ppppp;
void MMXCore::LoadLevel()
{
	LoadTilesAndPalettes();

	WORD pLevel = level*3;
	pLayout     = snes2pc(SReadDWord(p_layout[type] + pLevel));
	pScenes     = snes2pc(SReadDWord(p_scenes[type] + pLevel));
	pBlocks     = snes2pc(SReadDWord(p_blocks[type] + pLevel));
	pMaps       = snes2pc(SReadDWord(p_maps  [type] + pLevel));
	pCollisions = snes2pc(SReadDWord(p_collis[type] + pLevel));
	//ppppp = snes2pc(SReadDWord(p_unknow[type] + pLevel));
	//ppppp = ppppp;

	LoadLayout();
	LoadLevelLayout();
}
void MMXCore::LoadBackground()
{
	WORD pLevel = level*3;
	pLayout     = snes2pc(SReadDWord(0x868F4F + pLevel));
	pScenes     = snes2pc(SReadDWord(0x868FBE + pLevel));
	pBlocks     = snes2pc(SReadDWord(0x86902D + pLevel));
	LoadTilesAndPalettes();
	LoadLayout();
	LoadLevelLayout();
}
void MMXCore::LoadTilesAndPalettes()
{
	// Load Palettes
	DWORD configPointer = snes2pc(SReadWord(p_palett[type] + level*2 + 0x60) | 0x860000);
	BYTE colorsToLoad = rom[configPointer++];
	if (type==2)
		pPalette = snes2pc(ReadWord(configPointer++) | 0x8C0000);
	else
		pPalette = snes2pc(ReadWord(configPointer++) | 0x850000);

	for(int i=0; i<colorsToLoad; i++)
		palCache[i] = Get16Color(pPalette + i*2);
	for(int i=0; i<(colorsToLoad>>4); i++)
		palettesOffset[i] = (DWORD)pPalette + i*0x20;
	
	memcpy(vram, vrambase, 0x200);
	if (type==0) // Only MMX1 is able to do this
	{
		LoadPaletteDynamic();
		LoadGFXs();
		LoadTiles();
	}
	for(int i=0; i<0x400; i++)
		tile4bpp2raw(vram + (i<<5), vramCache + (i<<6));
}

void MMXCore::LoadGFXs()
{
	DWORD pConfigGfx = snes2pc(SReadWord(p_gfxcfg[type] + level*2 + 4) | 0x86 << 16);
	BYTE gfxID = rom[pConfigGfx];
	tileCmpSize = ReadWord(pConfigGfx+1);
	tileCmpDest = (ReadWord(pConfigGfx+3)<<1) - 0x2000;
	tileCmpPos = snes2pc(SReadDWord(p_gfxpos[type] + gfxID * 5 + 2));
	tileCmpRealSize = GFXRLE(rom, vram+tileCmpDest, tileCmpPos, tileCmpSize);
	//GFXRLECmp(vram+tileCmpDest, rom+tileCmpPos, 0, 0x4800);
	//GFXRLE(rom, vram+tileCmpDest, tileCmpPos, tileCmpSize);
}
void MMXCore::LoadTiles()
{
	BYTE tileSelect = checkpointInfo->tileLoad;

	int baseIndex = ReadWord(0x321D5 + level*2) + tileSelect*2;
	int mainIndex = ReadWord(0x321D5 + baseIndex);

	tileDecSize = ReadWord(0x321D5 + mainIndex);
	if (tileDecSize == NULL) return;
	tileDecDest = (ReadWord(0x321D5 + mainIndex + 2)<<1) -0x2000;
	tileDecPos = snes2pc(ReadDWord(0x321D5 + mainIndex + 4) & 0xFFFFFF);
	
	if (tileDecDest + tileDecSize > (WORD)0x8000)
	{
		MessageBox(NULL, "VRAM overflow.", "Error", MB_ICONERROR);
	}
	memcpy(vram+tileDecDest, rom+tileDecPos, tileDecSize);
}
void MMXCore::LoadPaletteDynamic()
{
	WORD iLevel = level & 0xFF;
	BYTE palSelect = checkpointInfo->palLoad;
	int baseIndex = ReadWord(0x32260 + iLevel*2) + palSelect*2;
	int mainIndex = ReadWord(0x32260 + baseIndex);
	int writeTo = 0;
	int colorPointer = 0;

	while(true)
	{
		colorPointer = ReadWord(0x32260 + mainIndex);
		if (colorPointer == 0xFFFF)
			return;
		writeTo = (ReadWord(0x32262 + mainIndex) & 0xFF);
		if (writeTo > 0x7F)
		{
			MessageBox(NULL, "Palette overflow.", "Error", MB_ICONERROR);
			return;
		}

		palettesOffset[writeTo>>4] = snes2pc(colorPointer | 0x850000);
		for(int i=0; i<0x10; i++)
		{
			palCache[writeTo + i] = Convert16Color(ReadWord(snes2pc(0x850000 | colorPointer + i*2)));
		}
		mainIndex += 3;
	}
}
void MMXCore::LoadLevelLayout()
{
	bool step = false;
	unsigned short index = 0;
	WORD writeIndex = 0;

	// Load other things O.o
	//writeIndex = SReadWord(0x868D20 + step*2);
	for(int i=0; i<sceneUsed; i++)
	{
		for(int y=0; y<8; y++)
		{
			for(int x=0; x<8; x++)
			{
				LPWORD takeBlock = (LPWORD)(rom + pScenes + (i*0x80) + x*2 + y*0x10);
				takeBlock = (LPWORD)(rom + pBlocks + *takeBlock * 8);
				
				mapping[writeIndex + 0x00] = *takeBlock++;
				mapping[writeIndex + 0x01] = *takeBlock++;
				mapping[writeIndex + 0x10] = *takeBlock++;
				mapping[writeIndex + 0x11] = *takeBlock++;
				writeIndex += 2;
			}
			writeIndex += 0x10;
		}
	}
}
void MMXCore::ReallocScene(BYTE scene)
{
	WORD writeIndex = SReadWord(0x868D20) + 0x100 * scene;
	for(int y=0; y<8; y++)
	{
		for(int x=0; x<8; x++)
		{
			LPWORD takeBlock = (LPWORD)(rom + pScenes + (scene*0x80) + x*2 + y*0x10);
			takeBlock = (LPWORD)(rom + pBlocks + *takeBlock * 8);
				
			mapping[writeIndex + 0x00] = *takeBlock++;
			mapping[writeIndex + 0x01] = *takeBlock++;
			mapping[writeIndex + 0x10] = *takeBlock++;
			mapping[writeIndex + 0x11] = *takeBlock++;
			writeIndex += 2;
		}
		writeIndex += 0x10;
	}
}
void MMXCore::LoadLayout()
{
	LPBYTE playout = (LPBYTE)(rom + pLayout);
	levelWidth  = *playout++;
	levelHeight = *playout++;
	sceneUsed   = *playout++;

	WORD writeIndex = 0;
	byte ctrl;
	while((ctrl = *playout++) != 0xFF)
	{
		byte buf = *playout++;
		for(int i=0; i<(ctrl & 0x7F); i++)
		{
			sceneLayout[writeIndex++] = buf;
			if ((ctrl & 0x80) == 0)
				buf++;
		}
	}
	return;
}
void MMXCore::SwitchLevelEvent(bool ev)
{
	WORD src, dest;
	switch(level)
	{
	case 2:
		sceneLayout[0x64] = sceneLayout[0xA0];
		sceneLayout[0x65] = sceneLayout[0xA1];
		sceneLayout[0x66] = sceneLayout[0xA2];
		sceneLayout[0x67] = sceneLayout[0xA3];
		
		sceneLayout[0x41] = sceneLayout[0xA4];
		sceneLayout[0x42] = sceneLayout[0xA5];
		sceneLayout[0x43] = sceneLayout[0xA6];
		sceneLayout[0x44] = sceneLayout[0xA7];
		break;
	case 4:
		src  = (WORD)((ReadDWord(0x3C79) & 0xFFFFFF) - 0x7EE800);
		dest = (WORD)((ReadDWord(0x3C7D) & 0xFFFFFF) - 0x7EE800);
		sceneLayout[dest] = sceneLayout[src];
		break;
	case 6:
		src  = (WORD)((ReadDWord(0x3C8B) & 0xFFFFFF) - 0x7EE800);
		dest = (WORD)((ReadDWord(0x3C8F) & 0xFFFFFF) - 0x7EE800);
		sceneLayout[dest-3] = sceneLayout[src-4];
		break;
	}
}