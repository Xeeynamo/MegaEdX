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

#include "stdafx.h"

class Toolbar
{
private:
	HWND hWnd, hWndMain;
	HINSTANCE hInst;
	DWORD bitmap;
	UINT id;
	TBBUTTON tButton[0x40];
	BYTE howButtons;
	BYTE howStructs;
public:
	bool Create(HWND, HINSTANCE, DWORD bitmap, UINT id);
	void AddButton(BYTE type, UINT id);
	bool Display();
	void Resize();
};