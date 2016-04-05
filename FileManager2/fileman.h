#pragma once
#include "other.h"
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <Windows.h>

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
private:
	inline void		updateView();
};