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

MegaEDCore::MegaEDCore()
{
	rom = NULL;
}
MegaEDCore::~MegaEDCore()
{
	FreeRom();
}
void MegaEDCore::Init()
{
}
void MegaEDCore::Save()
{
}
void MegaEDCore::Exit()
{
}
void MegaEDCore::GetHWND(HWND hWND)
{
	hWnd = hWND;
}
void MegaEDCore::FreeRom()
{
	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	if (rom)
	{
		rom = rom-dummyHeader;
		free(rom);
		rom = NULL;
	}
	dummyHeader = 0;
}
bool MegaEDCore::LoadNewRom(LPCSTR fileName)
{
	if (fileName[0] == NULL)
		return false;
	HANDLE hNewFile = CreateFile(fileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hNewFile != INVALID_HANDLE_VALUE)
	{
		FreeRom();
		hFile = hNewFile;
		romSize = GetFileSize(hFile, NULL);
		if (romSize == INVALID_FILE_SIZE)
			goto LOC_ERROR;
		rom = (LPBYTE)malloc(romSize);

		DWORD bytesR;
		if (!ReadFile(hFile, rom, romSize, &bytesR, NULL))
		{
			free(rom);
			goto LOC_ERROR;
		}
		if (romSize != bytesR)
		{
			free(rom);
			goto LOC_ERROR;
		}
		strcpy(filePath, fileName);
		Init();
		return true;
	}
LOC_ERROR:
	ShowLastError(GetLastError());
	return false;
}
bool MegaEDCore::SaveRom(LPCSTR fileName)
{
	if (hFile)
	{
		OVERLAPPED overlapped;
		ZeroMemory(&overlapped, sizeof(OVERLAPPED));
		DWORD bytesW;
		rom = rom-dummyHeader;
		if (!WriteFile(hFile, rom, romSize, &bytesW, &overlapped))
			goto LOC_ERROR;
		rom = rom+dummyHeader;
		return true;
	}
LOC_ERROR:
	ShowLastError(GetLastError());
	rom = rom+dummyHeader;
	return false;
}