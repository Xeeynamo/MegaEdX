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

const unsigned char hexCharIndex[0x10] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46};
const BYTE _bigTextAlloc[] = {
	0x80, 0x90, // A
	0x81, 0x91, // B
	0x82, 0x92, // C
	0x83, 0x93, // D
	0x84, 0x94, // E
	0x84, 0xB5, // F
	0x85, 0x95, // G
	0x86, 0x96, // H
	0x87, 0x97, // I
	0x00, 0x00, // J ***
	0x88, 0x98, // K
	0x89, 0x99, // L
	0x8A, 0x9A, // M
	0x8B, 0x9B, // N
	0x8C, 0x9C, // O
	0x8D, 0x9D, // P
	0x00, 0x00, // Q ***
	0x8E, 0x9E, // R
	0x8F, 0x9F, // S
	0xA0, 0x97, // T
	0xA1, 0xB1, // U
	0x00, 0x00, // V ***
	0xA2, 0xB2, // W
	0x00, 0x00, // X ***
	0xA3, 0xB3, // Y
	0x00, 0x00, // Z ***
};

const BITMAPINFO _bmpInfo = {sizeof(BITMAPINFOHEADER), 8, -8, 1, 16, BI_RGB, 0, 0, 0, 0, 0};
RenderED::~RenderED()
{
	Destroy();
}
void RenderED::Init(MMXCore *mmxParam)
{
	Destroy();
	pmmx = mmxParam;
	void *xdata;
	hBmpTile = CreateDIBSection(NULL, &_bmpInfo, 0, &xdata, 0, 0);
	data = (LPWORD)xdata;
}
void RenderED::Destroy()
{
	pmmx = NULL;
	if (hBmpTile != NULL) DeleteObject(hBmpTile);
	if (dcWork != NULL) DeleteDC(dcWork);
	dcWork   = NULL;
	hBmpTile = NULL;
	data     = NULL;
}
void RenderED::CreateMapCache(HWND hWND)
{
	hWnd = hWND;
	HDC dcMain = GetDC(hWnd);
	hBmpMaps = CreateCompatibleBitmap(dcMain, 0x400 * 16, 16);
	dcWork = CreateCompatibleDC(dcMain);
	SelectObject(dcWork, hBmpMaps);
	RefreshMapCache();
	ReleaseDC(hWnd, dcMain);
}
void RenderED::RefreshMapCache()
{
	LPWORD map = (LPWORD)(pmmx->rom + pmmx->pMaps);
	for(int i=0; i<0x400; i++)
	{
		RenderTile(dcWork, i*2 + 0, 0, *map++);
		RenderTile(dcWork, i*2 + 1, 0, *map++);
		RenderTile(dcWork, i*2 + 0, 1, *map++);
		RenderTile(dcWork, i*2 + 1, 1, *map++);
	}
}
void RenderED::RefreshMapCache(WORD index)
{
	LPWORD map = (LPWORD)(pmmx->rom + pmmx->pMaps) + (index*4);
	index <<= 1;
	RenderTile(dcWork, index + 0, 0, *map++);
	RenderTile(dcWork, index + 1, 0, *map++);
	RenderTile(dcWork, index + 0, 1, *map++);
	RenderTile(dcWork, index + 1, 1, *map++);
}
void RenderED::RenderTile(HDC hdc, int x, int y, WORD tile)
{
	LPWORD xdata = data;
	HDC dcBmp = CreateCompatibleDC(hdc);
	BYTE palette = (tile >> 6) & 0x70;
	LPBYTE image = pmmx->vramCache + ((tile & 0x3FF)<<6);
	for(int i=0; i<0x40; i++)
		*xdata++ = pmmx->palCache[*image++ | palette];
	SelectObject(dcBmp, hBmpTile);
	if (tile & 0x4000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 7, 0, -8, 8, SRCCOPY);
	if (tile & 0x8000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 0, 7, 8, -8, SRCCOPY);
	BitBlt(hdc, x<<3, y<<3, 8, 8, dcBmp, 0, 0, SRCCOPY);
	DeleteDC(dcBmp);
}
void RenderED::RenderMap(HDC hdc, int x, int y, WORD index)
{
	BitBlt(hdc, x<<4, y<<4, 16, 16, dcWork, index<<4, 0, SRCCOPY);
}
void RenderED::RenderBlock(HDC hdc, int x, int y, WORD index)
{
	LPWORD pmap = (LPWORD)(pmmx->rom + pmmx->pBlocks) + index*4;
	x <<= 1;
	y <<= 1;
	RenderMap(hdc, x + 0, y + 0, *pmap++);
	RenderMap(hdc, x + 1, y + 0, *pmap++);
	RenderMap(hdc, x + 0, y + 1, *pmap++);
	RenderMap(hdc, x + 1, y + 1, *pmap++);
}
void RenderED::RenderScene(HDC hdc, int x, int y, WORD index)
{
	x <<= 3;
	y <<= 3;
	LPWORD pblock = (LPWORD)(pmmx->rom + pmmx->pScenes) + (index<<6);
	for(int iy=0; iy<8; iy++)
		for(int ix=0; ix<8; ix++)
			RenderBlock(hdc, x + ix, y + iy, *pblock++);
}
void RenderED::RenderSceneEx(HDC hdc, int x, int y, WORD index)
{
	x <<= 4;
	y <<= 4;
	LPWORD pmap = pmmx->mapping + (index<<8);
	for(int iy=0; iy<16; iy++)
		for(int ix=0; ix<16; ix++)
			RenderMap(hdc, x + ix, y + iy, *pmap++);
}
void RenderED::ShowCollisionIndex(HDC hdc, int x, int y, WORD index)
{
	x <<= 4;
	y <<= 4;
	LPWORD pmap = pmmx->mapping + (index<<8);
	LPBYTE collision = (LPBYTE)(nmmx.rom + nmmx.pCollisions);
	for(int iy=0; iy<16; iy++)
		for(int ix=0; ix<16; ix++, pmap++)
			if (collision[*pmap])
				render.PutH(hdc, (ix + x)*2, (iy + y)*2, collision[*pmap], (ix&1) + 5);
}

