#include "fileman.h"
#include "file_act.h"

void FileExplorer::parsePath()
{
	wstring path, findpath;
	getPath(path);
	findpath = path + L"*.*";

	fileList.clear();

	File dotdot;
	dotdot.fullname = L"..";
	dotdot.isDir = true;
	dotdot.name = L"..";
	dotdot.modifier = true;
	fileList.push_back(dotdot);

	WIN32_FIND_DATA data;
	HANDLE h = FindFirstFileW(findpath.c_str(), &data);

	// stopgap
	if (h == 0)
		MessageBox(NULL, L"Ничего не найдено", L"Ошибка", 0);

	do {
		if (!lstrcmpW(data.cFileName, L"..") || !lstrcmpW(data.cFileName, L".")) {
			continue;
		}

		File file;
		file.name = data.cFileName;
		file.fullname = path + data.cFileName;
		file.size = (data.nFileSizeHigh * MAXDWORD) + data.nFileSizeLow;
		file.isDir = (bool)(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
		fileList.push_back(file);
	} while (FindNextFileW(h, &data));
	FindClose(h);

	currentPos = 0;
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
	else if (0 == currentPos) {
		currentPos = fileList.size() - 1;
	}
	updateView();
}

void FileExplorer::enter()
{
	if (fileList[currentPos].modifier) {

		// TODO: make disks list window

		if (path.size() > 0) // stopgap
			path.pop_back();

		parsePath();
	}
	else if (fileList[currentPos].isDir) {
		path.push_back(fileList[currentPos].name);
		parsePath();
	}
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

void Console::hideCursor()
{
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 20;
	cci.bVisible = false;
	SetConsoleCursorInfo(hstdout, &cci);
}

void Console::showCursor()
{
	CONSOLE_CURSOR_INFO cci;
	cci.dwSize = 20;
	cci.bVisible = true;
	SetConsoleCursorInfo(hstdout, &cci);
}

void Console::work()
{
	FileCopy filecopy;
	hstdin = GetStdHandle(STD_INPUT_HANDLE);
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hideCursor();
	fileExplorer.parsePath();

	INPUT_RECORD pin;
	DWORD event_count;
	wstring p;

	fileExplorer.getPath(p);
	wcout << "Current dir: " << p << endl;
	for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
		wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
	}

	HANDLE _hin = GetStdHandle(STD_INPUT_HANDLE);
	while (!WaitForSingleObject(_hin, INFINITE)) {
		ReadConsoleInput(_hin, &pin, 1, &event_count);
		if (pin.EventType == KEY_EVENT) {
			if (pin.Event.KeyEvent.bKeyDown) {
				if (pin.Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					fileExplorer.up();

					system("cls");
					fileExplorer.getPath(p);
					wcout << "Current dir: " << p << endl;
					for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
						//wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
						wprintf((i == fileExplorer.currentPos ? L"> " : L"  "));
						wprintf(fileExplorer.fileList[i].name.c_str());
						wprintf(L"\n");
					}
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
					fileExplorer.down();

					system("cls");
					fileExplorer.getPath(p);
					wcout << "Current dir: " << p << endl;
					for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
						//wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
						wprintf((i == fileExplorer.currentPos ? L"> " : L"  "));
						wprintf(fileExplorer.fileList[i].name.c_str());
						wprintf(L"\n");
					}
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
					fileExplorer.enter();

					system("cls");
					fileExplorer.getPath(p);
					wcout << "Current dir: " << p << endl;
					for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
						//wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
						wprintf((i == fileExplorer.currentPos ? L"> " : L"  "));
						wprintf(fileExplorer.fileList[i].name.c_str());
						wprintf(L"\n");
					}
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F2) {
					system("cls");
					int i = fileExplorer.currentPos;
					filecopy.StartCopy(fileExplorer.fileList[i].fullname);
				
					system("cls");
					fileExplorer.getPath(p);
					wcout << "Current dir: " << p << endl;
					for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
						//wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
						wprintf((i == fileExplorer.currentPos ? L"> " : L"  "));
						wprintf(fileExplorer.fileList[i].name.c_str());
						wprintf(L"\n");
					}
				}
			}
		}
	}
}
