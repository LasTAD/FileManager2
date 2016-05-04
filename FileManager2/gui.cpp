#include "gui.h"
#include "files.h"
#include <sstream>

// ������� �������� ����� �����/���������� �� ������������
/*
bool validateFilename(wstring str) {
	if (str.length() > 0 && str.find(L'*') == wstring::npos && str.find(L'|') == wstring::npos &&str.find(L'\\') == wstring::npos &&
		str.find(L':') == wstring::npos &&str.find(L'"') == wstring::npos &&str.find(L'<') == wstring::npos &&
		str.find(L'>') == wstring::npos &&str.find(L'?') == wstring::npos &&str.find(L'/') == wstring::npos) return true;
	return false;
}
*/

void showCursor(HANDLE hout, DWORD size)
{
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = size;
	cci.bVisible = true;
	SetConsoleCursorInfo(hout, &cci);
}

void hideCursor(HANDLE hout)
{
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 20;
	cci.bVisible = false;
	SetConsoleCursorInfo(hout, &cci);
}

void setCursorPosition(HANDLE hout, short x, short y)
{
	SetConsoleCursorPosition(hout, { x,y });
}

void drawWindow(CHAR_INFO* buf, int width, int height, int color, wstring caption)
{
	// check win size
	// HEADER
	buf[0].Char.UnicodeChar = 9484;
	buf[0].Attributes = color;
	buf[1].Char.UnicodeChar = 9472;
	buf[1].Attributes = color;
	if (caption.empty()) {
		for (int i = 2; i < width - 2; ++i) {
			buf[i].Char.UnicodeChar = 9472;
			buf[i].Attributes = color;
		}
	}
	else {
		buf[2].Char.UnicodeChar = L' ';
		buf[2].Attributes = color;
		if (caption != L"") {
			if ((int)caption.length() > width - 6) caption = caption.substr(0, width - 9) + L"...";
			for (int i = 3, j = 0; i < width - 2; ++i, ++j) {
				buf[i].Attributes = color;
				if (j == caption.length() || (i == width - 3 && j < (int)caption.length()))
					buf[i].Char.UnicodeChar = L' ';
				else if (j < (int)caption.length())
					buf[i].Char.UnicodeChar = caption[j];
				else
					buf[i].Char.UnicodeChar = 9472;
			}
		}
		else {
			for (int i = 2; i < width - 2; ++i) {
				buf[i].Char.UnicodeChar = 9472;
				buf[i].Attributes = color;
			}
		}
	}
	buf[width - 2].Char.UnicodeChar = 9472;
	buf[width - 2].Attributes = color;
	buf[width - 1].Char.UnicodeChar = 9488;
	buf[width - 1].Attributes = color;
	// BODY
	for (int i = 1; i < height - 1; ++i) {
		buf[width*i].Char.UnicodeChar = 9474;
		buf[width*i].Attributes = color;
		for (int j = 1; j < width - 1; j++) {
			buf[width*i + j].Char.UnicodeChar = L' ';
			buf[width*i + j].Attributes = color;
		}
		buf[width*(i + 1) - 1].Char.UnicodeChar = 9474;
		buf[width*(i + 1) - 1].Attributes = color;
	}
	// FOOTER
	buf[width*(height - 1)].Char.UnicodeChar = 9492;
	buf[width*(height - 1)].Attributes = color;
	for (int i = width*(height - 1) + 1; i < width*height - 1; ++i) {
		buf[i].Char.UnicodeChar = 9472;
		buf[i].Attributes = color;
	}
	buf[width*height - 1].Char.UnicodeChar = 9496;
	buf[width*height - 1].Attributes = color;
}

void drawText(CHAR_INFO* buf, int from, int length, wstring text)
{
	for (int i = 0; i < length; i++) {
		if (i > (int)text.length() - 1) {
			buf[from + i].Char.UnicodeChar = L' ';
		}
		else {
			buf[from + i].Char.UnicodeChar = text[i];
		}
	}
}