void RenderED::CreateFontCache(HWND hWND)
{
	HDC dcMain = GetDC(hWnd);
	hFont = CreateCompatibleBitmap(dcMain, 0x100 * 8, 64);
	dcFont = CreateCompatibleDC(dcMain);
	SelectObject(dcFont, hFont);
	RefreshFontCache();
	ReleaseDC(hWnd, dcMain);
}
void RenderED::RefreshFontCache()
{
	for(int c=0; c<8; c++)
	{
		for(int i=0; i<0x100; i++)
		{
			LPWORD xdata = data;
			HDC dcBmp = CreateCompatibleDC(dcWork);
			BYTE palette = c<<2;
			LPBYTE image = pmmx->fontCache + ((i & 0x3FF)<<6);
			for(int d=0; d<0x40; d++)
				*xdata++ = pmmx->fontPalCache[*image++ | palette];
			SelectObject(dcBmp, hBmpTile);
			if (i & 0x4000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 7, 0, -8, 8, SRCCOPY);
			if (i & 0x8000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 0, 7, 8, -8, SRCCOPY);
			BitBlt(dcFont, i*8, c*8, 8, 8, dcBmp, 0, 0, SRCCOPY);
			DeleteDC(dcBmp);
		}
	}
}
void RenderED::PutC(HDC hdc, int x, int y, unsigned char ch, char color)
{
	TransparentBlt(hdc, x*8, y*8, 8, 8, dcFont, ch * 8, color*8, 8, 8, 0);
}
void RenderED::PutH(HDC hdc, int x, int y, unsigned char hx, char color)
{
	TransparentBlt(hdc, x*8 + 0, y*8, 8, 8, dcFont, hexCharIndex[hx>>4 ] * 8, color*8, 8, 8, 0);
	TransparentBlt(hdc, x*8 + 8, y*8, 8, 8, dcFont, hexCharIndex[hx&0xF] * 8, color*8, 8, 8, 0);
}
void RenderED::Print(HDC hdc, int x, int y, LPCSTR text, char color)
{
	int len = (int)strlen(text);
	for(int i=0; i<len; i++)
		TransparentBlt(hdc, (x+i)*8, y*8, 8, 8, dcFont, *text++ * 8, color*8, 8, 8, 0);
}
void RenderED::PrintBig(HDC hdc, int x, int y, LPCSTR text, char color)
{
	int len = (int)strlen(text);
	for(int i=0; i<len; i++, text++)
	{
		if (*text != 0x20)
		{
			TransparentBlt(hdc, (x+i)*8, y*8 + 0, 8, 8, dcFont, _bigTextAlloc[(*text-0x41)*2 + 0] * 8, color*8, 8, 8, 0);
			TransparentBlt(hdc, (x+i)*8, y*8 + 8, 8, 8, dcFont, _bigTextAlloc[(*text-0x41)*2 + 1] * 8, color*8, 8, 8, 0);
		}
		else
		{
			TransparentBlt(hdc, (x+i)*8, y*8 + 0, 8, 8, dcFont, ' ' * 8, color*8, 8, 8, 0);
			TransparentBlt(hdc, (x+i)*8, y*8 + 8, 8, 8, dcFont, ' ' * 8, color*8, 8, 8, 0);
		}
	}
}
void RenderED::PrintCopyright(HDC hdc, int x, int y, char color)
{
	BitBlt(hdc, x*8 + 0, y*8 + 0, 8, 8, dcFont, 0x7B *8, color*8, SRCCOPY);
	BitBlt(hdc, x*8 + 8, y*8 + 0, 8, 8, dcFont, 0x7C *8, color*8, SRCCOPY);
	BitBlt(hdc, x*8 + 0, y*8 + 8, 8, 8, dcFont, 0x7D *8, color*8, SRCCOPY);
	BitBlt(hdc, x*8 + 8, y*8 + 8, 8, 8, dcFont, 0x7E *8, color*8, SRCCOPY);
}
