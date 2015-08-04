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

#define IDC_BLOCKINDEX 0x9123

ScrollBar sceneScroll;
SpinBox blockIndex;

BYTE blockSelected = 0;
LPWORD pScene;

// Drawing vars
HDC hScenekDC, hSceneBack;
HBITMAP backBufferSceneProc;
const RECT viewerRect = {0, 0, 256, 256};
RECT blockSelectRect = {0, 0, 32, 32};

BOOL CALLBACK SceneProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	switch (message)
	{
	case WM_VSCROLL:
		if (sceneScroll.IsIDEqual((long)lParam))
		{
			sceneScroll.SetRange(0, nmmx.sceneUsed-1);
			sceneScroll.Work(wParam);
			if (LOWORD(wParam) != SB_ENDSCROLL)
			{
				RECT rect;
				rect.top = 0;
				rect.left = 0;
				rect.bottom = 256;
				rect.right = 256;
				InvalidateRect(hWnd, &viewerRect, false);
			}
			char sIndex[910];
			sprintf_s(sIndex, "Scene #%02X", sceneScroll.GetPos());
			SetWindowText(GetDlgItem(hWnd, IDC_SCENED_LSCENE), sIndex);
		}
		else if (blockIndex.IsIDEqual((long)lParam))
		{
			blockIndex.Work(wParam);
			*(LPWORD)(nmmx.rom + nmmx.pScenes + sceneScroll.GetPos()*0x80 + blockSelected*2) = blockIndex.GetPos();
			InvalidateRect(hWnd, &viewerRect, false);
		}
	case WM_LBUTTONDOWN:
		if ((lParam & 0xFFFF) < 0x0100)
		{
			blockSelected = (WORD)(((lParam>>5)&0xF) | ((lParam>>18)&0xF8));
			blockIndex.SetPos(*(LPWORD)(nmmx.rom + nmmx.pScenes + sceneScroll.GetPos()*0x80 + blockSelected*2));
			blockSelectRect.left   = (blockSelected & 7) * 32;
			blockSelectRect.right  = (blockSelected & 7) * 32 + 32;
			blockSelectRect.top    = blockSelected / 8 * 32;
			blockSelectRect.bottom = blockSelected / 8 * 32 + 32;
			InvalidateRect(hWnd, &viewerRect, false);
		}
		break;
	case WM_MOVE:
		RECT rc;
		GetWindowRect(hWnd, &rc);
		set.scenED.X = (SHORT)rc.left;
		set.scenED.Y = (SHORT)rc.top;
	case WM_PAINT:
		hScenekDC = BeginPaint(hWnd, &ps);
		hSceneBack = CreateCompatibleDC(hScenekDC);
		SelectObject(hSceneBack, backBufferSceneProc);

		pScene = (LPWORD)(nmmx.rom + nmmx.pScenes + sceneScroll.GetPos()*0x80);
		for(int i=0; i<0x40; i++)
			render.RenderBlock(hSceneBack, i & 0x7, i>>3, *pScene++);
		
		DrawFocusRect(hSceneBack, &blockSelectRect);
		
		BitBlt(hScenekDC, 0, 0, 256, 256, hSceneBack, 0, 0, SRCCOPY);
		DeleteDC(hSceneBack);
		EndPaint(hWnd, &ps);
		break;
	case WM_INITDIALOG:
		hWID[5] = hWnd;
		backBufferSceneProc = CreateBitmapCache(hWnd, 256, 256);
		sceneScroll.Create(hWnd, 257, 0, 256);
		sceneScroll.SetRange(0, nmmx.sceneUsed-1);
		blockIndex.Create(hWnd, IDC_BLOCKINDEX, 196, 257, 60, 0, 0x3FF);
		break;
	case WM_SHOWWINDOW:
		SetWindowPosition(hWnd, set.scenED.X, set.scenED.Y);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}
	return 0; 
}