void drawText(CHAR_INFO* buf, int from, int length, wstring text, int textFrom)
{
	for (int i = 0; i < length; i++) {
		if (i+textFrom >(int)text.length() - 1) {
			buf[from + i].Char.UnicodeChar = L' ';
		}
		else {
			buf[from + i].Char.UnicodeChar = text[i+textFrom];
		}
	}
}

void fillColor(CHAR_INFO* buf, int from, int length, int color)
{
	for (int i = 0; i < length; ++i) {
		buf[from + i].Attributes = color;
	}
}

bool showDialogWindowYN(HANDLE hout, wstring text, wstring caption)
{
	// �������� ��������� ��������
	bool key = false;

	CHAR_INFO old[64 * 7];
	SMALL_RECT sr;
	sr.Top = 16;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 23;
	ReadConsoleOutputW(hout, old, { 64, 7 }, { 0, 0 }, &sr);

	CHAR_INFO wnd[64 * 7];
	drawWindow(wnd, 64, 7, TextWhite | BgBlue, caption);
	drawText(wnd, 64 * 2 + 2, 60, text + L" [�/�]");
	// ������
	drawText(wnd, 64 * 4 + 3, 2, L"��");
	fillColor(wnd, 64 * 4 + 2, 4, TextWhite | BgBlue);
	drawText(wnd, 64 * 4 + 8, 3, L"���");
	fillColor(wnd, 64 * 4 + 7, 5, TextWhite | BgCyan);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 7 }, { 0,0 }, &sr);
	while (true) {
		int b = _getch();
		if (b == 0) {
			b = _getch();
			// f �������
		}
		else if (b == 0xE0) {
			b = _getch();
			if (b == 77 || b == 75) {
				key = !key;
				SMALL_RECT bt;
				bt.Top = 20;
				bt.Left = 34;
				bt.Right = 44;
				bt.Bottom = 21;
				drawText(wnd, 64 * 4 + 3, 2, L"��");
				fillColor(wnd, 64 * 4 + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 4 + 8, 3, L"���");
				fillColor(wnd, 64 * 4 + 7, 5, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				WriteConsoleOutputW(hout, wnd, { 64, 7 }, { 2, 4 }, &bt);
			}
		}
		else {
			if (b == 108 || b == 76 || b == 164 || b == 132) {
				key = true;
				break;
			}
			if (b == 121 || b == 89 || b == 173 || b == 141) {
				key = false;
				break;
			}
			if (b == 13) break;
			if (b == 27) {
				key = false;
				break;
			}
		}
	}
	WriteConsoleOutputW(hout, old, { 64, 7 }, { 0,0 }, &sr);
	return key;
}

void showDialogWindowOk(HANDLE hout, int color, int btColor, wstring text, wstring caption)
{
	CHAR_INFO old[64 * 7];
	SMALL_RECT sr;
	sr.Top = 16;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 23;
	ReadConsoleOutputW(hout, old, { 64, 7 }, { 0, 0 }, &sr);

	CHAR_INFO wnd[64 * 7];
	drawWindow(wnd, 64, 7, color, caption);
	drawText(wnd, 64 * 2 + 2, 60, text);
	// ������
	drawText(wnd, 64 * 4 + 3, 2, L"��");
	fillColor(wnd, 64 * 4 + 2, 4, btColor);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 7 }, { 0,0 }, &sr);
	while (true) {
		int b = _getch();
		if (b == 0) {
			// f �������
		}
		else if (b == 0xE0) {
			b = _getch();
		}
		else {
			if (b == 13) break;
		}
	}
	WriteConsoleOutputW(hout, old, { 64, 7 }, { 0,0 }, &sr);
}

void showDialogWindowErrorOk(HANDLE hout, wstring text, wstring caption)
{
	showDialogWindowOk(hout, TextWhite | BgMagenta, TextWhite | BgLightMagenta, text, caption);
}

