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

// Control variables:
ScrollBar blockScroll;
SpinBox mapsAlloc[4];

// General variables:
BYTE blockSelection  = 0;
WORD dBlockSelected  = 0;
WORD dBlockOverMouse = 0;
WORD dBlockTextWrite = 0;

// Drawing vars
HDC hBlockDC, hBlockBack;
HBITMAP backBufferBlockProc;

extern RECT rMapEdit;
extern RECT rectMapTileFocus[];

void UpdateBlockWrite(HWND hWnd)
{
	*(LPWORD)(nmmx.rom + nmmx.pBlocks + (dBlockSelected<<3) + 0) = mapsAlloc[0].GetPos();
	*(LPWORD)(nmmx.rom + nmmx.pBlocks + (dBlockSelected<<3) + 2) = mapsAlloc[1].GetPos();
	*(LPWORD)(nmmx.rom + nmmx.pBlocks + (dBlockSelected<<3) + 4) = mapsAlloc[2].GetPos();
	*(LPWORD)(nmmx.rom + nmmx.pBlocks + (dBlockSelected<<3) + 6) = mapsAlloc[3].GetPos();
	InvalidateRect(hWnd, NULL, false);
}
BOOL CALLBACK BlockProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_VSCROLL:
		if (blockScroll.IsIDEqual((long)lParam))
		{
			blockScroll.Work(wParam);
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
		else
		{
			for(int i=0; i<4; i++)
				if (mapsAlloc[i].IsIDEqual((long)lParam))
				{
					mapsAlloc[i].Work(wParam);
					UpdateBlockWrite(hWnd);
				}
		}
	case WM_LBUTTONDOWN:
			if ((lParam & 0xFFFF) < 0x0100)
			{
				dBlockTextWrite = false;
				dBlockSelected = dBlockOverMouse;
				InvalidateRect(hWnd, &rMapEdit, false);
				for(int i=0; i<4; i++)
					mapsAlloc[i].SetPos(*(LPWORD)(nmmx.rom + nmmx.pBlocks + (dBlockSelected<<3) + i*2));
			}
			break;
	case WM_MOUSEMOVE:
		if ((lParam & 0xFFFF) < 0x0100)
		{
			dBlockOverMouse = (WORD)(((lParam>>5)&0xF) | ((lParam>>18)&0xF8) + (blockScroll.GetPos()<<3));
			goto PRINTEXT;
		}
		else
		{
			if (dBlockOverMouse == dBlockSelected)
				break;
			dBlockOverMouse = dBlockSelected;
			goto PRINTEXT;
		}
		if (false)
		{
PRINTEXT:
			dBlockTextWrite = true;
			CHAR sIndex[13];
			sprintf_s(sIndex, "Block N. %03X", dBlockOverMouse);
			SetWindowText(GetDlgItem(hWnd, IDC_LBLOCKINDEX), sIndex);
		}
		break;
	case WM_MOVE:
		RECT rc;
		GetWindowRect(hWnd, &rc);
		set.blkED.X = (SHORT)rc.left;
		set.blkED.Y = (SHORT)rc.top;
	case WM_PAINT:
		hBlockDC = BeginPaint(hWnd, &ps);
		hBlockBack = CreateCompatibleDC(hBlockDC);
		SelectObject(hBlockBack, backBufferBlockProc);
		
		render.RenderBlock(hBlockDC, 9, 1, dBlockSelected);
		StretchBlt(hBlockDC, 288, 32, 64, 64, hBlockDC, 288, 32, 32, 32, SRCCOPY);

		for(int i=0; i<0x40; i++)
			render.RenderBlock(hBlockBack, i & 0x7, i>>3, (blockScroll.GetPos()<<3) + i);
		
		BitBlt(hBlockDC, 0, 0, 256, 256, hBlockBack, 0, 0, SRCCOPY);
		DeleteDC(hBlockBack);
		EndPaint(hWnd, &ps);
		break;
	case WM_INITDIALOG:
		hWID[4] = hWnd;
		backBufferBlockProc = CreateBitmapCache(hWnd, 256, 256);
		blockScroll.Create(hWnd, 257, 0, 256);
		blockScroll.SetRange(0, 0x78);
		for(int i=0; i<4; i++)
			mapsAlloc[i].Create(hWnd, 0x9000 + i, 296, 128 + (i*24), 64, 0, 0x3FF);
		break;
	case WM_SHOWWINDOW:
		SetWindowPosition(hWnd, set.blkED.X, set.blkED.Y);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}
	return 0; 
}