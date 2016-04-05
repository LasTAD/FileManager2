#pragma once

#include <iostream>
#include <Windows.h>

struct File {
	std::wstring	name;
	std::wstring	fullname;
	ULONG64			size;
	bool			isDir;
};