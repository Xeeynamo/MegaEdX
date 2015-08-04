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

#define TILE(index, palette, up, mirror, flip) (index | (palette<<10) | (up<<13) | (mirror<<14) | (flip<<15)
class RenderED
{
private:
	MMXCore *pmmx;
	HWND hWnd;
	HDC dcWork;
	HBITMAP hBmpTile;
	HBITMAP hBmpMaps;
	LPWORD data;

	// FONT
	HDC dcFont;
	HBITMAP hFont;
public:
	~RenderED(); // Automatically call Destroy()
	void Init(MMXCore*); // Initialize Rendering core
	void Destroy(); // Destroy Rendering core (and free memory)

	void CreateMapCache(HWND); // Create a bitmap that store all 0x400 maps for fast renderings
	void RefreshMapCache(); // Refresh the entire bitmap (Slow in loops)
	void RefreshMapCache(WORD mapToRefresh); // Refresh single map

	void RenderTile(HDC, int x, int y, WORD tile); // Selecting a tile with his params, print it to X and Y coordinates
	void RenderMap(HDC, int x, int y, WORD map);
	void RenderBlock(HDC, int x, int y, WORD block);
	void RenderScene(HDC, int x, int y, WORD scene);
	void RenderSceneEx(HDC, int x, int y, WORD scene);
	void ShowCollisionIndex(HDC, int x, int y, WORD scene);

	void CreateFontCache(HWND);
	void RefreshFontCache();
	void PutC(HDC, int x, int y, unsigned char ch, char color);
	void PutH(HDC, int x, int y, unsigned char hx, char color);
	void Print(HDC, int x, int y, LPCSTR text, char color);
	void PrintBig(HDC, int x, int y, LPCSTR text, char color);
	void PrintCopyright(HDC, int x, int y, char color);
};