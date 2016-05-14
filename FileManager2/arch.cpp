#include "arch.h"
#include "files.h"
#include <fstream>
#include <map>
#include <vector>

bool decryptRLE(wstring filename, wstring newFilename)
{
	HANDLE fa = CreateFileW(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fa == INVALID_HANDLE_VALUE) {
		setLastErrorCode(GetLastError());
		CloseHandle(fa);
		return false;
	}
	HANDLE f = CreateFileW(newFilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE) {
		setLastErrorCode(GetLastError());
		CloseHandle(f);
		return false;
	}
	DWORD read;
	long num;
	byte b;
	while (true) {
		ReadFile(fa, &num, 4, &read, NULL);
		ReadFile(fa, &b, 1, &read, NULL);
		for (int i = 0; i < num; i++) {
			WriteFile(f, &b, 1, &read, NULL);
		}

	}
	return false;
}

bool encryptRLE(wstring filename, wstring newfilename)
{
	// open file
	HANDLE f = CreateFile(filename.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f == INVALID_HANDLE_VALUE) {
		setLastErrorCode(GetLastError());
		CloseHandle(f);
		return false;
	}

	// create new file
	HANDLE w = CreateFile(newfilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (w == INVALID_HANDLE_VALUE) {
		setLastErrorCode(GetLastError());
		CloseHandle(w);
		return false;
	}

	// read file
	byte b = 1, oldb;
	uint32 count = 0;
	DWORD read;
	bool firstread = true;
	DWORD writen;

	while (true) {
		oldb = b;
		ReadFile(f, &b, 1, &read, NULL);
		// ?????? ??????????
		if (firstread) {
			firstread = false;
			if (read == 0) break;
		}
		else if (b != oldb || read == 0) {
			WriteFile(w, &count, 4, &writen, NULL);
			WriteFile(w, &oldb, 1, &writen, NULL);
			if (read == 0) break;
			if (b != oldb) count = 0;
		}
		++count;
	}

	CloseHandle(f);
	CloseHandle(w);
	return true;
}
