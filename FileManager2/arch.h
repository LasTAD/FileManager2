#pragma once
#include <iostream>

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
	void StartArch(wstring &);
private:
	void Arch(wstring &);
	void UnArch(wstring &);

};

bool encryptRLE(wstring filename, wstring newfilename);