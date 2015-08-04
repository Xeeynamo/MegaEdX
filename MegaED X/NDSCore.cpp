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

#include "NDSCore.h"

void NDSCore::tile4bpp2raw(LPBYTE src, LPBYTE dst)
{
	for(int i=0; i<0x20; i++)
	{
		dst[i*2 + 0] = *src & 0xF;
		dst[i*2 + 1] = *src++ >> 4;
	}
}
void NDSCore::raw2tile4bpp(LPBYTE src, LPBYTE dst)
{
	for(int i=0; i<0x20; i++)
		*src++ = dst[i*2 + 0] | (dst[i*2 + 1]<<4);
}