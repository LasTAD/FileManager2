#pragma once
#include <iostream>

using namespace std;

class FileCopy {
	
public:
	void StartCopy(wstring &, int);
private:
	void _CopyFile(wstring &, wstring &);
	void CreateDir(wstring &);
	void _Copy(wstring , wstring);
};

class FileDel {
public:
	void StartDel(wstring &, int);
	void ChName(wstring &, int);
private:
	void DelFile(wstring &);
	void DelDir(wstring &);
	void _Del(wstring);
};
