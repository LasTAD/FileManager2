#include "gui.h"
#include "files.h"
#include "app.h"
#include "types.h"
#include <exception>
#include <sstream>

wstring HELP_STRING = L" \u25cf F1 HELP \u25cf F2 RENAME \u25cf F3 COPY \u25cf F4 DELETE \u25cf F5 CREATE DIRECTORY \u25cf F6 ARCHIVING \u25cf F7 UNARCHIVING";

// функция для подсчета необходимого количества строк в окне
int estimateString(wstring str, int onestrlen, int maxstrlen = -1) {
	int counter = 1;
	for (int i = 0, j = 1; i < str.length(); ++i, ++j) {
		if (str[i] == '\n' || (j == onestrlen && i != str.length() - 1)) {
			++counter;
			j = 0;
		}
	}
	return maxstrlen == -1 ? counter : min(maxstrlen, counter);
}

// функция для преобразования числа в шестнадцатеричную строку
template<typename ___int>
wstring intToHexString(___int i, int length = 0) {
	wchar_t *buf = new wchar_t[max(17, length)];
	wstring format;
	if (length == 0) {
		format = L"%X";
	}
	else {
		format = L"%0" + to_wstring(length) + L"X";
	}
	_snwprintf_s(buf, max(17, length), max(17, length), format.c_str(), i);
	format = buf;
	delete buf;
	return format;
}

// функция для преобразования шестнадцатеричного в число
uint64 stringToInt(wstring s) {
	if (s.length() == 0) return 0;
	uint64 n = 0;
	if (s[0] == L'0' && (s[1] == L'x' || s[1] == L'X')) { // hex
		s = s.substr(2);
		// count
		uint64 m = 1;
		for (size_t i = s.length() - 1; i != 0xFFFFFFFFFFFFFFFF; --i) {
			if (s[i] >= L'0' && s[i] <= L'9') {
				n += (s[i] - L'0') * m;
				m *= 16;
			}
			else if (s[i] >= L'a' && s[i] <= L'f') {
				n += (s[i] - L'a' + 10) * m;
				m *= 16;
			}
			else if (s[i] >= L'A' && s[i] <= L'F') {
				n += (s[i] - L'A' + 10) * m;
				m *= 16;
			}
			else {
				throw exception("stringToInt: invalid string");
			}
		}
	}
	else { // dec
		// count
		uint64 m = 1;
		for (size_t i = s.length() - 1; i != 0xFFFFFFFFFFFFFFFF; --i) {
			if (s[i] >= L'0' && s[i] <= L'9') {
				n += (s[i] - L'0') * m;
				m *= 10;
			}
			else {
				throw exception("stringToInt: invalid string");
			}
		}
	}
	return n;
}

// функция проверки введенного адреса
bool validateInputAddress(wstring s) {
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) { // hex
		s = s.substr(2);
		// check
		for (size_t i = 0; i < s.length(); ++i) {
			if (!((s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'f') || (s[i] >= 'A' && s[i] <= 'F'))) {
				return false;
			}
		}
	}
	else { // dec
		// check
		for (size_t i = 0; i < s.length(); ++i) {
			if (!(s[i] >= '0' && s[i] <= '9')) {
				return false;
			}
		}
	}
	return true;
}

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

