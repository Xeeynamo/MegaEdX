#include "stdafx.h"
#include "MegaED X.h"

class _SnesCore : public Core
{
public:
	void Init();
	void SetSNESRom();
	WORD palCache[0x100];

	#define ReadWord(offset) (*((WORD*)(bRom+offset)))
	#define ReadDWord(offset) (*((DWORD*)(bRom+offset)))
    DWORD snes2pc(int snesAddress);
    DWORD pc2snes(int pcAddress);
	
	WORD    Convert16Color (WORD color);
	WORD    Get16Color  (int pointer, int index);
	DWORD   GetRGBColor (int pointer, int index);
	DWORD   GetBGRColor (int pointer, int index);
	RGBQUAD GetRGBQuad  (int pointer, int index);
	
	void tile2raw4bpp(BYTE sprraw[], BYTE spr[]);
	void tile2raw2bpp(BYTE sprraw[], BYTE spr[]);
	void RenderTile4bpp(int index, WORD imagedata[]);
	void RenderTile2bpp(int index, WORD imagedata[]);
};