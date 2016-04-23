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
	HANDLE handle = FindFirstFileW((path += L"*.*").c_str(), data);
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