#include "MegaED X.h"

int pal;
const BITMAPINFO __bmpInfoX = {sizeof(BITMAPINFOHEADER), 8, -8, 1, 16, BI_RGB, 0, 0, 0, 0, 0};
MegaText::~MegaText()
{
	DeleteObject(bmpText);
}
void MegaText::Init(_MMXCore *parammmxc)
{
	mmxc = parammmxc;
	BYTE textCache[0x1000];
	void *text = mmxc->GFXRLE(mmxc->GetFontPointer(), 0x1000);
	memcpy(textCache, text, 0x1000);
	for(int i=0; i<0x20; i++) // Decompress the 32 colors
		palCache[i] = ((_SnesCore*)(mmxc))->Get16Color(0x2D140, i);
	for(int i=0; i<0x100; i++) // Decompress all 256 tiles in ram
		mmxc->tile2raw2bpp(textCache + (i<<4), vtext + (i<<6) + 0x400);
	free(text);
	bmpText = CreateDIBSection(NULL, &__bmpInfoX, 0, &bmpData, 0, 0);
}
void MegaText::RenderTile2bpp(int index, LPWORD imagedata)
{
	LPBYTE spr = vtext + (index << 6);
	for(int i=0; i<0x40; i++)
	{
		*imagedata++ = palCache[spr[i] | pal];
	}
}
void MegaText::PrintText(HDC hDC, LPCSTR text, int x, int y, int palette)
{
	pal = palette << 2;
	size_t length = strlen(text);
	HDC dcText = CreateCompatibleDC(hDC);
	SelectObject(dcText, bmpText);
	
	for(size_t i=0; i<length; i++)
	{
		RenderTile2bpp(*text++, (LPWORD)bmpData);
		BitBlt(hDC, (i<<3) + (x<<3), y<<3, 8, 8, dcText, 0, 0, SRCCOPY);
	}
	DeleteDC(dcText);
}