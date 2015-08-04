#include "stdafx.h"
#include "MegaED X.h"

/*#pragma pack(push, 1)
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
#pragma pack(pop)*/

class _MMXCore : public _SnesCore
{
	BYTE vtext[0x4000];
public:
	DWORD pGfx, pPalette, pTiles, pMaps, pBlocks, pScenes;
	LPBYTE pLayout;
	LPBYTE layoutStruct;
	CheckPointInfo *checkpointInfo;

	// Derivate
	WORD    Get16Color  (int index) { return _SnesCore::Get16Color  (pPalette, index); }
	DWORD   GetRGBColor (int index) { return _SnesCore::GetRGBColor (pPalette, index); }
	DWORD   GetBGRColor (int index) { return _SnesCore::GetBGRColor (pPalette, index); }
	RGBQUAD GetRGBQuad  (int index) { return _SnesCore::GetRGBQuad  (pPalette, index); }
	
	LPBYTE DataRLE(int pointer, LPDWORD size);
	void *GFXRLE(int pointer, int size);
	
	DWORD decTileOffset;
	WORD decTileIndex;
	BYTE iLevel, iPoint;

	BYTE levelWidth, levelHeight, levelScenes;

	DWORD GetFontPointer();
	DWORD GetPalettePointer(BYTE level);
	DWORD GetGfxPointer(BYTE level);
	DWORD GetTilesPointer(BYTE level);
	DWORD GetMapsPointer(BYTE level);
	DWORD GetBlocksPointer(BYTE level);
	DWORD GetScenesPointer(BYTE level);
	LPBYTE GetLayoutPointer(BYTE level);
	LPBYTE GetCheckPointPointer(BYTE level, BYTE point);

	void SetLevel(BYTE level, BYTE point);
	void LoadLevel(BYTE level);
	void LoadObjects();
	void LoadTiles();
	void LoadPalette();
};