#pragma once
#include <iostream>
#include <Windows.h>

struct File {
	std::wstring	name;
	std::wstring	fullname;
	ULONG64			size;
	bool			isDir;
	bool			modifier = false;
};

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