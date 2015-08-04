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

MMXCore nmmx;
RenderED render;

HWND hWID[6];
HINSTANCE hInst;
Toolbar tb;
// Procedures
BOOL CALLBACK PalettesProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK TilesProc    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK MapProc      (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK BlockProc    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SceneProc    (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK ImportParam  (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK SettingsProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutProc    (HWND, UINT, WPARAM, LPARAM);

// Imports
extern void RepaintAll();
extern void LoadLevel(WORD level);
extern void RefreshLevel();
extern void ScrollProc(HWND, bool, WPARAM, short*);
extern void SetWindowTitle(HWND, CSTRING);
extern bool IsFileExist(LPSTR);
extern void FreezeMenu(HWND, WORD id, bool freeze);

// Paint vars
PAINTSTRUCT ps;
HDC hDC, hBackDC, hLevelDC;
HBITMAP backBuffer, levelBuffer;
HBITMAP bmpTeleport, bmpXSprite;
HBRUSH hBrushBlack;

short winWidth, winHeight;
short cameraX, cameraY;
bool drawLevelBuffer = false;
bool drawCheckpointInfo = false;
bool drawLevelInfo = true;
bool drawBackground = false;
bool drawCollisionIndex = false;
RECT rectCPInitialCamera;

char   importName[MAX_PATH];
LPBYTE importFile;
DWORD  importSize;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rc;
	switch (message)
	{
	case WM_PAINT:
	{
		hDC = BeginPaint(hWnd, &ps);

		hBackDC  = CreateCompatibleDC(NULL);
		hLevelDC = CreateCompatibleDC(NULL);

		if (backBuffer  == NULL) backBuffer  = CreateCompatibleBitmap(hDC, nmmx.levelWidth<<8, nmmx.levelHeight<<8);
		if (levelBuffer == NULL) levelBuffer = CreateCompatibleBitmap(hDC, nmmx.levelWidth<<8, nmmx.levelHeight<<8);
		SelectObject(hBackDC,  backBuffer);
		SelectObject(hLevelDC, levelBuffer);

		if (drawLevelBuffer)
		{
			LPBYTE tmpLayout = nmmx.sceneLayout;
			for(int y=0; y<nmmx.levelHeight; y++)
				for(int x=0; x<nmmx.levelWidth; x++)
					render.RenderSceneEx(hLevelDC, x, y, *tmpLayout++);

			if (drawCollisionIndex)
			{
				tmpLayout = nmmx.sceneLayout;
				for(int y=0; y<nmmx.levelHeight; y++)
					for(int x=0; x<nmmx.levelWidth; x++)
						render.ShowCollisionIndex(hLevelDC, x, y, *tmpLayout++);
			}
			drawLevelBuffer = false;
		}
		if (drawCheckpointInfo && !drawBackground)
		{
			//DrawSprite(hWnd, hLevelDC, bmpTeleport, nmmx.checkpointInfo->chX, nmmx.checkpointInfo->chY, 8, 48, 0, 0);
			rectCPInitialCamera.left = nmmx.checkpointInfo->camX;
			rectCPInitialCamera.top = nmmx.checkpointInfo->camY;
			rectCPInitialCamera.right = nmmx.checkpointInfo->camX + 256;
			rectCPInitialCamera.bottom = nmmx.checkpointInfo->camY + 224;
			DrawFocusRect(hLevelDC, &rectCPInitialCamera);
			drawCheckpointInfo = false;
		}

		FillRect(hBackDC, &ps.rcPaint, hBrushBlack);
		BitBlt(hBackDC, 0, 0, winWidth, winHeight, hLevelDC, cameraX, cameraY, SRCCOPY);
		if (drawLevelInfo) WriteDebugInfo(hBackDC);
		BitBlt(hDC, 0, 28, winWidth, winHeight, hBackDC, 0, 0, SRCCOPY);
		DeleteDC(hBackDC);
		DeleteDC(hLevelDC);
		EndPaint(hWnd, &ps);
		break;
	}
	case WM_MOVE:
		if (!IsZoomed(hWnd))
		{
			GetWindowRect(hWnd, &rc);
			set.WinMainPos.X = (SHORT)rc.left;
			set.WinMainPos.Y = (SHORT)rc.top;
		}
		break;
	case WM_SIZE:
	{
		GetWindowRect(hWnd, &rc);
		winWidth = (short)(rc.right - rc.left);
		winHeight = (short)(rc.bottom - rc.top);
		if (!IsZoomed(hWnd))
		{
			set.WinMainSize.X = winWidth;
			set.WinMainSize.Y = winHeight;
		}
		SendMessage(GetDlgItem(hWnd, IDT_TOOLBAR), TB_AUTOSIZE, 0, 0);
		break;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_OPEN:
			char fileNameTmp[MAX_PATH];
			OpenFileDialog(hWnd, "Megaman X ROM (*.smc)\0*.smc\0All Files (*.*)\0*.*\0", fileNameTmp);
			if (nmmx.LoadNewRom(fileNameTmp))
			{
				if (!nmmx.CheckROM())
				{
					//MessageBox(hWnd, "Wrong ROM. Please open MMX1, X2 or X3 ROM.", "Error", MB_ICONERROR);
					MessageBox(hWnd, "Wrong ROM. Please open MMX1 US/EU ROM.", "Error", MB_ICONERROR);
					nmmx.FreeRom();
					RepaintAll();
					InvalidateRect(hWnd, NULL, false);
					return 0;
				}
				SetWindowTitle(hWnd, fileNameTmp);
				nmmx.LoadFont();
				render.Init(&nmmx);
				render.CreateMapCache(hWnd);
				render.CreateFontCache(hWnd);
				render.RefreshFontCache();
				LoadLevel(0);
				RepaintAll();
				SetScrollRange(hWnd, SB_HORZ, 0, (nmmx.levelWidth<<8), true);
				SetScrollRange(hWnd, SB_VERT, 0, (nmmx.levelHeight<<8), true);
				for(int i=0; i<4; i++)
					strcpy(set.lastroms[4-i], set.lastroms[3-i]);
				strcpy(set.lastroms[0], nmmx.filePath);
			}
			break;
		case ID_FILE_SAVE:
		{
			byte tvram[0x8000];
			ZeroMemory(nmmx.vram, 0x8000);
			for(int i=0; i<0x400; i++)
				nmmx.raw2tile4bpp(nmmx.vramCache+(i*0x40), nmmx.vram+(i*0x20));
			WORD newSize = GFXRLECmp(nmmx.vram+0x200, tvram, nmmx.tileCmpSize);
			if (newSize > nmmx.tileCmpRealSize)
				switch(MessageBox(hWnd, "The compressed tiles size is more than the original compress size.\nYou can corrupt the data of ROM if you don't know what are you doing.\nAre you sure to continue with the compression?",
					"Warning", MB_ICONWARNING | MB_YESNOCANCEL))
			{
				case IDYES:
COMPRESS_TILES:
					memcpy(nmmx.rom + nmmx.tileCmpPos, tvram, newSize);
					if (nmmx.tileCmpRealSize > newSize)
						ZeroMemory(nmmx.rom + nmmx.tileCmpPos + newSize, nmmx.tileCmpRealSize - newSize);
					break;
				case IDNO:
					break;
				case IDCANCEL:
					return 0;
			}
			else goto COMPRESS_TILES;
			memcpy(nmmx.rom + nmmx.tileDecPos, nmmx.vram + nmmx.tileDecDest, nmmx.tileDecSize);

			nmmx.SaveRom(nmmx.filePath);
			nmmx.LoadLevel();
			render.RefreshMapCache();
			RepaintAll();
			break;
		}
		case IDT_PLAY:
			if (IsFileExist(set.emupath))
			{
LOC_RUNEMU:
				STARTUPINFO startupInfo;
				PROCESS_INFORMATION processInfo;
				memset(&startupInfo, 0, sizeof(STARTUPINFO));
				memset(&processInfo, 0, sizeof(PROCESS_INFORMATION));
				startupInfo.cb = sizeof(STARTUPINFO);
				startupInfo.dwFlags = STARTF_USESHOWWINDOW | 0x00002000/*STARTF_PREVENTPINNING*/;
				startupInfo.wShowWindow = SW_SHOWDEFAULT;
				char cmd[MAX_PATH+1];
				sprintf_s(cmd, " %s", nmmx.filePath);
				CreateProcess(set.emupath, cmd, NULL, NULL, false, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInfo);
				WaitForSingleObject(processInfo.hProcess, INFINITE);
			}
			else
			{
				if (OpenFileDialog(hWnd, "SNES Emulator\0*.exe", set.emupath))
					goto LOC_RUNEMU;
			}
			break;
		case ID_IMPORT_SNES:
			importFile = OpenFileRam(OpenFileDialog(hWnd, "Super Nintendo VRAM\0*.*\0", importName), &importSize);
			for(int i=0; i<importSize/0x20; i++)
				nmmx.tile4bpp2raw(importFile+(i*0x20), nmmx.vramCache+(i*0x40));
			Free(importFile);
			break;
		case ID_IMPORT_GEN:
			importFile = OpenFileRam(OpenFileDialog(hWnd, "Genesis VRAM\0*.*\0", NULL), &importSize);
			Free(importFile);
			break;
		case ID_IMPORT_GBA:
			importFile = OpenFileRam(OpenFileDialog(hWnd, "Gameboy Advance VRAM\0*.*\0", NULL), &importSize);
			Free(importFile);
			break;
		case IDT_BACKGROUND:
			goto LOC_BACKGROUND;
			break;
		case IDT_COLLISION:
			goto LOC_COLLISION;
			break;
		case IDT_DEBUG:
			goto LOC_DEBUG;
			break;
		case IDT_POINTERS:
			break;
		case ID_EDITOR_PALED:
			if (!FindWindow(NULL, IDS_PALETTE))
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_PAL), 0, (DLGPROC)PalettesProc, NULL);
			break;
		case ID_EDITOR_TILESETED:
			if (!FindWindow(NULL, IDS_TILESET))
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_TILESET), 0, (DLGPROC)TilesProc, NULL);
			break;
		case ID_EDITOR_MAPED:
			if (!FindWindow(NULL, IDS_TILEMAP))
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_MAP), 0, (DLGPROC)MapProc, NULL);
			break;
		case ID_EDITOR_BLOCKED:
			if (!FindWindow(NULL, IDS_BLOCK))
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_BLOCK), 0, (DLGPROC)BlockProc, NULL);
			break;
		case ID_EDITOR_SCENED:
			if (!FindWindow(NULL, IDS_SCENE))
				DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SCENE), 0, (DLGPROC)SceneProc, NULL);
			break;
		case ID_FILE_SETTINGS:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hWnd, (DLGPROC)SettingsProc, NULL);
			break;
		case IDM_ABOUT:
			DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)AboutProc, NULL);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_KEYDOWN:
		{
			switch (LOWORD(wParam))
			{
			case VK_UP:
				if (cameraY > 0) cameraY-=16;
				SetScrollPos(hWnd, SB_VERT, cameraY, true);
				InvalidateRect(hWnd, NULL, false);
				break;
			case VK_DOWN:
				if (cameraY < (nmmx.levelHeight<<8)) cameraY+=16;
				SetScrollPos(hWnd, SB_VERT, cameraY, true);
				InvalidateRect(hWnd, NULL, false);
				break;
			case VK_LEFT:
				if (cameraX > 0) cameraX-=16;
				SetScrollPos(hWnd, SB_HORZ, cameraX, true);
				InvalidateRect(hWnd, NULL, false);
				break;
			case VK_RIGHT:
				if (cameraX < (nmmx.levelWidth<<8)) cameraX+=16;
				SetScrollPos(hWnd, SB_HORZ, cameraX, true);
				InvalidateRect(hWnd, NULL, false);
				break;
			case VK_PRIOR:
				if (nmmx.level > 0) nmmx.level--;
				set.lastLevel = nmmx.level;
				LoadLevel(nmmx.level);
				drawBackground = false;
				break;
			case VK_NEXT:
				if (nmmx.level < 12) nmmx.level++;
				set.lastLevel = nmmx.level;
				LoadLevel(nmmx.level);
				drawBackground = false;
				break;
			case 'D':
LOC_DEBUG:
				drawLevelInfo = !drawLevelInfo;
				InvalidateRect(hWnd, NULL, false);
				break;
			case 'Q':
				if (nmmx.checkpointInfo->objLoad > 0)
				{
					nmmx.checkpointInfo->objLoad--;
				}
				break;
			case 'W':
				if (nmmx.checkpointInfo->objLoad < 0x40)
				{
					nmmx.checkpointInfo->objLoad++;
				}
				break;
			case 'A':
				if (nmmx.checkpointInfo->tileLoad > 0)
				{
					nmmx.checkpointInfo->tileLoad--;
					RefreshLevel();
				}
				break;
			case 'S':
				if (nmmx.checkpointInfo->tileLoad < 0x40)
				{
					nmmx.checkpointInfo->tileLoad++;
					RefreshLevel();
				}
				break;
			case 'Z':
				if (nmmx.checkpointInfo->palLoad > 0)
				{
					nmmx.checkpointInfo->palLoad--;
					RefreshLevel();
				}
				break;
			case 'X':
				if (nmmx.checkpointInfo->palLoad < 0x40)
				{
					nmmx.checkpointInfo->palLoad++;
					RefreshLevel();
				}
				break;
			case 'B':
LOC_BACKGROUND:
				drawBackground = !drawBackground;
				RefreshLevel();
				break;
			case 'O':
				if (nmmx.point > 0) nmmx.SetLevel(nmmx.level, nmmx.point-1);
				nmmx.LoadTilesAndPalettes();
				render.RefreshMapCache();
				RepaintAll();
				break;
			case 'P':
				if (nmmx.point < 0x40) nmmx.SetLevel(nmmx.level, nmmx.point+1);
				nmmx.LoadTilesAndPalettes();
				render.RefreshMapCache();
				RepaintAll();
				break;
			case 'E':
LOC_COLLISION:
				drawCollisionIndex = !drawCollisionIndex;
				DeleteObject(levelBuffer);
				levelBuffer = NULL;
				drawLevelBuffer = true;
				InvalidateRect(hWnd, NULL, false);
				break;
			case 'C':
				drawCheckpointInfo = !drawCheckpointInfo;
				InvalidateRect(hWnd, NULL, false);
				break;
			}
		}
		break;
	case WM_SYSCOMMAND:
		if (wParam == SC_CLOSE)
			set.max = IsZoomed(hWnd);
		return DefWindowProc(hWnd, message, wParam, lParam);
	case WM_CREATE:
		hWID[0] = hWnd;
		hBrushBlack = CreateSolidBrush(RGB(0, 0, 0));
		CreateBitmapCache();
		bmpTeleport = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TELEPORT));
		bmpXSprite = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_XSPRITE));

		tb.Create(hWnd, hInst, IDB_TOOLBAR, IDT_TOOLBAR);
		tb.AddButton(TBSTYLE_SEP   , 0);
		tb.AddButton(TBSTYLE_BUTTON, ID_FILE_OPEN);
		tb.AddButton(TBSTYLE_BUTTON, ID_FILE_SAVE);
		tb.AddButton(TBSTYLE_SEP   , 0);
		tb.AddButton(TBSTYLE_BUTTON, IDT_PLAY);
		tb.AddButton(TBSTYLE_BUTTON, ID_FILE_SETTINGS);
		tb.AddButton(TBSTYLE_SEP   , 0);
		tb.AddButton(TBSTYLE_BUTTON, IDT_BACKGROUND);
		tb.AddButton(TBSTYLE_BUTTON, IDT_COLLISION);
		tb.AddButton(TBSTYLE_BUTTON, IDT_DEBUG);
		tb.AddButton(TBSTYLE_BUTTON, IDT_POINTERS);
		tb.AddButton(TBSTYLE_SEP   , 0);
		tb.AddButton(TBSTYLE_BUTTON, ID_EDITOR_PALED);
		tb.AddButton(TBSTYLE_BUTTON, ID_EDITOR_TILESETED);
		tb.AddButton(TBSTYLE_BUTTON, ID_EDITOR_MAPED);
		tb.AddButton(TBSTYLE_BUTTON, ID_EDITOR_BLOCKED);
		tb.AddButton(TBSTYLE_BUTTON, ID_EDITOR_SCENED);
		tb.Display();

		if (set.romAtStartup)
		{
			if (nmmx.LoadNewRom(set.lastroms[0]))
			{
				if (!nmmx.CheckROM())
					return 0;
				strcpy(nmmx.filePath, set.lastroms[0]);
				SetWindowTitle(hWnd, set.lastroms[0]);
				nmmx.LoadFont();
				render.Init(&nmmx);
				render.CreateMapCache(hWnd);
				render.CreateFontCache(hWnd);
				render.RefreshFontCache();
				LoadLevel(set.lastLevel);
				SetScrollRange(hWnd, SB_HORZ, 0, (nmmx.levelWidth<<8), true);
				SetScrollRange(hWnd, SB_VERT, 0, (nmmx.levelHeight<<8), true);
			}
		}
		break;
	case WM_DESTROY:
		DeleteObject(bmpTeleport);
		DeleteObject(bmpXSprite);
		DeleteObject(hBrushBlack);
		SaveSettings();
		PostQuitMessage(0);
		break;
	case WM_HSCROLL:
	case WM_VSCROLL:
	{
		ScrollProc(hWnd, message == 0x115, wParam, message == WM_HSCROLL ? &cameraX : &cameraY);
		if (cameraY > (nmmx.levelHeight<<8)) cameraY = nmmx.levelHeight<<8;
		if (cameraX > (nmmx.levelWidth <<8)) cameraX = nmmx.levelWidth <<8;
		InvalidateRect(hWnd, NULL, false);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
