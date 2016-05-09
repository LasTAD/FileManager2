#include "files.h"
#include "gui.h"

bool getFiles(wstring path, vector<PWIN32_FIND_DATAW> &files)
{
	PWIN32_FIND_DATAW data = new WIN32_FIND_DATAW; // ������� ������ ��� ������
	// ����� ������� ����� � ����� �� ������� � ������ ������
	HANDLE handle = FindFirstFileW((path + L"*.*").c_str(), data);
	if (handle == INVALID_HANDLE_VALUE) {
		delete data;
		showDialogWindowErrorOk(GetStdHandle(STD_OUTPUT_HANDLE), L"�� ������� ��������� ����� ������ � ����������", L"������ ��������");
		return false;
	}
	// -------------------
	for (auto data : files) delete data; // ������� ������ (�� ������)
	files.clear();
	do {
		// ��������� �������������
		if (lstrcmpW(data->cFileName, L"..") == 0 || lstrcmpW(data->cFileName, L".") == 0) {
			continue;
		}
		// -----------------------
		files.push_back(data);
		data = new WIN32_FIND_DATAW; // ������� ������ ��� ������
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
		// ��������� �������������
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
	} // ��� ��� ����� �� �������, ��� �������� � ������
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
			// ��������� �������������
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
/*
bool copyDir(wstring src, wstring res)
{
	vector<PWIN32_FIND_DATAW> dirFiles;

	if (!getFiles(src + L'\\', dirFiles)) {
		return false;
	}

	for (int i = 0; i < dirFiles.size(); i++) {
		if (dirFiles[i]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			CreateDir(res + L'\\' + dirFiles[i]->cFileName);
			CreateDirectoryW()
			_Copy(src + L'\\' + dirFiles[i]->cFileName, res + L'\\' + dirFiles[i]->cFileName);
		}
		else
			_CopyFile(src + L'\\' + dirFiles[i]->cFileName, res + L'\\' + dirFiles[i]->cFileName);
		delete dirFiles[i];
	}
}
*/

bool copyFile(wstring from, wstring to, bool isDir)
{
	// TEST
	HANDLE f1 = CreateFileW(from.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f1 == INVALID_HANDLE_VALUE) {
		CloseHandle(f1);
		return false;
	}
	/*
	HANDLE f2 = CreateFileW(to.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (f2 == INVALID_HANDLE_VALUE) {
		DWORD v = GetLastError();
		CloseHandle(f1);
		CloseHandle(f2);
		return false;
	}*/

	DWORD v = GetLastError();
	char buf[4096];
	unsigned long long read = 0;
	DWORD length = 0;
	do {
		ReadFile(f1, buf, 4096, &length, NULL);
		read += length;
	} while (length != 0);
	v = GetLastError();
	int t = 0;
	// =====
	/*
	errno_t err;
	FILE *fileSrc, *fileRes;
	err = _wfopen_s(&fileSrc, from.c_str(), L"rb");
	if (err) {
		// TODO
		//ErrorMessage(L"Unable to open requested file!");
		return;
	}
	err = _wfopen_s(&fileRes, to.c_str(), L"wb");
	if (err) {
		// TODO
		// ErrorMessage(L"Unable to create new file!");
		return;
	}
	unsigned int fsize, n;
	fseek(fileSrc, 0, SEEK_END);
	fsize = ftell(fileSrc);
	rewind(fileSrc);
	char *buffer;
	n = fsize / 65536;
	for (int i = 0; i < n; ++i)
	{
		buffer = new char[65536];
		fread(buffer, 65536, sizeof(char), fileSrc);
		fwrite(buffer, 65536, sizeof(char), fileRes);
		delete[] buffer;
	}
	int rem = fsize - n * 65536;
	buffer = new char[rem];
	fread(buffer, rem, sizeof(char), fileSrc);
	fwrite(buffer, rem, sizeof(char), fileRes);
	delete[] buffer;
	_fcloseall();
	return false;
	*/
	return false;
}

static wstring globalErrorFilename = L"";
static DWORD globalErrorCode = 0;

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