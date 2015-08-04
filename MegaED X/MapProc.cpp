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

#define ID_SP_COLLISION     6000

// Control variables:
ScrollBar mapScroll;
SpinBox mapTilePalette, mapTileIndex, mapCollision;

// General variables:
BYTE selection = 0;
WORD dMapSelected=0;
WORD dMapOverMouse=0;
WORD dMapTextWrite=0;

// Drawing variables:
HDC hMapDC, hMapBack;
HBITMAP backBufferMapProc;

RECT rMapEdit = {288, 32, 288 + 64, 32 + 64};
RECT rectMapTileFocus[] = {288, 32, 288+32, 32+32,
	288+32, 32, 288+64, 32+32,
	288, 32+32, 288+32, 32+64,
	288+32, 32+32, 288+64, 32+64};

void UpdateMapEdit(HWND hWnd, BYTE sel)
{
	WORD map = *(LPWORD)(nmmx.rom + nmmx.pMaps + (dMapSelected<<3) + sel*2);
	mapCollision.SetPos(*(LPBYTE)(nmmx.rom + nmmx.pCollisions + dMapSelected));
	mapTileIndex.SetPos(map & 0x3FF);
	mapTilePalette.SetPos((map >> 10) & 7);
	SendMessage(GetDlgItem(hWnd, IDC_MAPUPLAYER),BM_SETCHECK, map & 0x2000 ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hWnd, IDC_MAPMIRROR), BM_SETCHECK, map & 0x4000 ? BST_CHECKED : BST_UNCHECKED, 0);
	SendMessage(GetDlgItem(hWnd, IDC_MAPFLIP),   BM_SETCHECK, map & 0x8000 ? BST_CHECKED : BST_UNCHECKED, 0);
	InvalidateRect(hWnd, &rMapEdit, false);
}
void UpdateMapWrite(HWND hWnd)
{
	WORD map = 0;
	map |= mapTileIndex.GetPos();
	map |= mapTilePalette.GetPos() << 10;
	map |= SendMessage(GetDlgItem(hWnd, IDC_MAPUPLAYER), BM_GETCHECK, NULL, NULL) == BST_CHECKED ? 0x2000 : 0;
	map |= SendMessage(GetDlgItem(hWnd, IDC_MAPMIRROR),  BM_GETCHECK, NULL, NULL) == BST_CHECKED ? 0x4000 : 0;
	map |= SendMessage(GetDlgItem(hWnd, IDC_MAPFLIP),    BM_GETCHECK, NULL, NULL) == BST_CHECKED ? 0x8000 : 0;
	*(LPBYTE)(nmmx.rom + nmmx.pCollisions + dMapSelected) = mapCollision.GetPos();
	*(LPWORD)(nmmx.rom + nmmx.pMaps + (dMapSelected<<3) + selection*2) = map;
	render.RefreshMapCache(dMapSelected);
	InvalidateRect(hWnd, NULL, false);
}
BOOL CALLBACK MapProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	
	switch (message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_MAPFLIP:
		case IDC_MAPMIRROR:
		case IDC_MAPUPLAYER:
			UpdateMapWrite(hWnd);
			break;
		}
		break;
	case WM_VSCROLL:

		if (mapScroll.IsIDEqual((long)lParam))
		{
			mapScroll.Work(wParam);
			if (LOWORD(wParam) != SB_ENDSCROLL)
			{
				RECT rect;
				rect.top = 0;
				rect.left = 0;
				rect.bottom = 256;
				rect.right = 256;
				InvalidateRect(hWnd, &rect, false);
			}
		}
		else if (mapTileIndex.IsIDEqual((long)lParam))
		{
			mapTileIndex.Work(wParam);
			UpdateMapWrite(hWnd);
		}
		else if (mapTilePalette.IsIDEqual((long)lParam))
		{
			mapTilePalette.Work(wParam);
			UpdateMapWrite(hWnd);
		}
		else if (mapCollision.IsIDEqual((long)lParam))
		{
			mapCollision.Work(wParam);
			UpdateMapWrite(hWnd);
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if ((lParam & 0xFFFF) < 0x0100)
			{
				dMapTextWrite = false;
				dMapSelected = dMapOverMouse;
				InvalidateRect(hWnd, &rMapEdit, false);
			}
			else if (LOWORD(lParam) >= 288 && LOWORD(lParam) < 288+64)
			{
				if (HIWORD(lParam) >= 32 && HIWORD(lParam) < 32+64)
				{
					selection  = LOWORD(lParam) >= 288+32 ? 1 : 0;
					selection |= HIWORD(lParam) >=  32+32 ? 2 : 0;
				}
			}
			UpdateMapEdit(hWnd, selection);
		}
		break;
	case WM_MOUSEMOVE:
		if ((lParam & 0xFFFF) < 0x0100)
		{
			dMapOverMouse = (WORD)(((lParam>>4)&0xF) | ((lParam>>16)&0xF0) + (mapScroll.GetPos()<<4));
			goto PRINTEXT;
		}
		else
		{
			if (dMapOverMouse == dMapSelected)
				break;
			dMapOverMouse = dMapSelected;
			goto PRINTEXT;
		}
		if (false)
		{
PRINTEXT:
			dMapTextWrite = true;
			CHAR sIndex[11];
			wsprintf(sIndex, "Map N. %03X", dMapOverMouse);
			SetWindowText(GetDlgItem(hWnd, IDC_LMAPINDEX), sIndex);
		}
		break;
	case WM_MOVE:
		RECT rc;
		GetWindowRect(hWnd, &rc);
		set.mapED.X = (SHORT)rc.left;
		set.mapED.Y = (SHORT)rc.top;
	case WM_PAINT:
		hMapDC = BeginPaint(hWnd, &ps);
		hMapBack = CreateCompatibleDC(hMapDC);
		SelectObject(hMapBack, backBufferMapProc);
		
		render.RenderMap(hMapDC, 18, 2, dMapSelected);
		StretchBlt(hMapDC, 288, 32, 64, 64, hMapDC, 288, 32, 16, 16, SRCCOPY);

		for(int i=0; i<0x100; i++)
			render.RenderMap(hMapBack, i & 0xF, i>>4, (mapScroll.GetPos()<<4) + i);
		
		BitBlt(hMapDC, 0, 0, 256, 256, hMapBack, 0, 0, SRCCOPY);
		DeleteDC(hMapBack);
		DrawFocusRect(hMapDC, rectMapTileFocus + selection);
		EndPaint(hWnd, &ps);
		break;
	case WM_INITDIALOG:
	{
		hWID[3] = hWnd;
		backBufferMapProc = CreateBitmapCache(hWnd, 256, 256);
		mapScroll.Create(hWnd, 257, 0, 256);
		mapScroll.SetRange(0, 0x30);
		mapCollision.Create  (hWnd, ID_SP_COLLISION,     300, 100, 62, 0, 0xFF);
		mapTileIndex.Create  (hWnd, 0x9001,   300, 150, 62, 0, 0x3FF);
		mapTilePalette.Create(hWnd, 0x9002, 300, 176, 62, 0, 7);
		break;
	}
	case WM_SHOWWINDOW:
		SetWindowPosition(hWnd, set.mapED.X, set.mapED.Y);
		break;
	case WM_CLOSE:
		DeleteObject(backBufferMapProc);
		EndDialog(hWnd, 0);
		break;
	}
	return 0; 
}