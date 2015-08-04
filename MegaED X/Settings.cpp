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

#include "Settings.h"

Settings set;
const char *_setDialogsName[] = {"WinMain", "PalED", "TilED", "MapED", "BlkED", "ScenED"};

void WritePrivateProfileInt(LPCSTR lpszSection, LPCSTR lpszEntry, int value, LPCSTR lpszFilename)
{
	char szConverted[8];
	_itoa_s(value, szConverted, 10);
	WritePrivateProfileString(lpszSection, lpszEntry, szConverted, lpszFilename);
}
void LoadSettings()
{
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	set.desktop.X = (SHORT)rc.right;
	set.desktop.Y = (SHORT)rc.bottom;
	SHORT standardCoordX = set.desktop.X/2 - 256;
	SHORT standardCoordY = set.desktop.Y/2 - 256;
	
	if (GetPrivateProfileInt("MegaEDX", "Debug", 0, configFile))
		MessageBox(NULL, "YAY!            ", "Megaman greets you", MB_ICONINFORMATION);
	set.romAtStartup = GetPrivateProfileInt("MegaEDX", "RomAtStartup", 1, configFile);
	set.lastLevel = GetPrivateProfileInt("MegaEDX", "LastLevel", 0, configFile);
	GetPrivateProfileString("Recent", "Rom1", NULL, set.lastroms[0], MAX_PATH, configFile);
	GetPrivateProfileString("Recent", "Rom2", NULL, set.lastroms[1], MAX_PATH, configFile);
	GetPrivateProfileString("Recent", "Rom3", NULL, set.lastroms[2], MAX_PATH, configFile);
	GetPrivateProfileString("Recent", "Rom4", NULL, set.lastroms[3], MAX_PATH, configFile);
	GetPrivateProfileString("Recent", "Rom5", NULL, set.lastroms[4], MAX_PATH, configFile);
	GetPrivateProfileString("Emulator", "Path", NULL, set.emupath, MAX_PATH, configFile);
	set.WinMainSize.X = GetPrivateProfileInt(_setDialogsName[0], "SizeX", 620, configFile);
	set.WinMainSize.Y = GetPrivateProfileInt(_setDialogsName[0], "SizeY", 400, configFile);
	set.max = GetPrivateProfileInt(_setDialogsName[0], "Max", CW_USEDEFAULT, configFile);
	SHORT* pCoord = (SHORT*)&set + 2;
	for(int i=0; i<6; i++)
	{
		*pCoord++ = GetPrivateProfileInt(_setDialogsName[i], "PosX", standardCoordX, configFile);
		*pCoord++ = GetPrivateProfileInt(_setDialogsName[i], "PosY", standardCoordY, configFile);
	}
}
void SaveSettings()
{
	WritePrivateProfileInt("MegaEDX", "RomAtStartup", set.romAtStartup, configFile);
	WritePrivateProfileInt("MegaEDX", "LastLevel", set.lastLevel, configFile);
	WritePrivateProfileString("Recent", "Rom1", set.lastroms[0], configFile);
	WritePrivateProfileString("Recent", "Rom1", set.lastroms[0], configFile);
	WritePrivateProfileString("Recent", "Rom2", set.lastroms[1], configFile);
	WritePrivateProfileString("Recent", "Rom3", set.lastroms[2], configFile);
	WritePrivateProfileString("Recent", "Rom4", set.lastroms[3], configFile);
	WritePrivateProfileString("Recent", "Rom5", set.lastroms[4], configFile);
	WritePrivateProfileString("Emulator", "Path", set.emupath, configFile);
	WritePrivateProfileInt(_setDialogsName[0], "SizeX", set.WinMainSize.X, configFile);
	WritePrivateProfileInt(_setDialogsName[0], "SizeY", set.WinMainSize.Y, configFile);
	WritePrivateProfileInt(_setDialogsName[0], "Max", set.max, configFile);
	SHORT* pCoord = (SHORT*)&set + 2;
	for(int i=0; i<6; i++)
	{
		WritePrivateProfileInt(_setDialogsName[i], "PosX", *pCoord++, configFile);
		WritePrivateProfileInt(_setDialogsName[i], "PosY", *pCoord++, configFile);
	}
}