_input showDialogWindowInputOkCancel(HANDLE hout, wstring text, wstring caption, validateInput f)
{
	// �������� ��������� ��������
	bool key = true;
	wstring data;
	int cursorPos = 0, first = 0, last = 0;
	bool str = true;

	CHAR_INFO old[64 * 9];
	SMALL_RECT sr;
	sr.Top = 15;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 24;
	ReadConsoleOutputW(hout, old, { 64, 9 }, { 0, 0 }, &sr);

	CHAR_INFO wnd[64 * 9];
	drawWindow(wnd, 64, 9, TextWhite | BgBlue, caption);
	drawText(wnd, 64 * 2 + 2, 60, text);
	// ���� �����
	fillColor(wnd, 64 * 4 + 2, 60, TextWhite | BgBlack);
	// ������
	drawText(wnd, 64 * 6 + 3, 2, L"��");
	fillColor(wnd, 64 * 6 + 2, 4, TextWhite | BgBlue);
	drawText(wnd, 64 * 6 + 8, 6, L"������");
	fillColor(wnd, 64 * 6 + 7, 8, TextWhite | BgBlue);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 0,0 }, &sr);
	// ��������� �������
	setCursorPosition(hout, 34, 19);
	showCursor(hout, 10);
	// -----------------
	while (true) {
		int b = _getwch(); // �������� �������� �� ����������, �.�. ������������ ��� unsigned short
		if (b == 0) {
			b = _getwch();
			// f �������, ���������
		}
		else if (b == 0xE0) {
			b = _getwch();
			if (b == 80) {
				if (!str) continue;
				SMALL_RECT bt;
				bt.Top = 21;
				bt.Left = 34;
				bt.Right = 48;
				bt.Bottom = 22;
				drawText(wnd, 64 * 6 + 3, 2, L"��");
				fillColor(wnd, 64 * 6 + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"������");
				fillColor(wnd, 64 * 6 + 7, 8, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				// ����� ������
				hideCursor(hout);
				str = false;
				// ------------
				WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 6 }, &bt);
				continue;
			}
			else if (b == 72) {
				if (str) continue;
				SMALL_RECT bt;
				bt.Top = 21;
				bt.Left = 34;
				bt.Right = 48;
				bt.Bottom = 22;
				drawText(wnd, 64 * 6 + 3, 2, L"��");
				fillColor(wnd, 64 * 6 + 2, 4, TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"������");
				fillColor(wnd, 64 * 6 + 7, 8, TextWhite | BgBlue);
				// ����� �����
				showCursor(hout, 10);
				str = true;
				// ------------
				WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 6 }, &bt);
				continue;
			}
			else if (!str && (b == 77 || b == 75)) {
				// ����� ���� ����������� ��������
				key = !key;
				SMALL_RECT bt;
				bt.Top = 21;
				bt.Left = 34;
				bt.Right = 48;
				bt.Bottom = 22;
				drawText(wnd, 64 * 6 + 3, 2, L"��");
				fillColor(wnd, 64 * 6 + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"������");
				fillColor(wnd, 64 * 6 + 7, 8, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 6 }, &bt);
				continue;
				// -------------------------------
			}
			if (str) {
				// ����������� ������� � ������ ������
				if (b == 75) {
					cursorPos--;
					if (cursorPos < 0) cursorPos = 0;
				}
				if (b == 77) {
					cursorPos++;
					if (cursorPos > (int)data.length()) cursorPos = (int)data.length();
				}
				if (b == 71) {
					// home
					cursorPos = 0;
				}
				if (b == 79) {
					// end
					cursorPos = (int)data.length();
				}
			}
			if (b == 83) {
				// delete
				if (cursorPos == data.length()) continue;
				data.erase(cursorPos, 1);
			}
		}
		else if (b == 9) {
			// tab, ���������
		}
		else if (b == 27) {
			key = false;
			break;
		}
		else if (b == 13) {
			// �������� �� ������������
			/*if (key && ((f != nullptr) ? !f(data) : 1)) showDialogWindowErrorOk(hout, L"��� ����� ������ ��� �������� ������������ �������", L"������"); else {
				data = L"";
				break;
			}*/
			if (!str && !key) {
				data = L"";
				break;
			}
			else {
				if ((f != nullptr) ? f(data) : 1) {
					key = true;
					break;
				}
				else {
					showDialogWindowErrorOk(hout, L"��� ����� ������ ��� �������� ������������ �������", L"������");
				}
			}
			// ------------------------
		}
		else if (str) {
			// ���� ������� ����� ������ �� �������
			if (b == 8) {
				// backspace
				if (cursorPos == 0) continue;
				data.erase(cursorPos - 1, 1);
				cursorPos--;
			}
			else {
				// ���� ������ ������� ������
				data.insert(cursorPos, 1, b);
				++cursorPos;
			}
			// ------------------------------------
		}

		// ��������� ������ ��������� ������
		last = max((int)data.length() - 1, 59);
		if (cursorPos > last) {
			last = cursorPos - 1;
			first = max(0, last - 59 + 1);
		}
		if (cursorPos < first) {
			first = cursorPos;
			last = first + 60;
		}
		// ---------------------------------
		static int oldf = 0, oldl = 0, len = 0; // ����������� ���������
		// ���� ������� ����� ������ �� �������, �� ���������� ��
		if (str) {
			setCursorPosition(hout, 34 + cursorPos - first, 19);
		}
		if (str && (oldf != first || oldl != last || len != data.length())) {
			drawText(wnd, 64 * 4 + 2, 60, data, first);
			SMALL_RECT bt;
			bt.Top = 19;
			bt.Left = 34;
			bt.Right = 96;
			bt.Bottom = 19;
			WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 4 }, &bt);
		}
		oldf = first;
		oldl = last;
		len = (int)data.length();
	}
	hideCursor(hout);
	WriteConsoleOutputW(hout, old, { 64, 9 }, { 0,0 }, &sr);
	return { data == L"" || !key, data };
}

