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

#include <Windows.h>

int GFXRLE(BYTE* rom, BYTE *dest, int pointer, int size);
WORD GFXRLECmp(BYTE* src, BYTE *dest, int size);
int LayoutRLE(BYTE width, BYTE height, BYTE *sceneUsed, BYTE *src, BYTE *dst);