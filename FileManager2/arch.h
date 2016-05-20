#pragma once
#include <iostream>
#include <queue>

using namespace std;

bool encryptRLE(wstring filename, wstring newfilename);

bool decryptRLE(wstring filename, wstring newfilename);

bool enHuffman(wstring filename, wstring newfilename);

bool deHuffman(wstring filename, wstring newfilename);

typedef struct TREE {
	int weight;
	char ch;
	TREE *left;
	TREE *right;
}sym;

TREE * makeTree(sym * psym[], int k);