#include "MegaED X.h"

class MegaText
{
private:
	BYTE vtext[0x8000];
	WORD palCache[0x20];
	_MMXCore *mmxc;

	HBITMAP bmpText;
	void *bmpData;
public:
	~MegaText();
	void Init(_MMXCore*);
	void RenderTile2bpp(int, LPWORD);
	void PrintText(HDC, LPCSTR text, int x, int y, int palette);
};