void drawWindowFM(HANDLE hout)
{
	CHAR_INFO *buf = new CHAR_INFO[128 * 40];
	// COLOR
	for (int i = 0; i < 128 * 40; ++i) {
		buf[i].Attributes = TextLightGray | BgBlack;
		buf[i].Char.UnicodeChar = L' ';
	}
	// BORDER
	// --- dir ---
	buf[0].Char.UnicodeChar = 9556;
	for (int i = 1; i < 127; ++i) { buf[i].Char.UnicodeChar = 9552; }
	buf[127].Char.UnicodeChar = 9559;
	drawText(buf, 2, 6, L" Path ");
	fillColor(buf, 2, 6, TextGreen | BgBlack);
	buf[128].Char.UnicodeChar = buf[128 + 127].Char.UnicodeChar = 9553;
	// --- top ---
	buf[0 + 256].Char.UnicodeChar = 9568;//56
	for (int i = 1; i < 85; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 2 + 256, 6, L" Name ");
	fillColor(buf, 2 + 256, 6, TextGreen | BgBlack);
	buf[85 + 256].Char.UnicodeChar = 9574;
	for (int i = 86; i < 106; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 87 + 256, 6, L" Size ");
	fillColor(buf, 87 + 256, 6, TextGreen | BgBlack);
	buf[106 + 256].Char.UnicodeChar = 9574;
	for (int i = 107; i < 127; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 108 + 256, 6, L" Type ");
	fillColor(buf, 108 + 256, 6, TextGreen | BgBlack);
	buf[127 + 256].Char.UnicodeChar = 9571;//59
	// -- files --
	for (int i = 1; i < 36; ++i) {
		buf[128 * i + 256].Char.UnicodeChar = 9553;
		buf[128 * i + 85 + 256].Char.UnicodeChar = 9553;
		buf[128 * i + 106 + 256].Char.UnicodeChar = 9553;
		buf[128 * i + 127 + 256].Char.UnicodeChar = 9553;
	}
	// -- footer --
	buf[38 * 128].Char.UnicodeChar = 9562;
	for (int i = 1; i < 85; ++i) { buf[38 * 128 + i].Char.UnicodeChar = 9552; }
	buf[38 * 128 + 85].Char.UnicodeChar = 9577;
	for (int i = 86; i < 106; ++i) { buf[38 * 128 + i].Char.UnicodeChar = 9552; }
	buf[38 * 128 + 106].Char.UnicodeChar = 9577;
	for (int i = 107; i < 127; ++i) { buf[38 * 128 + i].Char.UnicodeChar = 9552; }
	buf[39 * 128 - 1].Char.UnicodeChar = 9565;
	// --- help ---
	drawText(buf, 39 * 128, 128, L" \u25cf F1 HELP \u25cf F2 RENAME \u25cf F3 COPY \u25cf F4 DELETE \u25cf F5 CREATE DIR");
	// draw
	SMALL_RECT sr{ 0,0,128,40 };
	WriteConsoleOutputW(hout, buf, { 128, 40 }, { 0,0 }, &sr);
	delete[] buf;
}

