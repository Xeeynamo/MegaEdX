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

#pragma pack(push, 1)
// ABCDDEEFFGGHHIIJJKKLLMMNNOOPQ
struct CheckPointInfo
{
	BYTE objLoad;
	BYTE tileLoad;
	BYTE palLoad;
	WORD chX;
	WORD chY;
	WORD camX;
	WORD camY;
	WORD bkgX;
	WORD bkgY;
	WORD minX;
	WORD minY;
	WORD maxX;
	WORD maxY;
	WORD n;
	WORD o;
	BYTE p;
	BYTE telDwn;
};
#pragma pack(pop)

class MMXCore : public SNESCore
{
private:
public:
	BYTE type;
	BYTE vram[0x8000];
	WORD mapping[(0xA600 - 0x2000)/2];
	BYTE mappingBG[0xE800 - 0xA600];
	BYTE sceneLayout[0x400];

	// Palettes relative
	DWORD palettesOffset[8];
	// Tiles relative
	DWORD tileCmpPos;
	WORD  tileCmpDest;
	WORD  tileCmpSize;
	WORD  tileCmpRealSize;
	DWORD tileDecPos;
	WORD  tileDecDest;
	WORD  tileDecSize;

	CheckPointInfo *checkpointInfo;
	WORD level, point;
	BYTE levelWidth, levelHeight, sceneUsed;
	DWORD pPalette, pLayout, pScenes, pBlocks, pMaps, pCollisions;

	BYTE CheckROM();

	// FONT
	WORD fontPalCache[0x20];
	BYTE fontCache[0x4400];
	DWORD GetFontPointer();
	void LoadFont();
	void LoadFont(int);

	// OLD
	DWORD GetCheckPointPointer();

	void LoadGFXs();
	void LoadTiles();
	void SetLevel(WORD, WORD);
	void LoadLevel();
	void LoadBackground();
	void LoadTilesAndPalettes();

	void LoadPaletteDynamic();
	void LoadLevelLayout();
	void ReallocScene(BYTE);
	void LoadLayout();
	void SwitchLevelEvent(bool);
};