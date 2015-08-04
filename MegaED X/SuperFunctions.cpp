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

// Imports
extern bool drawBackground;
extern HBITMAP levelBuffer;
extern HBITMAP backBuffer;

void RepaintAll()
{
	DeleteObject(levelBuffer);
	DeleteObject(backBuffer);
	levelBuffer = NULL;
	backBuffer = NULL;
	drawLevelBuffer = true;
	for(int i=0; i<5; i++)
		if (hWID[i] != NULL)
			InvalidateRect(hWID[i], NULL, false);
}

void LoadLevel(WORD level)
{
	nmmx.SetLevel(level, 0);
	nmmx.LoadLevel();
	render.RefreshMapCache();
	SetScrollRange(hWID[0], SB_HORZ, 0, (nmmx.levelWidth<<8), true);
	SetScrollRange(hWID[0], SB_VERT, 0, (nmmx.levelHeight<<8), true);
	RepaintAll();
}
void RefreshLevel()
{
	if (drawBackground)
		nmmx.LoadBackground();
	else
		nmmx.LoadLevel();
	render.RefreshMapCache();
	RepaintAll();
}
void ScrollProc(HWND hWnd, bool scrollbar, WPARAM wParam, short *value)
{
	switch(LOWORD(wParam))
	{
	case SB_LINEUP:
		*value-=8;
		break;
	case SB_LINEDOWN:
		*value+=8;
		break;
	case SB_PAGEUP:
		*value-=64;
		break;
	case SB_PAGEDOWN:
		*value+=64;
		break;
	case SB_THUMBPOSITION:
		*value = HIWORD(wParam);
		break;
	case SB_THUMBTRACK:
		*value = HIWORD(wParam);
		break;
	case SB_TOP:
		ErrorBox(hWnd, "SB_TOP unimplemented", 0);
		break;
	case SB_BOTTOM:
		ErrorBox(hWnd, "SB_BOTTOM unimplemented", 0);
		break;
	case SB_ENDSCROLL:
		SetScrollPos(hWnd, scrollbar, *value, true);
		break;
	}
	if (*value < 0) *value = 0;
	SetScrollPos(hWnd, scrollbar, *value, true);
}
void FreezeMenu(HWND hWnd, WORD id, bool freeze)
{
	SendMessage(GetDlgItem(hWnd, id), NULL, NULL, NULL);
}
bool IsFileExist(LPSTR fileName)
{
#ifdef WIN32
	return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
#else
	return _access(fileName, 00) == 0;
#endif
}
void SetWindowPosition(HWND hWnd, SHORT x, SHORT y)
{
	RECT rcwin;
	GetWindowRect(hWnd, &rcwin);
	SetWindowPos(hWnd, NULL, x, y, rcwin.right - rcwin.left, rcwin.bottom - rcwin.top, SWP_SHOWWINDOW);
}