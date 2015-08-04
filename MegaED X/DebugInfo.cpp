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

bool debugEnabled = true;
char out[0x40];
const char *levelsName[]={"  HIGHWAY STAGE  ", " LAUNCH OCTOPUS  ", " STING CHAMELEON ", "ARMORED ARMADILLO", "  FLAME MAMMOTH  ", "   STORM EAGLE   ",
	" SPARK MANDRILL  ", " BOOMER KUWANGER ", "  CHILL PENGUIN  ", " SIGMA FORTRESS  ", " SIGMA FORTRESS  ", " SIGMA FORTRESS  ", "   FINAL STAGE   "};

extern short winWidth, winHeight;
void WriteDebugInfo(HDC hDC)
{
	if (!debugEnabled)
		return;
	if (nmmx.checkpointInfo == NULL)
		return;
	RECT rect;
	GetClientRect(hWID[0], &rect);
	
	render.PrintBig(hDC, 0, 0, levelsName[nmmx.level], 1);
	sprintf_s(out, "Level: %i (PGDWN/PGUP)", nmmx.level);
	render.Print(hDC, 17, 0, out, 0);
	sprintf_s(out, "Point: %i (O/P)", nmmx.point);
	render.Print(hDC, 17, 1, out, 0);
	sprintf_s(out, "Level size:  %2ix%2i", nmmx.levelWidth, nmmx.levelHeight);
	render.Print(hDC, 0, 2, out, 0);
	sprintf_s(out, "Scene used:  %2i", nmmx.sceneUsed);
	render.Print(hDC, 0, 3, out, 0);

	sprintf_s(out, "Object:  %02X (Q/W)", nmmx.checkpointInfo->objLoad);
	render.Print(hDC, 0, 4, out, 0);
	sprintf_s(out, "Tile:    %02X (A/S)", nmmx.checkpointInfo->tileLoad);
	render.Print(hDC, 0, 5, out, 0);
	sprintf_s(out, "Palette: %02X (Z/X)", nmmx.checkpointInfo->palLoad);
	render.Print(hDC, 0, 6, out, 0);
	sprintf_s(out, "Character pos: %04X, %04X", nmmx.checkpointInfo->chX, nmmx.checkpointInfo->chY);
	render.Print(hDC, 0, 7, out, 0);
	sprintf_s(out, "Camera pos: %04X, %04X", nmmx.checkpointInfo->camX, nmmx.checkpointInfo->camY);
	render.Print(hDC, 0, 8, out, 0);
	sprintf_s(out, "Background pos: %04X,  %04X", nmmx.checkpointInfo->bkgX, nmmx.checkpointInfo->bkgY);
	render.Print(hDC, 0, 9, out, 0);
	sprintf_s(out, "Min char pos: %04X,  %04X", nmmx.checkpointInfo->minX, nmmx.checkpointInfo->minY);
	render.Print(hDC, 0,10, out, 0);
	sprintf_s(out, "Max char pos: %04X,  %04X", nmmx.checkpointInfo->maxX, nmmx.checkpointInfo->maxY);
	render.Print(hDC, 0,11, out, 0);
}