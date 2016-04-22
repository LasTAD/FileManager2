#pragma once
#include "fileman.h"
using namespace std;

class FileCopy {
	
public:
	FileExplorer fileEx;
	Console consoleCopy;
	void StartCopy(wstring &, bool);
private:
	void _CopyFile(wstring &, wstring &);
	void CreateDir(wstring &);
	void _Copy(wstring &, wstring&);
};

class FileDel {
public:
	void DelFile(wstring &);
	void ChName(wstring &);
};
