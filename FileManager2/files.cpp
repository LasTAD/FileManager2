#include "files.h"
#include "gui.h"
#include <string>

static DWORD globalErrorCode = 0;
static wstring globalErrorFilename = L"";

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
		data->dwReserved1 = MAXDWORD;
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
	FindClose(handle);
	delete data;
	return{ files, dirs };
}

wstring errorCodeToString(DWORD code)
{
	wchar_t buf[600];
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 600, NULL);
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
			bool k = RemoveDirectoryW(path.c_str());
			if (!k) {
				setLastErrorCode(GetLastError());
				setLastErrorFilename(path);
			}
			return k;
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
		bool k = RemoveDirectoryW(path.c_str());
		if (!k) {
			setLastErrorCode(GetLastError());
			setLastErrorFilename(path);
		}
		return k;
	}
	else {
		DWORD at = GetFileAttributesW(path.c_str());
		if (at & FILE_ATTRIBUTE_READONLY) {
			at &= ~FILE_ATTRIBUTE_READONLY;
			if (!SetFileAttributesW(path.c_str(), at)) {
				setLastErrorCode(GetLastError());
				setLastErrorFilename(path);
				return false;
			}
		}
		bool k = DeleteFileW(path.c_str());
		if (!k) {
			setLastErrorCode(GetLastError());
			setLastErrorFilename(path);
		}
		return k;
	}
}

bool _copy(wstring from, wstring to, bool isDir) {
	if (isDir) {
		CreateDirectoryW(to.c_str(), NULL);
		bool k = copyDir(from, to);
		if (!k) {
			setLastErrorCode(GetLastError());
			setLastErrorFilename(to);
		}
		return k;
	}
	else {
		DWORD at = GetFileAttributesW(from.c_str());
		if (at & FILE_ATTRIBUTE_READONLY) {
			at &= ~FILE_ATTRIBUTE_READONLY;
			if (!SetFileAttributesW(from.c_str(), at)) {
				setLastErrorCode(GetLastError());
				setLastErrorFilename(from);
				return false;
			}
		}
		bool k = CopyFileW(from.c_str(), to.c_str(), true);
		if (!k) {
			setLastErrorCode(GetLastError());
			setLastErrorFilename(to);
		}
		return k;
	}
}

bool copyDir(wstring from, wstring to)
{
	WIN32_FIND_DATAW data;
	HANDLE handle = FindFirstFileW(wstring(from + L"\\*.*").c_str(), &data);
	if (handle == INVALID_HANDLE_VALUE) {
		return false;
	}
	do {
		// отсечение модификаторов
		if (lstrcmpW(data.cFileName, L"..") == 0 || lstrcmpW(data.cFileName, L".") == 0) {
			continue;
		}
		// -----------------------
		size_t pos = to.find_last_of(L'\\', to.length());
		wstring res1= to.substr(pos + 1, to.length() - pos - 1);
		if (data.cFileName == res1)
			continue;
		bool dir = data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false;
		if (dir) {
			CreateDirectoryW((to + L"\\" + data.cFileName).c_str(), NULL);
			copyDir((from + L"\\" + data.cFileName).c_str(), (to + L"\\" + data.cFileName).c_str());
		}
		else {
			CopyFileW((from + L"\\" + data.cFileName).c_str(), (to + L"\\" + data.cFileName).c_str(), true);
		}
	} while (FindNextFileW(handle, &data) != NULL);
	FindClose(handle);
}

void setLastErrorFilename(wstring name)
{
	globalErrorFilename = name;
}

wstring getLastErrorFilename()
{
	wstring n = globalErrorFilename;
	globalErrorFilename = L"";
	return n;
}

void setLastErrorCode(DWORD code) {
	globalErrorCode = code;
}

DWORD getLastErrorCode()
{
	DWORD n = globalErrorCode;
	globalErrorCode = 0;
	return n;
}

bool WriteByte(HANDLE f, uint64 pos, byte b)
{
	LARGE_INTEGER li;
	li.QuadPart = pos;
	SetFilePointer(f, li.LowPart, &li.HighPart, FILE_BEGIN);
	DWORD written;
	return WriteFile(f, &b, 1, &written, NULL) && written == 1;
}
