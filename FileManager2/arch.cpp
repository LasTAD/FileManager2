#include "arch.h"
#include <fstream>
#include <map>
#include <vector>
using namespace std;

bool Archive::StartArch(wstring & fileName)
{
	// TODO
	//wstring extens;
	//extens = fileName.substr(fileName.length() - 4, fileName.length() - 1);
	/*if (extens == L"cmpr") 
		if(!UnArch(fileName))
			return false;
	else
		if(!Arch(fileName))
			return false;*/
	Arch(fileName);
		return true;
}

bool Archive::Arch(wstring &fileName)
{
	DWORD dwTemp=0;
	int quant = 0;
	int weight[0x100];
	for (auto &i : weight)
		i = 0;
	{
		HANDLE f = CreateFileW(fileName.c_str(), GENERIC_READ,FILE_SHARE_READ, NULL, OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL, NULL);
		if (f == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), errorCodeToString(err), L"������ ��� �������� �����");
			CloseHandle(f);
			return false;
		}
		while (dwTemp)
		{
			++quant;
			char ch;
			if (!ReadFile(f, &ch, sizeof(ch), &dwTemp, NULL)) {
				DWORD err = GetLastError();
				showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), errorCodeToString(err), L"������ ��� ������ �����");
				return false;
			}
			++weight[ch];
		} 
	}
	multimap<int, int> sortedWeight;
	vector<Node> tree;
	map<char, int> charMap;
	for (size_t i = 0; i < 0x100; ++i)
	{
		if (weight[i] > 0)
		{
			tree.push_back(Node{ (char)i,-1,-1,-1,false });
			charMap[i] = tree.size() - 1;
			sortedWeight.insert(make_pair(weight[i], tree
				.size() - 1));
		}
	}
	while (sortedWeight.size() > 1)
	{
		int w0 = begin(sortedWeight)->first;
		int n0 = begin(sortedWeight)->second;
		sortedWeight.erase(begin(sortedWeight));
		int w1 = begin(sortedWeight)->first;
		int n1 = begin(sortedWeight)->second;
		sortedWeight.erase(begin(sortedWeight));
		tree.push_back(Node{ '\0',-1,n0,n1,false });
		tree[n0].parent = tree.size() - 1;
		tree[n0].branch = false;
		tree[n1].parent = tree.size() - 1;
		tree[n1].branch = true;
		sortedWeight.insert(make_pair(w0 + w1, tree.size() - 1));
	}
	vector<bool> data;
	{
		HANDLE f = CreateFileW(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (f == INVALID_HANDLE_VALUE) {
			DWORD err = GetLastError();
			showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), errorCodeToString(err), L"������ ��� �������� �����");
			CloseHandle(f);
			return false;
		}
		while (dwTemp)
		{
			char ch;
			if (!ReadFile(f, &ch, sizeof(char), &dwTemp, NULL)) {
				DWORD err = GetLastError();
				showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), errorCodeToString(err), L"������ ��� ������ �����");
				return false;
			}
			vector<bool> compressedChar;
			auto n = tree[charMap[ch]];
			while (n.parent != -1)
			{
				compressedChar.push_back(n.branch);
				n = tree[n.parent];
			}
			data.insert(end(data), compressedChar.rbegin(), compressedChar.rend());
		}
	}
	HANDLE fa = CreateFileW((fileName + L"cmpr").c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fa == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), errorCodeToString(err), L"������ ��� �������� �����");
		CloseHandle(fa);
		return false;
	}
	--quant;
	if (!WriteFile(fa, (char*)&quant, sizeof(quant), &dwTemp, NULL)) {
		return false;
	}
	//f.write((char*)&quant, sizeof(quant));
	int treeSize = tree.size();
	if (!WriteFile(fa, (char*)&treeSize, sizeof(treeSize), &dwTemp, NULL)) {
		return false;
	}
	//f.write((char*)&treeSize, sizeof(treeSize));
	for (auto i : tree) {
		if (!WriteFile(fa, (char*)&i, sizeof(i), &dwTemp, NULL)) {
			return false;
		}
	}
		//f.write((char*)&i, sizeof(i));
	for (size_t i = 0; i <= data.size() / 8; ++i)
	{
		wchar_t ch = 0;
		for (int j = 0; j < 8; ++j)
			if ((i * 8 + j) >= data.size()) break;
			else
				if (data[i * 8 + j])
					ch |= (1 << j);
		if (!WriteFile(fa, (char*)&ch, sizeof(ch), &dwTemp, NULL)) {
			return false;
		}
		//f.write((char*)&ch, sizeof(ch));
	}
	CloseHandle(fa);
}

//bool Archive::UnArch(wstring &fileName)
//{
//	vector<Node> tree;
//	ifstream f(fileName, ios::binary);
//	unsigned int quant;
//	f.read((char*)&quant, sizeof(quant));
//	int treeSize;
//	f.read((char*)&treeSize, sizeof(treeSize));
//	for (int i = 0; i < treeSize; ++i)
//	{
//		Node n;
//		f.read((char*)&n, sizeof(n));
//		tree.push_back(n);
//	}
//	vector<bool> data;
//	while (!f.eof())
//	{
//		unsigned char ch;
//		f.read((char *)&ch, sizeof(ch));
//		for (int i = 0; i < 8; ++i)
//			data.push_back((ch&(1 << i)) != 0);
//
//	}
//	auto n = tree.size() - 1;
//	
//	wstring unCompFile;
//	unCompFile = fileName.substr(0, fileName.length()-4);
//	
//	ofstream f1(unCompFile);
//	for (auto i : data)
//	{
//		if (i)
//			n = tree[n].one;
//		else
//			n = tree[n].zero;
//		if (tree[n].one == -1)
//		{
//			if (quant--)
//			{
//				f1.write((char*)&tree[n].ch, sizeof(tree[n].ch));
//				n = tree.size() - 1;
//			}
//			else break;
//		}
//	}
//}
