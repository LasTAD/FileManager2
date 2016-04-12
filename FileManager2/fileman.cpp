#include "fileman.h"

void FileExplorer::parsePath()
{
	wstring path, findpath;
	getPath(path);
	findpath = path + L"*.*";

	fileList.clear();
	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFileW(findpath.c_str(), &data);
	do {
		File file;
		file.name = data.cFileName;
		file.fullname = path + data.cFileName;
		file.size = (data.nFileSizeHigh * MAXDWORD) + data.nFileSizeLow;
		file.isDir = data.dwFileAttributes && FILE_ATTRIBUTE_DIRECTORY != 0;
		fileList.push_back(file);
		//MessageBoxW(NULL, file.fullname.c_str(), L"", 0);
	} while (FindNextFileW(h, &data));
	FindClose(h);

	first = 0;
	last = min(fileList.size() - 1, filesPerPage - 1);

	updateView();
}

void FileExplorer::down()
{
	if (currentPos < fileList.size() - 1) {
		currentPos++;
	}
	else if (fileList.size() - 1 == currentPos) {
		currentPos = 0;
	}
	updateView();
}

void FileExplorer::up()
{
	if (currentPos > 0) {
		currentPos--;
	}
	else if(0 == currentPos) {
		currentPos = fileList.size() - 1;
	}
	updateView();
}

void FileExplorer::getPath(wstring &pathString)
{
	wstringstream wss;
	wss << drive << L":\\";
	for (auto i : path) {
		wss << i << L'\\';
	}
	pathString = wss.str();
}

inline void FileExplorer::updateView()
{
	if (currentPos > last) {
		last = currentPos;
		first = last - filesPerPage + 1;
	}
	else if (currentPos < first) {
		first = currentPos;
		last = first + filesPerPage - 1;
	}
}