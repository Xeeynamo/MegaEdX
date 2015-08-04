#ifndef MEGAEDX
#define MEGAEDX
#include "stdafx.h"

#include "MegaEDCore.h"
#include "SNESCore.h"
#include "NDSCore.h"
#include "MMXCore.h"
#include "RenderED.h"
#include "CompressionCore.h"
////////////////////////
#include "Settings.h"
#include "FileDialog.h"
#include "WinCtrl.h"
#include "Toolbar.h"
#include "resource.h"
#include "ResourceX.h"
#define byte unsigned char
#define u16 unsigned short
#define u32 unsigned int

// MegaED X.cpp
extern bool drawLevelBuffer;

extern RenderED render;
extern HINSTANCE hInst;
extern HWND hWID[6];
void RepaintAll();
void RefreshLevel();

// DebugInfo.cpp
extern char out[0x40];
extern unsigned char debugInfoIndex;
extern bool debugEnabled;
void WriteDebugInfo(HDC);

// WinControls
void ShowLastError(DWORD);
// Super Functions
void SetWindowPosition(HWND, SHORT x, SHORT y);

HBITMAP CreateBitmapCache(HWND hWnd, int width, int height);
void ErrorBox(HWND, LPCTSTR, DWORD);
void InitToolBar(HWND hWnd);
void InitStatusBar(HWND hWnd);
void memdwcopy(DWORD *dst, DWORD *src, int size);
void print(LPSTR text);
void print(LPSTR text, int param);
void FileDebugOut(void *data, DWORD dataSize, STRING fileName);
void CreateBitmapCache();
void RenderImage(HDC hdc, int x, int y, WORD image[], WORD param);
void RenderImageZoom(HDC hdc, int x, int y, WORD image[], int size);
bool OpenFileDialogCore(HWND hWnd, CSTRING filter, MegaEDCore *romcore);

extern MMXCore nmmx;

extern const unsigned char vrambase[];
extern const int vrambase_size;

#endif