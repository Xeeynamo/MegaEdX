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

bool dTileTextWrite=0;
BYTE dEditorPaletteSelect=0;
BYTE dTilePaletteN=0, dTilePaletteNOld=1;
WORD dTileSelected=0, dTileOverMouse=0;

HBITMAP backBufferTilesProc, backBufferTilesEdit;
BOOL CALLBACK TilesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC;
	HDC hBackDC;
	int wmId, wmEvent;

	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hBackDC = CreateCompatibleDC(hDC);
		SelectObject(hBackDC, backBufferTilesProc);
	
		for(int i=0; i<0x400; i++)
			render.RenderTile(hBackDC, i % 0x20, i >> 5, TILE(i, dTilePaletteN, 0, 0, 0)));

		BitBlt(hDC, 0, 0, 256, 256, hBackDC, 0, 0, SRCCOPY);
		
		SelectObject(hBackDC, backBufferTilesEdit);
		render.RenderTile(hBackDC, 0, 0, TILE(dTileSelected, dTilePaletteN, 0, 0, 0)));
		StretchBlt(hBackDC, 0, 0, 64, 64, hBackDC, 0, 0, 8, 8, SRCCOPY);
		BitBlt(hDC, 272, 80, 64, 64, hBackDC, 0, 0, SRCCOPY);

		HBRUSH brush;
		for(int y=0; y<4; y++)
			for(int x=0; x<4; x++)
			{
				brush = CreateSolidBrush(nmmx.ConvertBGRColor(nmmx.palCache[x | (y<<2) | (dTilePaletteN<<4)]));
				DeleteObject(SelectObject(hDC, brush));
				Rectangle(hDC, 270 + (x<<4), 160 + (y<<4), 270 + 18 + (x<<4), 160 + 18 + (y<<4));
			}
		DeleteObject(brush);
		DeleteDC(hBackDC);

		EndPaint(hWnd, &ps);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);

		switch (wmId)
		{
		case IDC_PALETTEN:
			dTilePaletteN = GetDlgItemInt(hWnd, IDC_PALETTEN, NULL, false);
			if (dTilePaletteN != dTilePaletteNOld)
			{
				dTilePaletteNOld = dTilePaletteN;
				InvalidateRect(hWnd, NULL, false);
			}
			break;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			dTileTextWrite = false;
			dTileSelected = dTileOverMouse;

PAINT_TILE:
			SHORT x = (SHORT)lParam;
			SHORT y = (SHORT)(lParam >> 16);
			if (x>=272 && x<=272+64 && y>=80 && y<=80+64)
			{
				x = (x-272)/8;
				y = (y- 80)/8;
				*(LPBYTE)(nmmx.vramCache + ((dTileSelected & 0x3FF)<<6) + x + (y*8)) = dEditorPaletteSelect;
			}
			if (x>=270 && x<=270+64 && y>=160 && y<=160+64)
			{
				x = (x-270)/16;
				y = (y-160)/16;
				dEditorPaletteSelect = x + (y*4);
			}

			RECT rect;
			rect.top = 80;
			rect.left = 272;
			rect.bottom = 80 + 64;
			rect.right = 272 + 64;
			InvalidateRect(hWnd, &rect, false);
		}
		break;
	case WM_MOUSEMOVE:
		if (wParam != 0)
			goto PAINT_TILE;
		if ((lParam & 0xFFFF) < 0x0100)
		{
			dTileOverMouse = ((lParam>>3)&0x1F) | ((lParam>>14)&0x3E0);
			goto PRINTEXT;
		}
		else
		{
			if (dTileOverMouse == dTileSelected)
				break;
			dTileOverMouse = dTileSelected;
			goto PRINTEXT;
		}
		if (!dTileTextWrite)
		{
PRINTEXT:
			dTileTextWrite = false;
			dTileTextWrite = true;
			CHAR sIndex[12];
			sprintf_s(sIndex, "Tile N. %03X", dTileOverMouse);
			SetWindowText(GetDlgItem(hWnd, IDC_LTILEINDEX), sIndex);
			if (dTileOverMouse < 0x10)
				SetWindowText(GetDlgItem(hWnd, IDC_LTILEPROP), "Locked in ASM");
			else if (dTileOverMouse >= (nmmx.tileCmpDest>>5) && dTileOverMouse < ((nmmx.tileCmpDest + nmmx.tileCmpSize)>>5))
				SetWindowText(GetDlgItem(hWnd, IDC_LTILEPROP), "Compressed");
			else if (dTileOverMouse >= (nmmx.tileDecDest>>5) && dTileOverMouse < ((nmmx.tileDecDest + nmmx.tileDecSize)>>5))
				SetWindowText(GetDlgItem(hWnd, IDC_LTILEPROP), "Uncompressed");
			else
				SetWindowText(GetDlgItem(hWnd, IDC_LTILEPROP), "Unknow");
		}
		break;
	case WM_MOVE:
		RECT rc;
		GetWindowRect(hWnd, &rc);
		set.tilED.X = (SHORT)rc.left;
		set.tilED.Y = (SHORT)rc.top;
	case WM_SHOWWINDOW:
	{
		hWID[2] = hWnd;
		SetWindowPosition(hWnd, set.tilED.X, set.tilED.Y);
		HDC tmpDC = GetDC(hWnd);
		backBufferTilesProc = CreateCompatibleBitmap(tmpDC, 256, 256);
		backBufferTilesEdit = CreateCompatibleBitmap(tmpDC, 64, 64);
		DeleteDC(tmpDC);
		SendMessage(GetDlgItem(hWnd, IDC_PALETTES), UDM_SETRANGE, 0, 7);
		break;
	}
	case WM_CLOSE:
		DeleteObject(backBufferTilesProc);
		EndDialog(hWnd, 0);
		render.RefreshMapCache();
		RepaintAll();
		break;
	}
	return 0; 
}