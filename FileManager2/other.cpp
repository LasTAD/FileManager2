#include "other.h"

void ErrorMessage(wchar_t *text)
{
	MessageBox(NULL, text, L"Error message", MB_OK | MB_ICONWARNING);
}

void ErrorMessage(std::wstring &text)
{
	MessageBox(NULL, text.c_str(), L"Error message", MB_OK | MB_ICONWARNING);
}

int GetFileList(wchar_t *path, std::vector<PWIN32_FIND_DATAW> &files)
{
	std::wstring spath(path);
	return GetFileList(spath, files);
}


int GetFileList(std::wstring path, std::vector<PWIN32_FIND_DATAW> &files)
{
	PWIN32_FIND_DATAW data = new WIN32_FIND_DATAW;
	HANDLE handle = FindFirstFileW((path + L"*.*").c_str(), data);
	if (handle == INVALID_HANDLE_VALUE) {
		std::wstringstream wss;
		wss << L"GetFileList: invalid handle, error code = " << GetLastError();
		ErrorMessage(wss.str());
		return 1;
	}
	do {
		if (!lstrcmpW(data->cFileName, L"..") || !lstrcmpW(data->cFileName, L".")) {
			continue;
		}
		files.push_back(data);
		data = new WIN32_FIND_DATAW;
	} while (FindNextFileW(handle, data));
	FindClose(handle);
	return 0;
}

static wchar_t __spaces20[] = L"                    ";

std::wstring l(char c, int length)
{
	if (c == ' ' && length == 20) return __spaces20;
	std::wstringstream wss;
	for (int i = 0; i < length; i++) {
		wss << c;
	}
	return wss.str();
}

std::wstring l(char* c, int length)
{
	std::wstringstream wss;
	for (int i = 0; i < length; i++) {
		wss << c;
	}
	return wss.str();
}

std::wstring l(std::wstring c, int length)
{
	std::wstringstream wss;
	for (int i = 0; i < length; i++) {
		wss << c;
	}
	return wss.str();
}

char c(int n)
{
	return n;
}

std::wstring crop(std::wstring str, int length)
{
	if (str.length() > length) return str.substr(0, length-3) + L"...";
	if (str.length() < length) return str + l(' ', length - str.length());
	return str;
}

std::wstring cropf(std::wstring str, int length)
{
	if (str.length() > length) return str.substr(0, length - 3) + L"...";
	if (str.length() < length) return l(' ', length - str.length()) + str;
	return str;
}

std::wstring cropt(std::wstring str, int length)
{
	if (str.length() > length) return L"..." + str.substr(0, length - 3);
	if (str.length() < length) return str + l(' ', length - str.length());
	return str;
}

std::wstring crop(ULONG64 value, int length)
{
	std::wstringstream wss;
	wss << value << "  B";
	return crop(wss.str(), length);
}

std::wstring cropf(ULONG64 value, int length)
{
	std::wstringstream wss;
	wss << value << "  B";
	return cropf(wss.str(), length);
}

CHAR_INFO newchar(char v, int cc)
{
	CHAR_INFO ci;
	ci.Char.AsciiChar = v;
	ci.Char.UnicodeChar = 0;
	ci.Attributes = cc;
	return ci;
}

CHAR_INFO newchar(wchar_t v, int cc)
{
	CHAR_INFO ci;
	ci.Char.AsciiChar = 0;
	ci.Char.UnicodeChar = v;
	ci.Attributes = cc;
	return ci;
}
