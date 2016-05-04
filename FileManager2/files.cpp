#include "files.h"
#include "gui.h"

bool getFiles(wstring path, vector<PWIN32_FIND_DATAW> &files)
{
	PWIN32_FIND_DATAW data = new WIN32_FIND_DATAW; // выделим память для данных
	// поиск первого файла и выход из функции в случае ошибки
	HANDLE handle = FindFirstFileW((path + L"*.*").c_str(), data);
	if (handle == INVALID_HANDLE_VALUE) {
		delete data;
		showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), L"Не удалось выполнить поиск файлов в директории", L"Доступ запрещен");
		return false;
	}
	// -------------------
	for (auto data : files) delete data; // очистка памяти (на всякий)
	files.clear();
	do {
		// отсечение модификаторов
		if (lstrcmpW(data->cFileName, L"..") == 0 || lstrcmpW(data->cFileName, L".") == 0) {
			continue;
		}
		// -----------------------
		files.push_back(data);
		data = new WIN32_FIND_DATAW; // выделим память для данных
	} while (FindNextFileW(handle, data));
	FindClose(handle);
	delete data;
	return true;
}

celements countFiles(wstring path)
{
	int files = 0, dirs = 0;
	PWIN32_FIND_DATAW data = new WIN32_FIND_DATAW;
	HANDLE handle = FindFirstFileW((path + L"*.*").c_str(), data);
	if (handle == INVALID_HANDLE_VALUE) {
		return{ -1, -1 };
	}
	do {
		// отсечение модификаторов
		if (lstrcmpW(data->cFileName, L"..") == 0 || lstrcmpW(data->cFileName, L".") == 0) {
			continue;
		}
		// -----------------------
		if (data->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ++dirs;
		else ++files;
	} while (FindNextFileW(handle, data));
	DWORD d = GetLastError();
	FindClose(handle);
	delete data;
	return{ files, dirs };
}

wstring errorCodeToString(DWORD code)
{
	wchar_t buf[62];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 62, NULL);
	wchar_t *p = wcsstr(buf, L"\r");
	if (p) {
		*p = 0;
	} // вот это лучше не трогать, все работает и пускай
	return wstring(buf);
}

bool removeFile(wstring path, bool isDir)
{
	if (isDir) {
		WIN32_FIND_DATAW data;
		HANDLE handle = FindFirstFileW(wstring(path + L"\\*.*").c_str(), &data);
		if (handle == INVALID_HANDLE_VALUE) {
			return RemoveDirectoryW(path.c_str());
		}
		do {
			// отсечение модификаторов
			if (lstrcmpW(data.cFileName, L"..") == 0 || lstrcmpW(data.cFileName, L".") == 0) {
				continue;
			}
			// -----------------------
			bool dir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false;
			if (!removeFile(wstring(path + L"\\" + data.cFileName).c_str(), dir)) {
				FindClose(handle);
				return false;
			}
		} while (FindNextFileW(handle, &data));
		FindClose(handle);
		return RemoveDirectoryW(path.c_str());
	}
	else {
		return DeleteFileW(path.c_str());
	}
}