void drawText(CHAR_INFO* buf, int from, int length, wstring text, int textFrom)
{
	for (size_t i = 0; i < length; i++) {
		if ((i+textFrom+1) > text.length()) {
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
	// значение выбранной кнопочки
	bool key = false;

	short strcount = estimateString(text, 60, 32);
	CHAR_INFO *old = new CHAR_INFO[64 * (6 + strcount)];
	SMALL_RECT sr;
	sr.Top = 16 - strcount / 2;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 23 + strcount / 2;
	ReadConsoleOutputW(hout, old, { 64, 6 + strcount }, { 0, 0 }, &sr);

	CHAR_INFO *wnd = new CHAR_INFO[64 * (6 + strcount)];

	// рамка окна
	drawWindow(wnd, 64, 6 + strcount, TextWhite | BgBlue, caption);
	// ----------

	// отрисовка многострочного текста
	int sc = 1;
	wstring tail = text;
	while (tail.length() != 0) {
		size_t p = tail.find(L'\n');
		if (p != wstring::npos) {
			if (p < 60) {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, p));
				tail = tail.substr(p + 1);
			}
			else {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, 60));
				tail = tail.substr(60);
			}
		}
		else {
			if (tail.length() > 60) {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, 60));
				tail = tail.substr(60);
			}
			else {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail);
				tail = L"";
			}
		}
		++sc;
	}
	// -------------------------------
	// кнопки
	drawText(wnd, 64 * (3+strcount) + 3, 2, L"Да");
	fillColor(wnd, 64 * (3+strcount) + 2, 4, TextWhite | BgBlue);
	drawText(wnd, 64 * (3+strcount) + 8, 3, L"Нет");
	fillColor(wnd, 64 * (3+strcount) + 7, 5, TextWhite | BgCyan);
	// -------------------------------
	WriteConsoleOutputW(hout, wnd, { 64, 6 + strcount }, { 0,0 }, &sr);
	while (true) {
		int b = _getch();
		if (b == 0) {
			b = _getch();
			// f клавиши
		}
		else if (b == 0xE0) {
			b = _getch();
			if (b == 77 || b == 75) {
				key = !key;
				SMALL_RECT bt;
				bt.Top = 20 + strcount / 2;
				bt.Left = 34;
				bt.Right = 44;
				bt.Bottom = 21 + strcount / 2;
				drawText(wnd, 64 * (3 + strcount) + 3, 2, L"Да");
				fillColor(wnd, 64 * (3 + strcount) + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * (3 + strcount) + 8, 3, L"Нет");
				fillColor(wnd, 64 * (3 + strcount) + 7, 5, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				WriteConsoleOutputW(hout, wnd, { 64, 6 + strcount }, { 2, 3 + strcount }, &bt);
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
	WriteConsoleOutputW(hout, old, { 64, 6 + strcount }, { 0,0 }, &sr);
	delete old;
	delete wnd;
	return key;
}

int showDialogWindowYNC(HANDLE hout, wstring text, wstring caption, int def)
{
	// значение выбранной кнопочки
	int key = def % 3;

	CHAR_INFO old[64 * 7];
	SMALL_RECT sr;
	sr.Top = 16;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 23;
	ReadConsoleOutputW(hout, old, { 64, 7 }, { 0, 0 }, &sr);

	CHAR_INFO wnd[64 * 7];
	drawWindow(wnd, 64, 7, TextWhite | BgBlue, caption);
	drawText(wnd, 64 * 2 + 2, 60, text + L" [д/н]");
	// кнопки
	drawText(wnd, 64 * 4 + 3, 2, L"Да");
	fillColor(wnd, 64 * 4 + 2, 4, key == 0 ? TextWhite | BgCyan : TextWhite | BgBlue);
	drawText(wnd, 64 * 4 + 8, 3, L"Нет");
	fillColor(wnd, 64 * 4 + 7, 5, key == 1 ? TextWhite | BgCyan : TextWhite | BgBlue);
	drawText(wnd, 64 * 4 + 14, 6, L"Отмена");
	fillColor(wnd, 64 * 4 + 13, 8, key == 2 ? TextWhite | BgCyan : TextWhite | BgBlue);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 7 }, { 0,0 }, &sr);
	while (true) {
		int b = _getch();
		if (b == 0) {
			b = _getch();
			// f клавиши
		}
		else if (b == 0xE0) {
			b = _getch();
			if (b == 77 || b == 75) {
				int a = b == 75 ? -1 : 1;
				key = (key + a) % 3;
				if (key < 0) key += 3;
				SMALL_RECT bt;
				bt.Top = 20;
				bt.Left = 34;
				bt.Right = 52;
				bt.Bottom = 21;
				drawText(wnd, 64 * 4 + 3, 2, L"Да");
				fillColor(wnd, 64 * 4 + 2, 4, key == 0 ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 4 + 8, 3, L"Нет");
				fillColor(wnd, 64 * 4 + 7, 5, key == 1 ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 4 + 14, 6, L"Отмена");
				fillColor(wnd, 64 * 4 + 13, 8, key == 2 ? TextWhite | BgCyan : TextWhite | BgBlue);
				WriteConsoleOutputW(hout, wnd, { 64, 7 }, { 2, 4 }, &bt);
			}
		}
		else {
			if (b == 108 || b == 76 || b == 164 || b == 132) {
				key = 0;
				break;
			}
			if (b == 121 || b == 89 || b == 173 || b == 141) {
				key = 1;
				break;
			}
			if (b == 13) break;
			if (b == 27) {
				key = 2;
				break;
			}
		}
	}
	WriteConsoleOutputW(hout, old, { 64, 7 }, { 0,0 }, &sr);
	return key;
}

void showDialogWindowOk(HANDLE hout, int color, int btColor, wstring text, wstring caption)
{
	short strcount = estimateString(text, 60, 32);
	CHAR_INFO *old = new CHAR_INFO[64 * (6 + strcount)];
	SMALL_RECT sr;
	sr.Top = 16 - strcount / 2;
	sr.Left = 32;
	sr.Right = 95;
	sr.Bottom = 23 + strcount / 2;
	ReadConsoleOutputW(hout, old, { 64, 6 + strcount }, { 0, 0 }, &sr);

	CHAR_INFO *wnd = new CHAR_INFO[64 * (6 + strcount)];

	// рамка окна
	drawWindow(wnd, 64, 6 + strcount, color, caption);
	// ----------

	// отрисовка многострочного текста
	int sc = 1;
	wstring tail = text;
	while (tail.length() != 0) {
		size_t p = tail.find(L'\n');
		if (p != wstring::npos) {
			if (p < 60) {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, p));
				tail = tail.substr(p + 1);
			}
			else {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, 60));
				tail = tail.substr(60);
			}
		}
		else {
			if (tail.length() > 60) {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail.substr(0, 60));
				tail = tail.substr(60);
			}
			else {
				drawText(wnd, 64 * (1 + sc) + 2, 60, tail);
				tail = L"";
			}
		}
		++sc;
	}
	// -------------------------------

	// кнопка
	drawText(wnd, 64 * (3 + strcount) + 3, 2, L"Ок");
	fillColor(wnd, 64 * (3 + strcount) + 2, 4, btColor);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 6 + strcount }, { 0,0 }, &sr);
	delete wnd;
	while (true) {
		int b = _getch();
		if (b == 0) {
			// f клавиши
		}
		else if (b == 0xE0) {
			b = _getch();
		}
		else {
			if (b == 13) break;
			if (b == 27) break;
		}
		
	}
	WriteConsoleOutputW(hout, old, { 64, 6 + strcount }, { 0,0 }, &sr);
	delete old;
}

