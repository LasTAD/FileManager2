#pragma once
#include "fileman.h"
using namespace std;

class FileCopy : public FileExplorer {
	
public:
	
	void StartCopy(wstring &);
private:
	void _CopyFile(wstring &, wstring &);
	void CreateDir(wstring &);
};

class FileDel {
public:
	void DelFile(wstring &);
};
