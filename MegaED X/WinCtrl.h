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

class ScrollBar
{
private:
	HWND hCtrl;
	int value;
public:
	void Create(HWND hWnd, const int x, const int y, const int height);
	void GetRange(int *min, int *max);
	void SetRange(const int min, const int max);
	int  GetPos();
	void SetPos(int pos);
	void Work(WPARAM wParam);
	long GetID();
	bool IsIDEqual(long);
};

class SpinBox
{
private:
	HWND hCtrl;
	HWND hBox;
	HFONT hFont;
	int value;
	int maxval;
	char text[8];
public:
	~SpinBox();
	void Create(HWND hWnd, WORD id, const int x, const int y, const int width, WORD min, WORD max);
	void GetRange(int *min, int *max);
	void SetRange(const int min, const int max);
	int  GetPos();
	void SetPos(int pos);
	void Work(WPARAM);
	DWORD ParseText(char*);
	long GetID();
	bool IsIDEqual(long);
};

class StatusBar
{
private:
	HWND hCtrl;
public:
	void Create(HWND hWnd, HINSTANCE hInst);
	void SetText(STRING text);
};