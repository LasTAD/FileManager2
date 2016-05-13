#pragma once
#include <iostream>
#include "app.h"
#include "files.h"
#include "gui.h"
using namespace std;

struct Node {
	char ch;
	int parent;
	int zero;
	int one;
	bool branch;
};
class Archive {
public:
	bool StartArch(wstring &);
private:
	bool Arch(wstring &);
	//bool UnArch(wstring &);

};
