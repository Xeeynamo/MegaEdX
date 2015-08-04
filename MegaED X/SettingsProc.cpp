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

#define IsCheck(i) SendMessage(GetDlgItem(hWnd, i), BM_GETCHECK, NULL, NULL) == BST_CHECKED

BOOL CALLBACK SettingsProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		SetWindowText(GetDlgItem(hWnd, IDC_EMUPATH), set.emupath);
		SendMessage(GetDlgItem(hWnd, IDC_LOADROMSTARTUP), BM_SETCHECK, set.romAtStartup, NULL);
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_ERASEEMUPATH:
			set.emupath[0] = '\0';
			SetWindowText(GetDlgItem(hWnd, IDC_EMUPATH), "");
			break;
		case IDC_LOADROMSTARTUP:
			set.romAtStartup = IsCheck(IDC_LOADROMSTARTUP);
			set.romAtStartup = set.romAtStartup;
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}
	return 0;
}
