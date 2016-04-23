#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <sstream>

struct File {
	std::wstring	name;
	std::wstring	fullname;
	ULONG64			size;
	bool			isDir;
	bool			modifier = false;
};

typedef File* PFile;

enum ConsoleColor {
	Black = 0,
	Blue = 1,
	Green = 2,
	Cyan = 3,
	Red = 4,
	Magenta = 5,
	Brown = 6,
	LightGray = 7,
	DarkGray = 8,
	LightBlue = 9,
	LightGreen = 10,
	LightCyan = 11,
	LightRed = 12,
	LightMagenta = 13,
	Yellow = 14,
	White = 15
};

// Функция выводит сообщение об ошибке
void ErrorMessage(wchar_t*);
void ErrorMessage(std::wstring&);

// Функция для поиска файлов, возвращает 0 в случае успеха
int GetFileList(wchar_t*, std::vector<PWIN32_FIND_DATAW>&);
int GetFileList(std::wstring, std::vector<PWIN32_FIND_DATAW>&);