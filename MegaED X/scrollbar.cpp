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

void ScrollBar::Create(HWND hWnd, const int x, const int y, const int height)
{
	hCtrl = CreateWindow("SCROLLBAR", NULL, WS_CHILD | WS_VISIBLE | SBS_VERT, x, y, 15, height, hWnd, NULL, NULL, NULL);
	if (hCtrl == NULL)
		ErrorBox(hWnd, "Unable to create ScrollBar", GetLastError());
}
void ScrollBar::GetRange(int *min, int *max)
{
	if (!GetScrollRange(hCtrl, SB_CTL, min, max))
		ErrorBox(hCtrl, "Unable to get scrollbar's range", GetLastError());
}
void ScrollBar::SetRange(const int min, const int max)
{
	if (!SetScrollRange(hCtrl, SB_CTL, min, max, true))
		ErrorBox(hCtrl, "Unable to set scrollbar's range", GetLastError());
}
int  ScrollBar::GetPos()
{
	return GetScrollPos(hCtrl, SB_CTL);
}
void ScrollBar::SetPos(int pos)
{
	SetScrollPos(hCtrl, SB_CTL, pos, true);
}
void ScrollBar::Work(WPARAM wParam)
{
	value = GetPos();
	switch(LOWORD(wParam))
	{
	case SB_LINEUP:
		value--;
		break;
	case SB_LINEDOWN:
		value++;
		break;
	case SB_PAGEUP:
		value-=2;
		break;
	case SB_PAGEDOWN:
		value+=2;
		break;
	case SB_THUMBPOSITION:
		value = HIWORD(wParam);
		break;
	case SB_THUMBTRACK:
		value = HIWORD(wParam);
		break;
	case SB_TOP:
		ErrorBox(hCtrl, "SB_TOP unimplemented", 0);
		break;
	case SB_BOTTOM:
		ErrorBox(hCtrl, "SB_BOTTOM unimplemented", 0);
		break;
	case SB_ENDSCROLL:
		SetPos(value);
		break;
	}
	SetPos(value);
}
long ScrollBar::GetID()
{
	return (long)hCtrl;
}
bool ScrollBar::IsIDEqual(long id)
{
	return (long)hCtrl == id;
}