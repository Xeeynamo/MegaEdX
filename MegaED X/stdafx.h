#pragma once

#include "targetver.h"
//#include <SDKDDKVer.h>

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <WinGDI.h>
#include <CommDlg.h>
#include <CommCtrl.h>
#include <ShellAPI.h>

// Libraries
#pragma comment(lib, "ComDlg32.lib")
#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Msimg32.lib") // Used for TransparentBlt
#pragma comment(lib, "shell32.lib") // Used fot ShellExecute

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>

// TODO: reference additional headers your program requires here
#ifdef UNICODE
#define STRING  LPWSTR
#define CSTRING LPCWSTR
#else
#define STRING  LPSTR
#define CSTRING LPCSTR
#endif

#define MAX_STRING 260

#ifdef WIN32X
#define Malloc(size)		HeapAlloc(GetProcessHeap(), NULL, size)
#define MallocZ(size)		HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#define Free(p)				HeapFree(GetProcessHeap(), NULL, p);
#else
#include <malloc.h>
#define _CRT_SECURE_NO_WARNINGS
#define sprintf_s sprintf
#define _itoa_s _itoa
#define Malloc(size)		malloc(size)
#define MallocZ(size)	\
	x = malloc(size); \
	memset(x, 0, size);
#define Free(p)				free(p)
#endif
