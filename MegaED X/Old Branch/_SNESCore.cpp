#include "MegaED X.h"

void _SnesCore::Init()
{
	if (vram != NULL) free(vram);
	vram = (LPBYTE)malloc(0x8000);
}
void _SnesCore::SetSNESRom()
{
	if (romSize & 0x200)
	{
		bRom += 0x200;
		wRom += 0x100;
		dwRom += 0x80;
	}
}

DWORD _SnesCore::snes2pc(int snesAddress)
{
	snesAddress &= 0xFFFFFF;
    if (snesAddress >= 0x800000)
        snesAddress -= 0x800000;

    if (snesAddress / 0x10000 % 2 != 0)
        snesAddress -= 0x10000;
    else
        snesAddress -= 0x8000;

    return (snesAddress / 0x10000) * 0x8000 + (snesAddress % 0x10000);
}
DWORD _SnesCore::pc2snes(int pcAddress)
{
    pcAddress = ((pcAddress / 0x10000)) * 0x20000 + (pcAddress % 0x10000);

    if ((pcAddress % 0x10000) >= 0x8000)
        pcAddress += 0x10000;
    else
        pcAddress += 0x8000;

    return ((pcAddress / 0x10000) * 0x10000) + (pcAddress % 0x10000) + 0x800000;
}

WORD _SnesCore::Convert16Color(WORD c)
{
	return ((c&0x1F)<<10) | (c&0x3E0) | ((c>>10)&0x1F);
}
WORD _SnesCore::Get16Color(int pointer, int index)
{
	WORD c = wRom[(pointer>>1) + index];
	return ((c&0x1F)<<10) | (c&0x3E0) | ((c>>10)&0x1F);
}
DWORD _SnesCore::GetRGBColor(int pointer, int index)
{
	WORD c = wRom[(pointer>>1) + index];
	return ((c&0x1F)<<3) | ((c&0x3E0)<<6) | ((c&0x7C00)<<9);
}
DWORD _SnesCore::GetBGRColor(int pointer, int index)
{
	WORD c = wRom[(pointer>>1) + index];
	return ((c&0x1F)<<19) | ((c&0x3E0)<<6) | ((c&0x7C00)>>7);
}
RGBQUAD _SnesCore::GetRGBQuad(int pointer, int index)
{
	DWORD c = wRom[(pointer+(index<<1))>>1];
	RGBQUAD ret;
	ret.rgbRed = (c%0x20)<<3;
	ret.rgbGreen = ((c>>5)%0x20)<<11;
	ret.rgbBlue = ((c>>10)%0x20)<<19;
	return ret;
}

void _SnesCore::tile2raw4bpp(BYTE raw[], BYTE spr[])
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
			*spr = 0;
            *spr++ = raw[y << 1] >> (~x&7) & 1
                | (raw[y * 2 + 1] >> (~x&7) << 1) & 2
                | (raw[y * 2 + 16] >> (~x&7) << 2) & 4
                | (raw[y * 2 + 17] >> (~x&7) << 3) & 8;
        }
}
void _SnesCore::tile2raw2bpp(BYTE raw[], BYTE spr[]) // UNTESTED
{
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
        {
			*spr = 0;
            *spr++ = raw[y << 1] >> (~x&7) & 1
                | (raw[y * 2 + 1] >> (~x&7) << 1) & 2;
        }
}

void _SnesCore::RenderTile4bpp(int index, LPWORD imagedata)
{
	int pal = (index>>6)&0x70;

	BYTE sprraw[0x20];
	BYTE spr[0x40];
	memcpy(sprraw, vram + ((index&0x3FF)<<5), 0x20);
	tile2raw4bpp(sprraw, spr);

	for(int i=0; i<0x40; i++)
	{
		*imagedata++ = palCache[spr[i] | pal];
	}
}

void _SnesCore::RenderTile2bpp(int index, LPWORD imagedata)
{
	int pal = (index>>6)&0x70;

	BYTE sprraw[0x10];
	BYTE spr[0x40];
	memcpy(sprraw, vram + ((index&0x3FF)<<4), 0x10);
	tile2raw2bpp(sprraw, spr);

	for(int i=0; i<0x40; i++)
	{
		*imagedata++ = palCache[spr[i] | pal];
	}
}
