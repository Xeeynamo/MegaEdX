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

void SpinBox::Create(HWND hWnd, WORD id, const int x, const int y, const int width, WORD min, WORD max)
{
	hBox = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "Edit", "0",WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_UPPERCASE
		, x, y, width, 20, hWnd, (HMENU)(id | 0x8000), hInst, NULL);
	hCtrl = CreateUpDownControl(WS_CHILD | WS_VISIBLE | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_NOTHOUSANDS | UDS_WRAP
		, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, hWnd, id, hInst, hBox, min, max, min);
	if (hCtrl == NULL)
		ErrorBox(hWnd, "Unable to create SpinBox", GetLastError());

	HFONT hFont = CreateFont(14, 0, 0, 0, FW_DONTCARE, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                    CLIP_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, FF_DONTCARE, TEXT("Arial"));
	maxval = max;
	
	SendMessage(hCtrl, UDM_SETBASE , 16, true);
	SendMessage(hBox, WM_SETFONT, WPARAM(hFont), true);
}
SpinBox::~SpinBox()
{
	DeleteObject(hFont);
}
void SpinBox::GetRange(int *min, int *max)
{
	/*if (!GetScrollRange(hCtrl, SB_CTL, min, max))
		ErrorBox(hCtrl, L"Unable to get scrollbar's range", GetLastError());*/
}
void SpinBox::SetRange(const int min, const int max)
{
	/*if (!SetScrollRange(hCtrl, SB_CTL, min, max, true))
		ErrorBox(hCtrl, L"Unable to set scrollbar's range", GetLastError());*/
}
int  SpinBox::GetPos()
{
	return SendMessage(hCtrl, UDM_GETPOS, 0, 0) & 0xFFFF;
}
void SpinBox::SetPos(int pos)
{
	SendMessage(hCtrl, UDM_SETPOS, 0, MAKELONG(pos,0));
	if (maxval>>24)
		sprintf_s(text, "%08X", pos);
	else if (maxval>>16)
		sprintf_s(text, "%06X", pos);
	else if (maxval>>12)
		sprintf_s(text, "%04X", pos);
	else if (maxval>>8)
		sprintf_s(text, "%03X", pos);
	else if (maxval>>4)
		sprintf_s(text, "%02X", pos);
	else
		sprintf_s(text, "%01X", pos);
	SendMessage(hBox, WM_SETTEXT, 0, (LPARAM)text);
}
void SpinBox::Work(WPARAM wParam)
{
	char text[8];
	SendMessage(hBox, WM_GETTEXT, 8, (LPARAM)text);
	if (text[0] != 0)
		sscanf_s(text, "%X", &value);
	switch(LOWORD(wParam))
	{
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		value = HIWORD(wParam);
	}
	SetPos(value);
}
long SpinBox::GetID()
{
	return (long)hCtrl;
}
bool SpinBox::IsIDEqual(long id)
{
	return (long)hCtrl == id;
}
char _parseKeys[0x10]={'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
DWORD SpinBox::ParseText(char *text)
{
	DWORD value = 0;
	BYTE  index = 0;
	if (!*text) return -1;
	do
	{
		for(int i=0; i<0x10; i++)
		{
			if (*text == _parseKeys[i])
				value = i<<index;
		}
		index++;
	} while(!*++text);
	return value;
}