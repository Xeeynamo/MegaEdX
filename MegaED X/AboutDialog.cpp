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

HBITMAP hBmpLogo;

BOOL CALLBACK AboutProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hDC, hBuf;
	switch (message)
	{
	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);
		hBuf = CreateCompatibleDC(NULL);

		SelectObject(hBuf, hBmpLogo);
		BitBlt(hDC, 0, 0, 320, 200, hBuf, 0, 0, SRCCOPY);

		DeleteDC(hBuf);
		DeleteDC(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_INITDIALOG:
		hBmpLogo = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_MEGAEDXLOGO));
		break;
	case WM_CTLCOLORSTATIC:
		SetBkColor((HDC)wParam, RGB(0, 0, 0));
		SetTextColor((HDC)wParam, RGB(255, 255, 255));
		return (BOOL)GetStockObject(BLACK_BRUSH);
		break;
	case WM_DESTROY:
		DeleteObject(hBmpLogo);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return 0;
}