void showDialogWindowErrorOk(HANDLE hout, wstring text, wstring caption)
{
	showDialogWindowOk(hout, TextWhite | BgMagenta, TextWhite | BgLightMagenta, text, caption);
}


_input showDialogWindowInputOkCancel(HANDLE hout, wstring text, wstring caption, validateInput f)
{
	// значение выбранной кнопочки
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
	// поле ввода
	fillColor(wnd, 64 * 4 + 2, 60, TextWhite | BgBlack);
	// кнопки
	drawText(wnd, 64 * 6 + 3, 2, L"Ок");
	fillColor(wnd, 64 * 6 + 2, 4, TextWhite | BgBlue);
	drawText(wnd, 64 * 6 + 8, 6, L"Отмена");
	fillColor(wnd, 64 * 6 + 7, 8, TextWhite | BgBlue);
	// ------
	WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 0,0 }, &sr);
	// установка курсора
	setCursorPosition(hout, 34, 19);
	showCursor(hout, 10);
	// -----------------
	while (true) {
		int b = _getwch(); 
		if (b == 0) {
			b = _getwch();
			// f клавиши, пропустим
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
				drawText(wnd, 64 * 6 + 3, 2, L"Ок");
				fillColor(wnd, 64 * 6 + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"Отмена");
				fillColor(wnd, 64 * 6 + 7, 8, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				// режим кнопок
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
				drawText(wnd, 64 * 6 + 3, 2, L"Ок");
				fillColor(wnd, 64 * 6 + 2, 4, TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"Отмена");
				fillColor(wnd, 64 * 6 + 7, 8, TextWhite | BgBlue);
				// режим ввода
				showCursor(hout, 10);
				str = true;
				// ------------
				WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 6 }, &bt);
				continue;
			}
			else if (!str && (b == 77 || b == 75)) {
				// всего лишь перерисовка кнопочек
				key = !key;
				SMALL_RECT bt;
				bt.Top = 21;
				bt.Left = 34;
				bt.Right = 48;
				bt.Bottom = 22;
				drawText(wnd, 64 * 6 + 3, 2, L"Ок");
				fillColor(wnd, 64 * 6 + 2, 4, key ? TextWhite | BgCyan : TextWhite | BgBlue);
				drawText(wnd, 64 * 6 + 8, 6, L"Отмена");
				fillColor(wnd, 64 * 6 + 7, 8, key ? TextWhite | BgBlue : TextWhite | BgCyan);
				WriteConsoleOutputW(hout, wnd, { 64, 9 }, { 2, 6 }, &bt);
				continue;
				// -------------------------------
			}
			if (str) {
				// перемещение курсора в режиме строки
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
			// tab, пропустим
		}
		else if (b == 27) {
			key = false;
			break;
		}
		else if (b == 13) {
			// проверка на правильность
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
					hideCursor(hout);
					showDialogWindowErrorOk(hout, L"Неверный ввод", L"Ошибка");
					showCursor(hout, 10);
				}
			}
			// ------------------------
		}
		else if (str) {
			// если включен режим работы со строкой
			if (b == 8) {
				// backspace
				if (cursorPos == 0) continue;
				data.erase(cursorPos - 1, 1);
				cursorPos--;
			}
			else {
				// либо просто добавим символ
				data.insert(cursorPos, 1, b);
				++cursorPos;
			}
			// -----------------------------
		}

		// установка границ отрисовки строки
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
		static int oldf = 0, oldl = 0, len = 0; // оптимизация отрисовки
		// если включен режим работы со строкой, то перерисуем ее
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
	fillColor(buf, 2, 6, TextLightGreen | BgBlack); // Цветовая схема
	buf[128].Char.UnicodeChar = buf[128 + 127].Char.UnicodeChar = 9553;
	// --- top ---
	buf[0 + 256].Char.UnicodeChar = 9568;//56
	for (int i = 1; i < 85; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 2 + 256, 6, L" Name ");
	fillColor(buf, 2 + 256, 6, TextLightGreen | BgBlack); // Цветовая схема
	buf[85 + 256].Char.UnicodeChar = 9574;
	for (int i = 86; i < 106; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 87 + 256, 6, L" Size ");
	fillColor(buf, 87 + 256, 6, TextLightGreen | BgBlack); // Цветовая схема
	buf[106 + 256].Char.UnicodeChar = 9574;
	for (int i = 107; i < 127; ++i) { buf[i + 256].Char.UnicodeChar = 9552; }
	drawText(buf, 108 + 256, 6, L" Type ");
	fillColor(buf, 108 + 256, 6, TextLightGreen | BgBlack); // Цветовая схема
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
	fillColor(buf, 39 * 128, 128, BgGreen | TextWhite);
	drawText(buf, 39 * 128, 128, HELP_STRING);
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
		if (files[f]->dwReserved1 != dotdot && files[f]->dwReserved1 != fold && files[f]->dwReserved1 != drive && !(files[f]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			uint64 size = ((files[f]->nFileSizeHigh * (0xFFFFFFFFULL + 1ULL)) + files[f]->nFileSizeLow);
			drawText(sizes, i * 20, 20, to_wstring(size) + L" B");
		}
		else if (files[f]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (files[f]->dwReserved1 == fold) {
				wstringstream wss; wss << files[f]->nFileSizeLow << L" files, " << files[f]->nFileSizeHigh << L" folders";
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
		if (files[f]->dwReserved1 == fold) type = L"";
		if (files[f]->dwReserved1 == drive) type = L"Drive";
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

// проверена
void drawPath(HANDLE hout, wstring path, int shift)
{
	CHAR_INFO *buf = new CHAR_INFO[126];
	SMALL_RECT sr{ 1,1,126,2 };
	fillColor(buf, 0, 126, TextWhite | BgBlack);
	drawText(buf, 0, 126, path, path.length() > 126 ? shift : 0);
	WriteConsoleOutputW(hout, buf, { 126,1 }, { 0,0 }, &sr);
	delete[] buf;
}

// функция для отрисовки таблицы адресов в буфер
// проверена, возможно нужно посчитать константы
void drawEditorTable(CHAR_INFO* buf, uint64 page = 0) {
	for (int i = 0; i < 16; ++i) {
		drawText(buf, 82 * 2 + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, intToHexString(i, 2));
		fillColor(buf, 82 * 2 + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, TextRed | BgLightCyan);
	}
	for (int i = 0; i < 29; ++i) {
		drawText(buf, 82 * (i + 3) + 2, 10, intToHexString(i*16 + page*29*16, 10));
		fillColor(buf, 82 * (i + 3) + 2, 10, TextRed | BgLightCyan);
	}
}

// функция для отрисовки байтов в буфер, позицию передавать абсолютную
// проверена, возможно нужно посчитать константы
void drawEditorBody(CHAR_INFO* buf, unsigned char *bytes, uint64 len, int posx = 0, uint64 posy = 0) {
	int c = 0;
	for (int j = 0; j < 29; ++j) {
		for (int i = 0; i < 16; ++i) {
			if (c < len) {
				drawText(buf, 82 * (j + 3) + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, intToHexString(bytes[c], 2u));
				wchar_t b[2];
				b[0] = bytes[c];
				if (b[0] < 32) b[0] = L'.';
				b[1] = 0;
				drawText(buf, 82 * (j + 3) + 4 + 10 + i + 50, 1, b);
			}
			else {
				drawText(buf, 82 * (j + 3) + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, L"  ");
				wchar_t b[2];
				b[0] = L' ';
				b[1] = 0;
				drawText(buf, 82 * (j + 3) + 4 + 10 + i + 50, 1, b);
			}
			if (i == posx && j == posy % 29) {
				fillColor(buf, 82 * (j + 3) + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, TextBlack | BgWhite);
				fillColor(buf, 82 * (j + 3) + 4 + 10 + i + 50, 1, TextBlack | BgWhite);
			}
			else {
				fillColor(buf, 82 * (j + 3) + 4 + 10 + i * 3 + (i > 7 ? 1 : 0), 2, TextWhite | BgGreen);
				fillColor(buf, 82 * (j + 3) + 4 + 10 + i + 50, 1, TextWhite | BgGreen);
			}
			c++;
		}
	}
}

// функция перерисовки выделения
// проверена, возможно нужно посчитать константы
void drawEditorSelect(CHAR_INFO* buf, int posx, int posy, int oldpage, int oldposx, int oldposy) {
	fillColor(buf, 82 * (posy + 3) + 4 + 10 + posx * 3 + (posx > 7 ? 1 : 0), 2, TextBlack | BgWhite);
	fillColor(buf, 82 * (posy + 3) + 4 + 10 + posx + 50, 1, TextBlack | BgWhite);

	fillColor(buf, 82 * (oldposy + 3) + 4 + 10 + oldposx * 3 + (oldposx > 7 ? 1 : 0), 2, TextWhite | BgGreen);
	fillColor(buf, 82 * (oldposy + 3) + 4 + 10 + oldposx + 50, 1, TextWhite | BgGreen);
}

void startEditor(HANDLE hout, wstring path) {
	// предупреждение
	static bool warn = false;
	if (!warn) {
		if (!showDialogWindowYN(hout, L"В процессе работы HEX-редактор сразу записывает изменения в файл,"
			L"поэтому рекомендуем сделать копию файла. Вы согласны продолжить работу?", L"Предупреждение")) {
			return;
		}
		warn = true;
	}
	// --------------

	// показатель чтения
	bool readOnly = false;
	bool wasReadonly = false;
	// -----------------

	showStateString(L"Opening file...");
	// установка атрибута чтения в отключенный
	DWORD attr = GetFileAttributesW(path.c_str());
	if ((attr & FILE_ATTRIBUTE_READONLY) != 0) {
		attr &= ~FILE_ATTRIBUTE_READONLY;
		if (SetFileAttributesW(path.c_str(), attr)) {
			wasReadonly = true;
		}
		else {
			readOnly = true;
		}
	}
	// ---------------------------------------
	DWORD desiredAccess = GENERIC_READ;
	if (!readOnly) desiredAccess |= GENERIC_WRITE;
	HANDLE f = CreateFileW(path.c_str(), desiredAccess, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE) {
		DWORD e = GetLastError();
		showDialogWindowErrorOk(hout, errorCodeToString(e), L"Ошибка при открытии файла");
		CloseHandle(f);
		hideStateString();
		return;
	}

	// чтение размера, все отлично
	DWORD sizeHigh_tpl;
	DWORD size = GetFileSize(f, &sizeHigh_tpl);
	uint64 sizeHigh = sizeHigh_tpl;
	uint64 sizeSum = (sizeHigh * (0xFFFFFFFFULL + 1ULL)) + size, sizeCounter = sizeSum;

	if (sizeSum == 0) {
		showDialogWindowErrorOk(hout, L"Размер файла равен нулю, HEX-редактор не предназначен для добавления байтов", L"Предупреждение");
		CloseHandle(f);
		hideStateString();
		return;
	}
	// ---------------------------

	// выделение памяти, все супер
	showStateString(L"Allocating memory...");
	unsigned char *buffer = (unsigned char*) malloc(29 * 16);
	if (!buffer) { // эта ошибка никогда возможно не произойдет
		showDialogWindowErrorOk(hout, L"Недостаточно памяти для открытия файла", L"Ошибка при открытии файла");
		CloseHandle(f);
		hideStateString();
		if (wasReadonly) {
			DWORD attr = GetFileAttributesW(path.c_str());
			attr |= FILE_ATTRIBUTE_READONLY;
			SetFileAttributesW(path.c_str(), attr);
		}
		return;
	}
	// ---------------------------

	// чтение файла
	showStateString(L"Reading file");
	DWORD readed;
	if (!ReadFile(f, buffer, min (29 * 16, sizeSum), &readed, NULL)) {
		DWORD e = GetLastError();
		showDialogWindowErrorOk(hout, errorCodeToString(e), L"Ошибка при чтении файла");
		CloseHandle(f);
		free(buffer);
		hideStateString();
		return;
	}
	hideStateString();
	// ------------

	// отрисовка окна
	SMALL_RECT brdr;
	brdr.Left = 23;
	brdr.Right = 23 + 82;
	brdr.Top = 2;
	brdr.Bottom = 2 + 36;

	CHAR_INFO *old = new CHAR_INFO[82 * 36];
	ReadConsoleOutputW(hout, old, { 82, 36 }, { 0,0 }, &brdr);

	CHAR_INFO *brd = new CHAR_INFO[82 * 36];
	wstring filename;
	size_t filenameSlashPos;
	if ((filenameSlashPos = path.find_last_of('\\')) != wstring::npos) {
		filename = path.substr(filenameSlashPos + 1);
	}
	else {
		filename = path;
	}
	drawWindow(brd, 82, 36, TextWhite | BgGreen, (readOnly ? L"HEX Editor [Read only] - [" : L"HEX Editor - [") + filename + L"]");
	drawText(brd, 82 * 33 + 2, 78, L"\u25cf HOME to first pos \u25cf END to last pos");
	drawText(brd, 82 * 34 + 2, 78, L"\u25cf ESC exit \u25cf ARROWS change position \u25cf TAB edit mode \u25cf ENTER goto address");
	drawEditorTable(brd);
	drawEditorBody(brd, buffer, sizeSum);

	WriteConsoleOutputW(hout, brd, { 82, 36 }, { 0,0 }, &brdr);
	// --------------

	int posx = 0, oldx = posx;// , posy = 0, oldy = posy, globalpos = 0;
	int cursor = 0; // позиция курсора в текущем редакторе
	bool force = false;
	int mode = 0;
	unsigned long long posy = 0, oldy = posy, globalpos = 0, page = 0, oldpage = page;

	showCursor(hout, 100);
	while (true) {
		setCursorPosition(hout, 37 + (mode == 0 ? (posx > 7 ? 1 : 0) + posx * 3 + cursor : 50 + posx), 5 + posy % 29);
		int b = _getch();
		if (b == 0) {
			b = _getch();
		}
		else if (b == 0xE0) {
			b = _getch();
#pragma region
			if (b == 72) {
				if (posy > 0) {
					posy--;
					globalpos -= 16;
				}
			}
			else if (b == 80) {
				if (globalpos + 16 < sizeSum) {
					posy++;
					globalpos += 16;
				}
			}
			else if (b == 75) {
				if (mode == 0) {
					if (!(posx == 0 && cursor == 0 && posy == 0)) {
						cursor = abs((cursor - 1) % 2);
						if (cursor == 1) {
							posx--;
							globalpos--;
							if (posx < 0) {
								posy--;
								posx = 15;
							}
						}
					}
				}
				else {
					posx--;
					globalpos--;
					if (posx < 0) {
						posy--;
						posx = 15;
					}
				}
			}
			else if (b == 77) {
JUMP:
				if (mode == 0) {
					if (!(globalpos == sizeSum - 1 && cursor == 1)) {
						cursor = (cursor + 1) % 2;
						if (cursor == 0) {
							if (globalpos != sizeSum - 1) {
								posx++;
								globalpos++;
								posy += posx / 16;
								posx %= 16;
							}
						}
					}
				}
				else {
					if (globalpos != sizeSum - 1) {
						posx++;
						globalpos++;
						posy += posx / 16;
						posx %= 16;
					}
				}
			}
#pragma endregion Логика перемещения стрелочек
			else if (b == 71) {
				globalpos = 0;
			}
			else if (b == 79) {
				globalpos = sizeSum - 1;
			}
		}
		else if (b == 27) {
			break;
		}
		else if (b == 13) {
			auto input = showDialogWindowInputOkCancel(hout, L"Код адреса символа:", L"Ввод", validateInputAddress);
			if (!input.canceled) {
				if (input.data == L"") input.data = L"0";
				int gps = stringToInt(input.data);
				globalpos = gps < sizeSum - 1 ? gps : sizeSum - 1;
				cursor = 0;
			}
			setCursorPosition(hout, 37 + (mode == 0 ? (posx > 7 ? 1 : 0) + posx * 3 + cursor : 50 + posx), 5 + posy % 29);
			showCursor(hout, 100);
		}
		else if (b == 9) {
			mode = (mode + 1) % 2;
		}
		else {
#pragma region
			if (!readOnly)
			if (mode == 0) {
				if (b >= '0' && b <= '9') {
					if (cursor == 0) {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0x0F) + 16 * (b - '0');
					}
					else {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0xF0) + b - '0';
					}
					force = true;
					WriteByte(f, globalpos, buffer[globalpos % (29 * 16)]);
					// прыг на следующий символ
					goto JUMP;
				}
				else if (b >= 'A' &&  b <= 'F') {
					if (cursor == 0) {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0x0F) + 16 * (b + 10 - 'A');
					}
					else {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0xF0) + b + 10 - 'A';
					}
					force = true;
					WriteByte(f, globalpos, buffer[globalpos % (29 * 16)]);
					// прыг на следующий символ
					goto JUMP;
				}
				else if (b >= 'a' &&  b <= 'f') {
					if (cursor == 0) {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0x0F) + 16 * (b + 10 - 'a');
					}
					else {
						buffer[globalpos % (29 * 16)] = (buffer[globalpos % (29 * 16)] & 0xF0) + b + 10 - 'a';
					}
					force = true;
					WriteByte(f, globalpos, buffer[globalpos % (29 * 16)]);
					// прыг на следующий символ
					goto JUMP;
				}
			}
			else {
				if (b >= 32 && b <= 126) {
					buffer[globalpos % (29 * 16)] = b;
					force = true;
					WriteByte(f, globalpos, buffer[globalpos % (29 * 16)]);
					// прыг на следующий символ
					goto JUMP;
				}
			}
#pragma endregion Логика смены символа
		}
		posx = globalpos % 16;
		posy = globalpos / 16;
		page = posy / 29;

		if (oldpage != page) {
			LARGE_INTEGER li;
			li.QuadPart = page * 29 * 16;
			SetFilePointer(f, li.LowPart, &li.HighPart, FILE_BEGIN);
			if (!ReadFile(f, buffer, min(29 * 16, sizeSum - 1), &readed, NULL)) {
				DWORD e = GetLastError();
				showDialogWindowErrorOk(hout, errorCodeToString(e), L"Ошибка при чтении файла");
				goto EXIT;
			}
		}

		if (oldpage != page || force) {
#pragma region
			force = false;
			drawEditorTable(brd, page);
			drawEditorBody(brd, buffer, readed, posx, posy);
			SMALL_RECT dr;
			dr.Top = 4;
			dr.Left = 25;
			dr.Right = 102;
			dr.Bottom = 33;
			WriteConsoleOutputW(hout, brd, { 82, 36 }, { 2,2 }, &dr);
#pragma endregion Перерисовка при смене страницы
		}
		else if (posx != oldx || posy != oldy) {
#pragma region
			drawEditorSelect(brd, posx, posy % 29, oldpage, oldx, oldy % 29);
			// now
			SMALL_RECT selr;
			selr.Left = 23 + 14 + (posx > 7 ? 1 : 0) + posx * 3;
			selr.Right = selr.Left + 2;
			selr.Top = 2 + 3 + posy % 29;
			selr.Bottom = selr.Top + 2;
			WriteConsoleOutputW(hout, brd, { 82, 36 }, { selr.Left - 23, selr.Top - 2 }, &selr);
			// r block now
			selr.Left = 23 + 64 + posx;
			selr.Right = selr.Left + 1;
			selr.Top = 2 + 3 + posy % 29;
			selr.Bottom = selr.Top + 2;
			WriteConsoleOutputW(hout, brd, { 82, 36 }, { selr.Left - 23, selr.Top - 2 }, &selr);
			// old
			selr.Left = 23 + 14 + (oldx > 7 ? 1 : 0) + oldx * 3;
			selr.Right = selr.Left + 2;
			selr.Top = 2 + 3 + oldy % 29;
			selr.Bottom = selr.Top + 2;
			WriteConsoleOutputW(hout, brd, { 82, 36 }, { selr.Left - 23, selr.Top - 2 }, &selr);
			// r block old
			selr.Left = 23 + 64 + oldx;
			selr.Right = selr.Left + 1;
			selr.Top = 2 + 3 + oldy % 29;
			selr.Bottom = selr.Top + 2;
			WriteConsoleOutputW(hout, brd, { 82, 36 }, { selr.Left - 23, selr.Top - 2 }, &selr);
#pragma endregion Перерисовка при смене позиции
		}
		oldpage = page;
		oldx = posx;
		oldy = posy;
	}
	delete brd;
	hideCursor(hout);
