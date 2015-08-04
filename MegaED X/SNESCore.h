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

struct Interrupts
{
	BYTE unk1;
	BYTE unk2;
	BYTE unk3;
	WORD cop;
	WORD brk;
	WORD abort;
	WORD nmi;
	WORD reset;
	WORD irq;
};
struct SNESHeader
{
	char title[21];    // C0
	BYTE mapMode;      // D5
	BYTE cartType;     // D6
	BYTE romSize;      // D7
	BYTE ramSize;      // D8
	BYTE country;      // D9
	BYTE license;      // DA
	BYTE version;      // DB
	WORD checksumx;    // DC
	WORD checksum;     // DE
	Interrupts intNative, intEmu;
};

class SNESCore : public MegaEDCore
{
private:
public:
	WORD dumpHeader;
	SNESHeader *header;
	WORD palCache[0x100];
	BYTE vramCache[0x10000];
	
	#define ReadByte(offset) (*((BYTE*)(rom+offset)))
	#define ReadWord(offset) (*((WORD*)(rom+offset)))
	#define ReadDWord(offset) (*((DWORD*)(rom+offset)))
	#define SReadByte(offset) (*((BYTE*)(rom+snes2pc(offset))))
	#define SReadWord(offset) (*((WORD*)(rom+snes2pc(offset))))
	#define SReadDWord(offset) (*((DWORD*)(rom+snes2pc(offset))))
    DWORD snes2pc(int snesAddress);
    DWORD pc2snes(int pcAddress);
	
	void Init();
	void Save();
	void Exit();
	void LoadHeader();
	
	void tile2bpp2raw(LPBYTE src, LPBYTE dst);
	void tile4bpp2raw(LPBYTE src, LPBYTE dst);
	void raw2tile2bpp(LPBYTE src, LPBYTE dst);
	void raw2tile4bpp(LPBYTE src, LPBYTE dst);

	WORD Convert16Color(WORD color);
	DWORD ConvertRGBColor(WORD color);
	DWORD ConvertBGRColor(WORD color);
	WORD ConvertRGB2SNES(DWORD color);
	WORD Get16Color(int address);
	DWORD GetRGBColor(int address);
	DWORD GetBGRColor(int address);
	RGBQUAD GetRGBQuad(int address);
};