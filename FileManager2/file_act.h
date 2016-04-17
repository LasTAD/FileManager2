#pragma once
#include "fileman.h"
#include "other.h"

class FileCopy {
public:
	void StartCopy();
private:
	//std::wstring src, res;
	void Copy(wstring, wstring);
};