void drawTableFM(HANDLE hout, vector<PWIN32_FIND_DATAW> files, int first, int last, int pos)
{
	CHAR_INFO *names = new CHAR_INFO[84 * 35], *sizes = new CHAR_INFO[20 * 35], *types = new CHAR_INFO[20 * 35];
	fillColor(names, 0, 84 * 35, TextWhite | BgBlack);
	fillColor(sizes, 0, 20 * 35, TextWhite | BgBlack);
	fillColor(types, 0, 20 * 35, TextWhite | BgBlack);
	fillColor(names, (pos - first) * 84, 84, TextBlack | BgWhite);
	fillColor(sizes, (pos - first) * 20, 20, TextBlack | BgWhite);
	fillColor(types, (pos - first) * 20, 20, TextBlack | BgWhite);
	for (int f = first, i = 0; i < 35; ++f, ++i) {
		if (f > last) {
			drawText(names, i * 84, 84, L"");
			drawText(sizes, i * 20, 20, L"");
			drawText(types, i * 20, 20, L"");
			continue;
		}

		drawText(names, i * 84, 84, files[f]->cFileName);
		if (files[f]->dwReserved0 != 1 && files[f]->dwReserved0 != 2 && !(files[f]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			unsigned long long size = ((files[f]->nFileSizeHigh * MAXDWORD) + files[f]->nFileSizeLow);
			wstringstream wss; wss << size << L" B";
			drawText(sizes, i * 20, 20, wss.str());
		}
		else if (files[f]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (files[f]->dwReserved1 == 1) {
				wstringstream wss; wss << files[f]->nFileSizeLow << L" files, " << files[f]->nFileSizeHigh << L" dirs";
				drawText(sizes, i * 20, 20, wss.str());
			}
			else {
				drawText(sizes, i * 20, 20, L"unknown");
			}
		}
		else {
			drawText(sizes, i * 20, 20, L"");
		}
		wstring type = L"File";
		if (files[f]->dwReserved0 == 1) type = L"";
		if (files[f]->dwReserved0 == 2) type = L"Drive";
		else if (files[f]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) type = L"Directory";
		drawText(types, i * 20, 20, type);
	}
	SMALL_RECT sr{ 1, 3, 84, 37 };
	WriteConsoleOutputW(hout, names, { 84,35 }, { 0,0 }, &sr);
	sr = { 86, 3, 105, 37 };
	WriteConsoleOutputW(hout, sizes, { 20,35 }, { 0,0 }, &sr);
	sr = { 107, 3, 126, 37 };
	WriteConsoleOutputW(hout, types, { 20,35 }, { 0,0 }, &sr);
	delete[] names;
	delete[] sizes;
	delete[] types;
}

void drawSelectRow(HANDLE hout, int pos, int color)
{
	CHAR_INFO *buf = new CHAR_INFO[126];
	SMALL_RECT sr{ 1,(short)pos+3,126,(short)pos+4 };
	ReadConsoleOutputW(hout, buf, { 126,1 }, { 0,0 }, &sr);
	for (int i = 0; i < 126; ++i) {
		if (i == 84 || i == 105) continue;
		buf[i].Attributes = color;
	}
	WriteConsoleOutputW(hout, buf, { 126,1 }, { 0,0 }, &sr);
	delete[] buf;
}

void drawPath(HANDLE hout, wstring path, int shift)
{
	CHAR_INFO *buf = new CHAR_INFO[126];
	SMALL_RECT sr{ 1,1,126,2 };
	fillColor(buf, 0, 126, TextWhite | BgBlack);
	drawText(buf, 0, 126, path, path.length() > 126 ? shift : 0);
	WriteConsoleOutputW(hout, buf, { 126,1 }, { 0,0 }, &sr);
	delete[] buf;
}
