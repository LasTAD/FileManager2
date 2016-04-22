#pragma once
#include "fileman.h"
using namespace std;

class FileCopy {
	
public:
	
	void StartCopy(wstring &, bool);
private:
	void _CopyFile(wstring &, wstring &);
	int CreateDir(wstring &);
};

class FileDel {
public:
	void DelFile(wstring &);
};
