#pragma once
#include "other.h"
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <Windows.h> 
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

class FileExplorer {
	wchar_t			drive = L'C';
public:
	vector<wstring> path;
	UINT32			currentPos = 0;
	UINT32			first, last;
	UINT32			filesPerPage = 20;
	void			parsePath();
public:
	vector<File>	fileList;
	void			getPath(wstring &);
	void			down();
	void			up();
	void			enter();
	void			SetColor(int, int);
protected:
	inline void		updateView();
};

#define CONSOLE_SIZE_X 128
#define CONSOLE_SIZE_Y 40

class Console {
	HANDLE			hstdout, hstdin;
	FileExplorer	fileExplorer;
public:
	Console();
	void			hideCursor();
	void			showCursor();
	void			work();
};