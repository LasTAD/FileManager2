#pragma once
#include "fileman.h"
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
	void StartArch(wstring &, int);
private:
	void Arch(wstring &);
	void UnArch(wstring &);

};
