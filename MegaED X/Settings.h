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

const char configFile[] = "MegaEDX.ini";

struct Settings
{
	COORD WinMainSize, WinMainPos;
	COORD palED, tilED, mapED, blkED, scenED;
	BYTE  max;
	BYTE  romAtStartup;
	WORD  lastLevel;
	COORD desktop;
	char emupath[MAX_PATH];
	char lastroms[5][MAX_PATH];
};

extern Settings set;
void LoadSettings();
void SaveSettings();