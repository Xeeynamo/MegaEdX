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

#include "FileDialog.h"

void ShowLastError(DWORD error)
{
	LPSTR message;
	switch(error)
	{
	case ERROR_FILE_NOT_FOUND:
		message = "File not found.";
		break;
	case ERROR_PATH_NOT_FOUND:
		message = "Invalid path.";
		break;
	case ERROR_TOO_MANY_OPEN_FILES:
		message = "Too many files open.";
		break;
	case ERROR_ACCESS_DENIED:
		message = "Access deined.";
		break;
	case ERROR_INVALID_HANDLE:
		message = "Invalid handle.";
		break;
	case ERROR_WRITE_PROTECT:
		message = "Write protect.";
		break;
	case ERROR_WRITE_FAULT:
		message = "Write error.";
		break;
	case ERROR_READ_FAULT:
		message = "Read error.";
		break;
	case ERROR_GEN_FAILURE:
		message = "Generic error.";
		break;
	case ERROR_SHARING_VIOLATION:
		message = "File already open.";
		break;
	case ERROR_INVALID_NAME:
		message = "Invalid filename.";
		break;
	default:
		{
			char msgx[40];
			sprintf_s(msgx, "Unknow error, please report.\n%08X", error);
			message = msgx;
		}
		break;
	}
	MessageBox(NULL, message, "Error", MB_ICONERROR);
}
LPTSTR OpenFileDialog(HWND hWnd, LPCTSTR filter, LPSTR fileName)
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

	if (GetOpenFileName(&ofn))
	{
		strcpy(fileName, ofn.lpstrFile);
		return fileName;
	}
	fileName[0] = '\0';
	return NULL;
}
LPBYTE OpenFileRam(LPTSTR fileName, LPDWORD size)
{
	if (fileName[0] == NULL)
		return false;
	LPBYTE data;
	HANDLE hFile = CreateFile(fileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		*size = GetFileSize(hFile, NULL);
		if (*size == INVALID_FILE_SIZE)
		{
			MessageBox(NULL, "GetFileSize", "Error", MB_ICONERROR);
			ShowLastError(GetLastError());
			CloseHandle(hFile);
			return false;
		}
		data = (LPBYTE)Malloc(*size);

		DWORD bytesR;
		if (!ReadFile(hFile, data, *size, &bytesR, NULL))
		{
			MessageBox(NULL, "ReadFile", "Error", MB_ICONERROR);
			ShowLastError(GetLastError());
			Free(data);
			return false;
		}
		if (*size != bytesR)
		{
			MessageBox(NULL, "Bytes Read", "Error", MB_ICONERROR);
			ShowLastError(GetLastError());
			Free(data);
			return false;
		}
		CloseHandle(hFile);
		return data;
	}
	ShowLastError(GetLastError());
	return NULL;
}