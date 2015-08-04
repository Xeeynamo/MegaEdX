#include "MegaED X.h"
#include <stdio.h>

// NDS CORE

// Xeeynamo Core
#ifdef NOPUBLIC
void XeeyCore::EnumFS()
{
	fsElements = *wRom++;
	xFile = (XFILE*)malloc(fsElements * sizeof(XFILE));

	int pos = 0;
	for(int i=0; i<fsElements; i++)
	{
		xFile[i].setID(*wRom++);
		xFile[i].size = (*wRom++);
		xFile[i].position = pos;
		pos += xFile[i].size;
	}
}




#endif