#pragma once

#include <Windows.h>
#include <iostream>
#include <vector>
#include "types.h"
#include <string>

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

// ’Ё… пачаны, кто «ј√ЋяЌ”Ћ ¬Ќ”“–№, как вам мой [быдло]код? 
class TempFile {
	bool open = false;
	HANDLE h;
	uint64 pos = 0;
	uint64 cnt = 0;
	wstring name;
public:
	TempFile() {
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		name = to_wstring(ft.dwLowDateTime + ft.dwHighDateTime * 0xFFFFFFFFFFFFFFFF) + L'_' + to_wstring(rand());
		h = CreateFileW(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (h == INVALID_HANDLE_VALUE) {
			CloseHandle(h);
			return;
		}
		open = true;
	}
	~TempFile() {
		if (open) {
			CloseHandle(h);
		}
		DeleteFileW(name.c_str());
	}
	bool put(byte val, uint64 bytepos) {
		bool suc = false;
		DWORD written;
		suc = WriteFile(h, &bytepos, 8, &written, NULL);
		suc = WriteFile(h, &val, 1, &written, NULL);
		pos += 9;
		++cnt;
		return suc;
	}
	bool get(byte& val, uint64& bytepos) {
		bool suc = false;
		SetFilePointer(h, -9, 0, FILE_CURRENT);
		DWORD readed;
		suc = ReadFile(h, &bytepos, 8, &readed, NULL);
		suc = ReadFile(h, &val, 1, &readed, NULL);
		SetFilePointer(h, -9, 0, FILE_CURRENT);
		pos -= 9;
		--cnt;
		return suc;
	}
	bool getNotDelete(byte& val, uint64& bytepos, int newpos) {
		if (newpos > cnt - 1) return false;
		bool suc = false;
		SetFilePointer(h, newpos*9, 0, FILE_BEGIN);
		DWORD readed;
		suc = ReadFile(h, &bytepos, 8, &readed, NULL);
		suc = ReadFile(h, &val, 1, &readed, NULL);
		SetFilePointer(h, pos, 0, FILE_BEGIN);
		return suc;
	}
	bool isEmpty() {
		return pos == 0;
	}
	bool isOpen() {
		return open;
	}
	uint64 count() {
		return cnt;
	}
};

// выдаст сообщение в случае ошибки и вернет false
bool getFiles(wstring path, vector<PWIN32_FIND_DATAW> &files);

// вернет -1 в случае ошибки
celements countFiles(wstring path);

// преобразует ошибку в строку
wstring errorCodeToString(DWORD code);

// удал€ет файл/директорию и все внутри
bool removeFile(wstring path, bool isDir);

// копирует все содержимое
bool copyDir(wstring from, wstring to);

bool _copy(wstring from, wstring to, bool isDir);

void setLastErrorFilename(wstring name);

wstring getLastErrorFilename();

void setLastErrorCode(DWORD code);

DWORD getLastErrorCode();

bool WriteByte(HANDLE f, uint64 pos, byte b);