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

#include "CompressionCore.h"

int GFXRLE(BYTE* rom, BYTE *dest, int pointer, int size)
{
	int oldPointer = pointer;
	int writeIndex = 0;
	for(int i=0; i<size>>3; i++)
	{
		byte control = rom[pointer++];
		byte data = rom[pointer++];
		for(int j=0; j<8; j++)
		{
			if (control & 0x80)
				dest[writeIndex++] = rom[pointer++];
			else
				dest[writeIndex++] = data;
			control <<= 1;
		}
	}
	return pointer - oldPointer;
}
WORD GFXRLECmp(BYTE* src, BYTE *dest, int size)
{
	BYTE *odest = dest;
	byte maxCount = 0;
	byte bArr[0x100];
	byte control = 0xFF;
	ZeroMemory(bArr, 0x100);
	for(int i=0; i<size>>3; i++)
	{
		byte *config = config = dest;
		byte data;
		for (int j=0; j<8; j++)
		{
			bArr[src[j]]++;
			if (bArr[src[j]] > maxCount) 
			{ 
				maxCount = bArr[src[j]]; 
				data = src[j];
			}
		}
		maxCount = 0;
		ZeroMemory(bArr, 0x100);

		dest = dest + 2;
		for(int i=0; i<8; i++)
		{
			if (data == *src)
				control ^= 0x80>>i;
			else
				*dest++ = *src;
			*src++;
		}
		*config++ = control;
		*config   = data;
		control = 0xFF;
	}
	return (WORD)(dest-odest);
}

int LayoutRLE(BYTE width, BYTE height, BYTE *sceneUsed, BYTE *src, BYTE *dst)
{
	bool cType;
	BYTE counter = 0;
	SHORT writeIndex = 3;
    for(int i=0; i<width*height;)
    {
        byte buf = src[i++];
        counter++;
        do
        {
            if (i >= width*height)
            {
                goto WRITE;
            }
            else
            {
                if (*sceneUsed < src[i])
                    *sceneUsed = src[i]+1;
                if (src[i] == buf)
                {
                    if (counter == 1)
                    {
                        cType = true;
                        counter |= 0x80;
						counter++;
                        i++;
                    }
                    else if (cType)
                    {
                        counter++;
                        i++;
                    }
                    else
                        goto WRITE;
                }
                else if (src[i] == buf+counter)
                {
                    if (counter == 1)
                    {
                        cType = false;
						counter++;
                        i++;
                    }
                    else if (!cType)
                    {
                        counter++;
                        i++;
                    }
                    else
                        goto WRITE;
                }
                else if (counter == 0x7E)
                {
                    goto WRITE;
                }
                else
                {
    WRITE:
                    dst[writeIndex++] = counter;
                    dst[writeIndex++] = buf;
                    counter = 0;
                }
            }
        } while(counter);
    }
	dst[0] = width;
	dst[1] = height;
	dst[2] = *sceneUsed;
	return writeIndex;
}
