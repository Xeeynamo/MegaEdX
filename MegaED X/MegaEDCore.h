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
#include "CompressionCore.h"

class MegaEDCore
{
private:
	HWND hWnd;
	HANDLE hFile;
public:
	LPBYTE rom;
	DWORD romSize;
	WORD dummyHeader;
	char filePath[MAX_PATH];

	MegaEDCore();
	~MegaEDCore();
	void GetHWND(HWND);
	void FreeRom();
	bool LoadNewRom(LPCSTR);
	bool SaveRom(LPCSTR);
	
	virtual void Init();
	virtual void Save();
	virtual void Exit();
};