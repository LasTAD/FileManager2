#pragma once
#include "fileman.h"

using namespace std;

class FileCopy {
	
public:
	void StartCopy(wstring &, bool);
private:
	vector<PWIN32_FIND_DATAW> dirFiles;
	void _CopyFile(wstring &, wstring &);
	void CreateDir(wstring &);
	void _Copy(wstring &, wstring&);
};

//class FileDel {
//public:
//	void Del(wstring &, bool);
//	void ChName(wstring &);
//private:
//	void DelFile(wstring &);
//	void DelDir(wstring &, wstring &);
//};
