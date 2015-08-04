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

BYTE dPalOverMouse, dPalSelected;
bool dPalTextWrite;
BOOL CALLBACK PalettesProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_LBUTTONDBLCLK:
		{
			BYTE cPal = ((lParam&0xFF)>>4) + ((lParam>>16)&0xF0);
			CHOOSECOLOR chColor;
			memset(&chColor, 0, sizeof(CHOOSECOLOR));
			chColor.lStructSize = sizeof(CHOOSECOLOR);
			chColor.hwndOwner = hWnd;
			chColor.Flags = CC_ANYCOLOR | CC_RGBINIT;

			COLORREF colors[16];
			for(int i=0; i<16; i++)
				colors[i] = nmmx.ConvertBGRColor(nmmx.palCache[i | (cPal&0xF0)]);
			chColor.lpCustColors = colors;
			chColor.rgbResult = colors[cPal&0xF];

			if(ChooseColor(&chColor))
			{
				*((LPWORD)(nmmx.rom + nmmx.palettesOffset[dPalOverMouse]) + (cPal&0xF)) = nmmx.ConvertRGB2SNES(chColor.rgbResult);
				LPWORD asd = ((LPWORD)(nmmx.rom + nmmx.palettesOffset[dPalOverMouse]) + (cPal&0xF));
				asd = asd;
				nmmx.palCache[(dPalOverMouse<<4) | cPal] = nmmx.Get16Color(nmmx.palettesOffset[dPalOverMouse] + cPal*2);
				RefreshLevel();
			}
		}
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			HDC hBack = CreateCompatibleDC(hdc);
			HBITMAP hBmpBack = CreateCompatibleBitmap(hdc, ps.rcPaint.right, ps.rcPaint.bottom);
			SelectObject(hBack, hBmpBack);

			HBRUSH brush;
			for(int y=0; y<8; y++)
				for(int x=0; x<0x10; x++)
				{
					brush = CreateSolidBrush(nmmx.ConvertBGRColor(nmmx.palCache[x | (y<<4)]));
					SelectObject(hBack, brush);
					Rectangle(hBack, (x<<4), (y<<4), 18 + (x<<4), 18 + (y<<4));
                    DeleteObject(brush);
				}

			sprintf_s(out, "Offset: 0x%5X   Color %04X", nmmx.palettesOffset[dPalOverMouse], *(LPWORD)(nmmx.rom + nmmx.palettesOffset[dPalOverMouse] + dPalSelected*2));
			render.Print(hBack, 2, 17, out, 0);

			BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, hBack, 0, 0, SRCCOPY);
			DeleteObject(hBmpBack);
			DeleteDC(hBack);
			EndPaint(hWnd, &ps);
		}
		break;
	case WM_MOUSEMOVE:
		if ((lParam >> 16) < 0x080)
		{
			dPalOverMouse = (BYTE)(lParam>>20);
			dPalSelected = (BYTE)(lParam>>4 & 0xF);
			InvalidateRect(hWnd, NULL, false);
		}
		break;
	case WM_MOVE:
		RECT rc;
		GetWindowRect(hWnd, &rc);
		set.palED.X = (SHORT)rc.left;
		set.palED.Y = (SHORT)rc.top;
	case WM_SHOWWINDOW:
		hWID[1] = hWnd;
		SetWindowPosition(hWnd, set.palED.X, set.palED.Y);
		break;
	case WM_CLOSE:
		EndDialog(hWnd, 0);
		break;
	}
	return 0;
}
