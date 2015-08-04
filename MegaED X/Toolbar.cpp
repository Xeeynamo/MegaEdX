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

#include "Toolbar.h"

bool Toolbar::Create(HWND _hWnd, HINSTANCE _hInst, DWORD _bitmap, UINT _id)
{
	/*INITCOMMONCONTROLSEX InitCtrlEx;
	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC  = ICC_BAR_CLASSES;
	if (!InitCommonControlsEx(&InitCtrlEx)) return false;*/
	InitCommonControls();

	if (_hWnd == NULL) return false;
	else hWndMain = _hWnd;
	if (_hInst == NULL) return false;
	else hInst = _hInst;
	if (_bitmap == NULL) return false;
	else bitmap = _bitmap;
	if (_id == NULL) return false;
	else id = _id;
	ZeroMemory(tButton, sizeof(TBBUTTON) * 0x40);
	howButtons = 0;
	howStructs = 0;
	return true;
}
void Toolbar::AddButton(BYTE type, UINT id)
{
	tButton[howStructs].iBitmap = type == TBSTYLE_SEP ? 0 : howButtons++;
	tButton[howStructs].idCommand = id;
	tButton[howStructs].fsState = TBSTATE_ENABLED;
	tButton[howStructs].fsStyle = type;
	tButton[howStructs].dwData  = 0;
	//tButton[howStructs].iString = (INT_PTR)"asd";
	howStructs++;
}
bool Toolbar::Display()
{
	hWnd = CreateToolbarEx(hWndMain, WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS | /*TBSTYLE_LIST | */TBSTYLE_WRAPABLE, id, 12, hInst, bitmap, tButton, howStructs, 16, 16, 16, 16, sizeof(TBBUTTON));
	if (hWnd == NULL) return false;
	return true;
}
void Toolbar::Resize()
{
	RECT rc;
	GetClientRect(hWndMain, &rc);

}
