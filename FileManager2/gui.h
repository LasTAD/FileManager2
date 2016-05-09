#pragma once

#include <Windows.h>
#include <iostream>
#include <conio.h>
#include <vector>

using namespace std;

#define CONSOLE_SIZE_X 128
#define CONSOLE_SIZE_Y 40

enum ConsoleColor
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

// Указатель на функцию проверки ввода
typedef bool (*validateInput)(wstring str);

// Показать курсор с заданным размером (от 10 до 100 (d = 10)), либо скрыть
void showCursor(HANDLE hout, DWORD size = 20);
void hideCursor(HANDLE hout);
// Установить позицию курсора
void setCursorPosition(HANDLE hout, short x, short y);

// Нарисовать рамку для окна
void drawWindow(CHAR_INFO* buf, int width, int height, int color, wstring caption = L"");

// Написать текст с позиции в массиве с ограниченной длиной в одну строку с заданной позиции в строке
void drawText(CHAR_INFO* buf, int from, int length, wstring text, int textFrom = 0);

// Установить цвет в заданных позициях
void fillColor(CHAR_INFO* buf, int from, int length, int color);

// Показать окно выбора (да/нет)
bool showDialogWindowYN(HANDLE hout, wstring text = L"", wstring caption = L"");

// Показать окно выбора (да/нет/отмена)
int showDialogWindowYNC(HANDLE hout, wstring text, wstring caption, int def = 0);

// Показать окно информации (ок)
void showDialogWindowOk(HANDLE hout, int color, int btColor, wstring text = L"", wstring caption = L"");

// Показать окно с ошибкой
void showDialogWindowErrorOk(HANDLE hout, wstring text = L"", wstring caption = L"");

// Показать окно с полем ввода
struct _input {
	_input(bool c, wstring d) {
		canceled = c;
		data = d;
	}
	bool canceled = false;
	wstring data;
} showDialogWindowInputOkCancel(HANDLE hout, wstring text = L"", wstring caption = L"", validateInput f = nullptr);

// Нарисовать основное окно файлового менеджера (буфер 128x40)
void drawWindowFM(HANDLE hout);

// Нарисовать таблицу файлов, pos АБСОЛЮТНЫЙ !!!
void drawTableFM(HANDLE hout, vector<PWIN32_FIND_DATAW> files, int first, int last, int pos);

// Выделить строку, pos ОТНОСИТЕЛЬНО first !!!
void drawSelectRow(HANDLE hout, int pos, int color);

// Написать путь со смещением в основной строке, если она длиннее поля
void drawPath(HANDLE hout, wstring path, int shift);

// Запускает HEX редактор, функцию по идее бы надо переделать в класс, но и так пусть работает
void startEditor(HANDLE hout, wstring path);

// Пишет строку состояния
void showStateString(wstring state);

// Скрывает строку состояния
void hideStateString();