EXIT:
	CloseHandle(f);
	free(buffer);
	hideStateString();
	if (wasReadonly) {
		DWORD attr = GetFileAttributesW(path.c_str());
		attr |= FILE_ATTRIBUTE_READONLY;
		SetFileAttributesW(path.c_str(), attr);
	}
	// -------
	WriteConsoleOutputW(hout, old, { 82, 36 }, { 0,0 }, &brdr);
	delete old;
}

// проверена
void showStateString(wstring state)
{
	static bool initialized = false;
	static CHAR_INFO buf[128];
	static SMALL_RECT sr;
	if (!initialized) {
		fillColor(buf, 0, 128, BgGreen | TextWhite);
		sr.Top = 39;
		sr.Bottom = 40;
		sr.Left = 0;
		sr.Right = 128;
	}
	drawText(buf, 0, 128, state);
	WriteConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE), buf, { 128, 1 }, { 0,0 }, &sr);
}

// проверена
void hideStateString()
{
	static bool initialized = false;
	static CHAR_INFO buf[128];
	static SMALL_RECT sr;
	if (!initialized) {
		fillColor(buf, 0, 128, BgGreen | TextWhite);
		sr.Top = 39;
		sr.Bottom = 40;
		sr.Left = 0;
		sr.Right = 128;
	}
	drawText(buf, 0, 128, HELP_STRING);
	WriteConsoleOutputW(GetStdHandle(STD_OUTPUT_HANDLE), buf, { 128, 1 }, { 0,0 }, &sr);
}
