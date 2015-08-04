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

void ErrorBox(HWND hWnd, LPCTSTR strError, DWORD dwError)
{
	CHAR message[128];
	sprintf_s(message, "%s\nError 0x%08X", strError, dwError);
	MessageBox(hWnd, message, "Error", MB_ICONERROR);
}
HBITMAP CreateBitmapCache(HWND hWnd, int width, int height)
{
	HDC tmpDC = GetDC(hWnd);
	HBITMAP hBmp = CreateCompatibleBitmap(tmpDC, 256, 256);
	DeleteDC(tmpDC);
	return hBmp;
}
#ifdef NOPUBLIC
void FileDebugOut(void *data, DWORD dataSize, STRING fileName)
{
	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD bytesW;
		
		if (!WriteFile(hFile, data, dataSize, (LPDWORD)&bytesW, NULL))
			ErrorBox(NULL, "FileDebugOut->WriteFile error", GetLastError());
		else if (dataSize != bytesW)
			ErrorBox(NULL, "FileDebugOut->WriteFile error!", GetLastError());
		CloseHandle(hFile);
		return;
	}
	ErrorBox(NULL, "FileDebugOut WTF", (DWORD)INVALID_HANDLE_VALUE);
}
#endif

const BITMAPINFO __bmpInfo = {sizeof(BITMAPINFOHEADER), 8, -8, 1, 16, BI_RGB, 0, 0, 0, 0, 0};
HBITMAP hBmp = NULL;
void *xdata;
void CreateBitmapCache()
{
	if (hBmp == NULL) hBmp = CreateDIBSection(NULL, &__bmpInfo, 0, &xdata, 0, 0);
}
void RenderImage(HDC hdc, int x, int y, WORD image[], WORD param)
{
	HDC dcBmp = CreateCompatibleDC(hdc);
	memcpy(xdata, image, 128);
	SelectObject(dcBmp, hBmp);
	if (param & 0x4000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 7, 0, -8, 8, SRCCOPY);
	if (param & 0x8000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 0, 7, 8, -8, SRCCOPY);
	BitBlt(hdc, x<<3, y<<3, 8, 8, dcBmp, 0, 0, SRCCOPY);
	DeleteDC(dcBmp);
}
void RenderImageX(HDC hdc, int x, int y, LPBYTE image, BYTE palette, WORD param)
{
	HDC dcBmp = CreateCompatibleDC(hdc);
	LPWORD data = (LPWORD)xdata;
	palette <<= 4;
	for(int i=0; i<0x40; i++)
		*data++ = nmmx.palCache[*image++ | palette];
	SelectObject(dcBmp, hBmp);
	if (param & 0x4000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 7, 0, -8, 8, SRCCOPY);
	if (param & 0x8000) StretchBlt(dcBmp, 0, 0, 8, 8, dcBmp, 0, 7, 8, -8, SRCCOPY);
	BitBlt(hdc, x<<3, y<<3, 8, 8, dcBmp, 0, 0, SRCCOPY);
	DeleteDC(dcBmp);
}
void RenderImageZoom(HDC hdc, int x, int y, WORD image[], int size)
{
	HDC dcBmp = CreateCompatibleDC(hdc);
	BITMAPINFO bmpInfo = __bmpInfo;
	size *= 8;
	bmpInfo.bmiHeader.biWidth = 8;
	bmpInfo.bmiHeader.biHeight = -8;
	
	void *xdata;
	HBITMAP hBmp = CreateDIBSection(NULL, &bmpInfo, 0, (void**)&xdata, 0, 0);
	memcpy(xdata, image, 128);
	SelectObject(dcBmp, hBmp);
	DeleteObject(hBmp);
	StretchBlt(dcBmp, 0, 0, 16, 16, dcBmp, 0, 0, 8, 8, SRCCOPY);
	BitBlt(hdc, 0, 0, 16, 16, dcBmp, 0, 0, SRCCOPY);
	DeleteDC(dcBmp);
}
void SetWindowTitle(HWND hWnd, CSTRING fileName)
{
	LPSTR realFileName = (LPSTR)Malloc(MAX_PATH);
	GetFileTitle(fileName, realFileName, MAX_PATH);
	STRING title = (STRING)Malloc(MAX_PATH+22);
	wsprintf(title, "MegaED X - %s", realFileName);
	SetWindowText(hWnd, title);
	Free(realFileName);
	Free(title);
}
bool OpenDebug(MegaEDCore *megaedcore)
{
	return megaedcore->LoadNewRom("C:\\debugx.smc");
}
bool OpenFileDialogCore(HWND hWnd, LPCSTR filter, MegaEDCore *megaedcore)
{
    OPENFILENAME ofn;
    CHAR szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

	megaedcore->FreeRom();
	if (GetOpenFileName(&ofn))
		if (megaedcore->LoadNewRom(ofn.lpstrFile))
		{
			SetWindowTitle(hWnd, ofn.lpstrFile);
			return true;
		}
	return false;
}
bool SaveFileDialogCore(HWND hWnd, LPCSTR filter, MegaEDCore *romcore)
{
    OPENFILENAME ofn;
    CHAR szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_PATHMUSTEXIST;

	if (GetSaveFileName(&ofn))
	{
		HANDLE hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			int bytesW;
			if (!WriteFile(hFile, romcore->rom, romcore->romSize, (LPDWORD)&bytesW, NULL))
			{
				ErrorBox(hWnd, "SaveFileDialog->WriteFile error", GetLastError());
				return false;
			}
			if (romcore->romSize != bytesW)
			{
				ErrorBox(hWnd, "SaveFileDialog->WriteFile error!", GetLastError());
				return false;
			}
			CloseHandle(hFile);
			return true;
		}
		ErrorBox(hWnd, "Unable to open the file", GetLastError());
	}
	return false;
}