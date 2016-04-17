#pragma once
#include "fileman.h"
using namespace std;

class FileCopy : public FileExplorer {
	
public:
	void StartCopy(wstring &);
private:
	void Copy(wstring &, wstring &);
};
