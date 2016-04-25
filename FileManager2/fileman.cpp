#include "fileman.h"
#include "file_act.h"
#include "arch.h"

void FileExplorer::parsePath()
{
	fileList.clear();
	if (path.size() == 0) {
		wchar_t drives[128];
		DWORD count = GetLogicalDriveStringsW(128, drives);
		for (DWORD i = 0; i < count; i += 4) {
			File file;
			file.name = drives[i];
			file.fullname = drives[i];
			file.isDrive = true;
			file.isDir = false;
			
			ULARGE_INTEGER FreeBytesAvailable;
			ULARGE_INTEGER TotalNumberOfBytes;
			ULARGE_INTEGER TotalNumberOfFreeBytes;
			BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceExW(
				&drives[i],									// directory name
				(PULARGE_INTEGER)&FreeBytesAvailable,		// bytes available to caller
				(PULARGE_INTEGER)&TotalNumberOfBytes,		// bytes on disk
				(PULARGE_INTEGER)&TotalNumberOfFreeBytes	// free bytes on disk
				);
			if (!GetDiskFreeSpaceFlag) ErrorMessage(L"Can't check disk space");
			file.size = TotalNumberOfBytes.QuadPart - TotalNumberOfFreeBytes.QuadPart;

			fileList.push_back(file);
		}
	}
	else {
		wstring path = getPath() + L"*.*";
		vector<PWIN32_FIND_DATAW> foundFiles;
		GetFileList(path, foundFiles);
		// check error
		File dotdot;
		dotdot.fullname = L"..";
		dotdot.isDir = true;
		dotdot.name = L"..";
		dotdot.modifier = true;
		fileList.push_back(dotdot);
		for (auto f : foundFiles) {
			File file;
			file.name = f->cFileName;
			file.fullname = path + f->cFileName;
			file.size = (f->nFileSizeHigh * MAXDWORD) + f->nFileSizeLow;
			file.isDir = (bool)(f->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			fileList.push_back(file);
			delete f;
		}
	}

	currentPos = 0;
	first = 0;
	last = min(fileList.size() - 1, filesPerPage - 1);
	page = 0;

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

void FileExplorer::updateView()
{
	/*
	if (currentPos > last) {
		last = currentPos;
		first = last - filesPerPage + 1;
	}
	else if (currentPos < first) {
		first = currentPos;
		last = first + filesPerPage - 1;
	}*/
	page = currentPos / filesPerPage;
	first = filesPerPage * page;
	last = min(filesPerPage * (page + 1) - 1, fileList.size() - 1);
}

void FileExplorer::enter()
{
	if (fileList[currentPos].modifier) {

		// TODO: make disks list window

		if (path.size() > 0) // stopgap
			path.pop_back();

		parsePath();
	}
	else if (fileList[currentPos].isDir || fileList[currentPos].isDrive) {
		path.push_back(fileList[currentPos].name);
		parsePath();
	}
}

wstring FileExplorer::getPath()
{
	if (path.size() == 0) return L"";
	wstringstream wss;
	wss << path[0] << L":\\";
	for (int i = 1; i < path.size(); i++) {
		wss << path[i] << L'\\';
	}
	return wss.str();
}

void Console::setColor(int color)
{
	SetConsoleTextAttribute(hstdout, color);
}

void Console::setCursorPos(short x, short y)
{
	SetConsoleCursorPosition(hstdout, { x,y });
}

void Console::drawExplorersBorder()
{
	setlocale(2, "C");
	// Header
	setCursorPos(0, 0);
	wcout << c(218) << l(196, 84) << c(194) << l(196, 20) << c(194) << l(196, 20) << c(191);
	wcout << c(179) << left << setw(84) << L"Name" << c(179) << setw(20) << L"Size" << c(179) << setw(20) << L"Type" << c(179);
	wcout << c(195) << l(196, 84) << c(197) << l(196, 20) << c(197) << l(196, 20) << c(180);
	// Left and right
	for (int i = 3; i < 35; i++) { setCursorPos(0, i); wcout << c(179); setCursorPos(85, i); wcout << c(179); setCursorPos(106, i); wcout << c(179); setCursorPos(127, i); wcout << c(179); }
	// Footer
	wcout << c(195) << l(196, 84) << c(197) << l(196, 20) << c(197) << l(196, 20) << c(180);
	setCursorPos(0, 36); wcout << c(179); setCursorPos(85, 36); wcout << c(179); setCursorPos(106, 36); wcout << c(179); setCursorPos(127, 36); wcout << c(179);
	wcout << c(192) << l(196, 84) << c(193) << l(196, 20) << c(193) << l(196, 20) << c(217);
	setlocale(2, "rus");
}

void Console::draw()
{
	// а здесь будет страшная дичь
	static int e_first, e_last, e_cursorPos;
	if (e_first == fileExplorer.first && e_last == fileExplorer.last) {
		if (e_cursorPos != fileExplorer.currentPos) {
			/*
			setColor(FWhite | BBlack);
			setCursorPos(1, e_cursorPos + 3);
			wcout << crop(fileExplorer.fileList[e_cursorPos].name, 84);
			setCursorPos(86, e_cursorPos + 3);
			wcout << (fileExplorer.fileList[e_cursorPos].isDir ? l(' ', 20) : cropf(fileExplorer.fileList[e_cursorPos].size, 20));
			setCursorPos(107, e_cursorPos + 3);
			wcout << crop(fileExplorer.fileList[e_cursorPos].isDir ? L"directory" : L"file", 20);
			setColor(FBlack | BWhite);
			setCursorPos(1, fileExplorer.currentPos + 3);
			wcout << crop(fileExplorer.fileList[fileExplorer.currentPos].name, 84);
			setCursorPos(86, fileExplorer.currentPos + 3);
			wcout << (fileExplorer.fileList[fileExplorer.currentPos].isDir ? l(' ', 20) : cropf(fileExplorer.fileList[fileExplorer.currentPos].size, 20));
			setCursorPos(107, fileExplorer.currentPos + 3);
			wcout << crop(fileExplorer.fileList[fileExplorer.currentPos].isDir ? L"directory" : L"file", 20);*/
			CHAR_INFO ci1[128], ci2[128];
			SMALL_RECT sr1, sr2;
			sr1.Left = sr2.Left = 0;
			sr1.Right = sr2.Right = 128;
			sr1.Top = sr1.Bottom = e_cursorPos - fileExplorer.first + 3;
			sr2.Top = sr2.Bottom = fileExplorer.currentPos - fileExplorer.first + 3;
			ReadConsoleOutputW(hstdout, ci1, { 128,1 }, { 0, 0 }, &sr1);
			ReadConsoleOutputW(hstdout, ci2, { 128,1 }, { 0, 0 }, &sr2);
			for (int i = 1; i < 127; i++) {
				if (i == 85 || i == 106) continue;
				ci1[i].Attributes = FWhite | BBlack;
				ci2[i].Attributes = BWhite | FBlack;
			}
			WriteConsoleOutputW(hstdout, ci1, { 128,1 }, { 0,0 }, &sr1);
			WriteConsoleOutputW(hstdout, ci2, { 128,1 }, { 0,0 }, &sr2);
		}
	}
	// стандартная отрисовка эксплорера
	else for (int i = fileExplorer.first, j = 3; j < 35; i++, j++) {
		if (i <= fileExplorer.last) {
			setCursorPos(1, j);
			if (i == fileExplorer.currentPos) setColor(FBlack | BWhite); else setColor(FWhite | BBlack);
			wcout << crop(fileExplorer.fileList[i].name, 84);
			setCursorPos(86, j);
			wcout << (fileExplorer.fileList[i].isDir ? l(' ', 20) : cropf(fileExplorer.fileList[i].size, 20));
			setCursorPos(107, j);
			wcout << crop(fileExplorer.fileList[i].isDir ? L"Directory" : (fileExplorer.fileList[i].isDrive ? L"Drive" : L"File"), 20);
		}
		else {
			setColor(FWhite | BBlack);
			setCursorPos(1, j);
			wcout << l(' ', 84);
			setCursorPos(86, j);
			wcout << l(' ', 20);
			setCursorPos(107, j);
			wcout << l(' ', 20);
		}
	}
	e_first = fileExplorer.first;
	e_last = fileExplorer.last;
	e_cursorPos = fileExplorer.currentPos;
	/*
	system("cls");
	wcout << "Current dir: " << fileExplorer.getPath() << endl;
	for (int i = fileExplorer.first; i <= fileExplorer.last; i++) {
		//wcout << (i == fileExplorer.currentPos ? "> " : "  ") << fileExplorer.fileList[i].name << endl;
		if (i == fileExplorer.currentPos) {
			SetColor(15, 1);
		}
		wprintf(fileExplorer.fileList[i].name.c_str());
		wprintf(L"\n");
		SetColor(7, 0);
	}
	SetColor(15, 3);
	wcout << "F2";
	SetColor(7, 0);
	wcout << "Copy\t";
	SetColor(15, 3);
	wcout << "F3";
	SetColor(7, 0);
	wcout << "Delete  ";
	SetColor(15, 3);
	wcout << "F4";
	SetColor(7, 0);
	wcout << "Change name  ";
	SetColor(15, 3);
	wcout << "F5";
	SetColor(7, 0);
	wcout << "Make archive";
	*/
}

Console::Console()
{
	// Запомним хэндлы
	hstdin = GetStdHandle(STD_INPUT_HANDLE);
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hideCursor();
	// Установим размер окна
	COORD size{ CONSOLE_SIZE_X, CONSOLE_SIZE_Y };
	SMALL_RECT rect{ 0, 0, CONSOLE_SIZE_X-1, CONSOLE_SIZE_Y-1 };
	SetConsoleScreenBufferSize(hstdout, size);
	SetConsoleWindowInfo(hstdout, true, &rect);
	SetConsoleTitleW(L"Файловый менеджер");
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
	FileDel filedel;
	Archive arch;
	hstdin = GetStdHandle(STD_INPUT_HANDLE);
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	hideCursor();
	fileExplorer.parsePath();

	INPUT_RECORD pin;
	DWORD event_count;

	drawExplorersBorder();
	draw();
	
	while (!WaitForSingleObject(hstdin, INFINITE)) {
		ReadConsoleInput(hstdin, &pin, 1, &event_count);
		if (pin.EventType == KEY_EVENT) {
			if (pin.Event.KeyEvent.bKeyDown) {
				if (pin.Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					fileExplorer.up();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
					fileExplorer.down();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_RETURN) {
					fileExplorer.enter();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F2) {
					system("cls");
					int i = fileExplorer.currentPos;
					filecopy.StartCopy(fileExplorer.fileList[i].fullname, fileExplorer.fileList[i].isDir);
					drawExplorersBorder();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F3) {
					int i = fileExplorer.currentPos;

					filedel.DelFile(fileExplorer.fileList[i].fullname);
					drawExplorersBorder();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F4) {
					system("cls");
					int i = fileExplorer.currentPos;

					filedel.ChName(fileExplorer.fileList[i].fullname);
					drawExplorersBorder();
					draw();
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F5) {
					system("cls");
					int i = fileExplorer.currentPos;

					arch.StartArch(fileExplorer.fileList[i].fullname);
					drawExplorersBorder();
					draw();
				}
			}
		}
	}
	/*
	while (true) {
		int t = _getch();
		if (t == 224) {
			t = _getch();
			if (t == 72) {
				fileExplorer.up();
				draw();
			}
			if (t == 80) {
				fileExplorer.down();
				draw();
			}
		}
		if (t == 27) break;
		//MessageBoxW(NULL, crop(t, 30).c_str(), L"", 0);
	}*/
}
