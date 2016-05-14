#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <vector>
using namespace std;

//перечисление типов
enum fType {
	dotdot = 0,
	fold = 1,
	drive = 2
};

//класс консоли
class Console {
	HANDLE hin, hout;
	// --------------
	vector<wstring> path;
	// --------------
	int fpp = 35;
	int	pos = 0, first = 0, last = 0;
	int page = 0;
	vector<PWIN32_FIND_DATAW> files;
	// --------------
	void updatePages();
	void updateFiles();
	void drawFiles(bool force = false);
	wstring getPath();
public:
	Console();
	void work();
};