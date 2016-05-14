#pragma once
#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <vector>
using namespace std;

#define CONSOLE_SIZE_X 128
#define CONSOLE_SIZE_Y 40

enum ConsoleColor	//консольные цвета
{
	TextBlack = 0,
	TextBlue = 1,
	TextGreen = 2,
	TextCyan = 3,
	TextRed = 4,
	TextMagenta = 5,
	TextBrown = 6,
	TextLightGray = 7,
	TextDarkGray = 8,
	TextLightBlue = 9,
	TextLightGreen = 10,
	TextLightCyan = 11,
	TextLightRed = 12,
	TextLightMagenta = 13,
	TextYellow = 14,
	TextWhite = 15,
	BgBlack = 0,
	BgBlue = 16,
	BgGreen = 32,
	BgCyan = 48,
	BgRed = 64,
	BgMagenta = 80,
	BgBrown = 96,
	BgLightGray = 112,
	BgDarkGray = 128,
	BgLightBlue = 144,
	BgLightGreen = 160,
	BgLightCyan = 176,
	BgLightRed = 192,
	BgLightMagenta = 208,
	BgYellow = 224,
	BgWhite = 240
};


typedef bool (*validateInput)(wstring str);		// Указатель на функцию проверки ввода

void showCursor(HANDLE hout, DWORD size = 20);	// Показать курсор с заданным размером (от 10 до 100 (d = 10))

void hideCursor(HANDLE hout);					// скрыть курсор

void setCursorPosition(HANDLE hout, short x, short y);											// Установить позицию курсора

void drawWindow(CHAR_INFO* buf, int width, int height, int color, wstring caption = L"");		// Нарисовать рамку для окна

void drawText(CHAR_INFO* buf, int from, int length, wstring text, int textFrom = 0);			// Написать текст с позиции в массиве с ограниченной длиной в одну строку с заданной позиции в строке

void fillColor(CHAR_INFO* buf, int from, int length, int color);								// Установить цвет в заданных позициях

bool showDialogWindowYN(HANDLE hout, wstring text = L"", wstring caption = L"");				// Показать окно выбора (да/нет)

int showDialogWindowYNC(HANDLE hout, wstring text, wstring caption, int def = 0);				// Показать окно выбора (да/нет/отмена)

void showDialogWindowOk(HANDLE hout, int color, int btColor, wstring text = L"", wstring caption = L"");	// Показать окно информации (ок)

void showDialogWindowErrorOk(HANDLE hout, wstring text = L"", wstring caption = L"");						// Показать окно с ошибкой

struct _input {
	_input(bool c, wstring d) {
		canceled = c;
		data = d;
	}
	bool canceled = false;
	wstring data;
} showDialogWindowInputOkCancel(HANDLE hout, wstring text = L"", wstring caption = L"", validateInput f = nullptr);		// Показать окно с полем ввода

void drawWindowFM(HANDLE hout);			// Нарисовать основное окно файлового менеджера (буфер 128x40)

void drawTableFM(HANDLE hout, vector<PWIN32_FIND_DATAW> files, int first, int last, int pos);			// Нарисовать таблицу файлов, позиция абсолютная

void drawSelectRow(HANDLE hout, int pos, int color);				// Выделить строку, позиция относительно первого

void drawPath(HANDLE hout, wstring path, int shift);				// Написать путь со смещением в основной строке, если она длиннее поля

void startEditor(HANDLE hout, wstring path);						// Запускает HEX редактор

void showStateString(wstring state);								// Пишет строку состояния

void hideStateString();												// Скрывает строку состояния
