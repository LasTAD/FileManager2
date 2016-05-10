#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>

using namespace std;

struct celements {
	int all = 0;
	int files = 0;
	int dirs = 0;
	celements(int files, int dirs) {
		if (files > -1 && dirs > -1) {
			this->files = files;
			this->dirs = dirs;
			all = files + dirs;
		}
		else {
			all = files = dirs = -1;
		}
	}
};

// ������ ��������� � ������ ������ � ������ false
bool getFiles(wstring path, vector<PWIN32_FIND_DATAW> &files);

// ������ -1 � ������ ������
celements countFiles(wstring path);

// ����������� ������ � ������
wstring errorCodeToString(DWORD code);

// ������� ����/���������� � ��� ������
bool removeFile(wstring path, bool isDir);

// �������� ��� ����������
bool copyDir(wstring from, wstring to, bool isDir);

bool _copy(wstring from, wstring to, bool isDir);

void setLastErrorFilename(wstring name);

wstring getLastErrorFilename();

void setLastErrorCode(DWORD code);

DWORD getLastErrorCode();