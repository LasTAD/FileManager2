#pragma once

#include <Windows.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <vector>


using namespace std;

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