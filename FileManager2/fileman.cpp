#include "fileman.h"
#include "file_act.h"
#include "arch.h"

bool FileExplorer::parsePath()
{
	if (path.size() == 0) {
		wchar_t drives[128];
		DWORD count = GetLogicalDriveStringsW(128, drives);
		fileList.clear();
		for (DWORD i = 0; i < count; i += 4) {
			File file;
			file.name = drives[i];
			file.fullname = drives[i];
			file.fAttr = DRIVE;
			
			//ULARGE_INTEGER FreeBytesAvailable;
			//ULARGE_INTEGER TotalNumberOfBytes;
			//ULARGE_INTEGER TotalNumberOfFreeBytes;
			//BOOL GetDiskFreeSpaceFlag = GetDiskFreeSpaceExW(
			//	&drives[i],									// directory name
			//	(PULARGE_INTEGER)&FreeBytesAvailable,		// bytes available to caller
			//	(PULARGE_INTEGER)&TotalNumberOfBytes,		// bytes on disk
			//	(PULARGE_INTEGER)&TotalNumberOfFreeBytes	// free bytes on disk
			//	);
			////if (!GetDiskFreeSpaceFlag) ErrorMessage(L"Can't check disk space");
			//file.size = TotalNumberOfBytes.QuadPart - TotalNumberOfFreeBytes.QuadPart;

			fileList.push_back(file);
		}
	}
	else {
		wstring path = getPath();
		vector<PWIN32_FIND_DATAW> foundFiles;
		if (GetFileList(path, foundFiles) != 0) return false;
		fileList.clear();
		// check error
		File dotdot;
		dotdot.fullname = L"..";
		dotdot.fAttr= DOTDOT;
		dotdot.name = L"..";
		fileList.push_back(dotdot);
		for (auto f : foundFiles) {
			File file;
			file.name = f->cFileName;
			file.fullname = path + f->cFileName;
			file.size = (f->nFileSizeHigh * MAXDWORD) + f->nFileSizeLow;
			
			if ((bool)(f->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				file.fAttr = DIR;
			else if ((bool)(f->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM))
				file.fAttr = SYS;
			else 
				file.fAttr = FIL;
			wstringstream wr; wr << "/"; //wr << f->cFileName << L"   " << (unsigned long)f->dwFileAttributes;
			if (f->dwFileAttributes & FILE_ATTRIBUTE_READONLY) wr << "[readonly]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) wr << "[hidden]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) wr << "[sys]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) wr << "[dir]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) wr << "[arch]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_DEVICE) wr << "[device]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_NORMAL) wr << "[normal]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) wr << "[temp]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE) wr << "[sparse]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) wr << "[reparse]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) wr << "[compressed]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_OFFLINE) wr << "[offline]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_NOT_CONTENT_INDEXED) wr << "[nci]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED) wr << "[enc]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_INTEGRITY_STREAM) wr << "[intstr]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_VIRTUAL) wr << "[virt]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_NO_SCRUB_DATA) wr << "[nsd]";
			if (f->dwFileAttributes & FILE_ATTRIBUTE_EA) wr << "[ea]";
			//WriteToLog(wr.str());
			file.name += wr.str();
			fileList.push_back(file);
			delete f;
		}
	}

	currentPos = 0;
	first = 0;
	last = min(fileList.size() - 1, filesPerPage - 1);
	page = 0;

	updateView();
	return true;
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
	page = currentPos / filesPerPage;
	first = filesPerPage * page;
	last = min(filesPerPage * (page + 1) - 1, fileList.size() - 1);
}

void FileExplorer::enter()
{
	if (fileList[currentPos].fAttr==DOTDOT) {
		wstring tmp = path[path.size() - 1];
		path.pop_back();

		if (!parsePath()) path.push_back(tmp);
	}
	else if (fileList[currentPos].fAttr==DIR || fileList[currentPos].fAttr==DRIVE) {
		wstring name = fileList[currentPos].name.substr(0, fileList[currentPos].name.find(L'/'));
		//path.push_back(fileList[currentPos].name);
		path.push_back(name);
		if (!parsePath()) path.pop_back();
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

void Console::showError(wstring msg)
{
	CHAR_INFO ci[88 * 15];
	CHAR_INFO nci[88 * 15];
	SMALL_RECT sr;
	sr.Top = 12;
	sr.Bottom = 27;
	sr.Left = 20;
	sr.Right = 108;
	ReadConsoleOutputW(hstdout, ci, { 88, 15 }, { 0, 0 }, &sr);

	CIFillColor(nci, 0, 88 * 15, FWhite | BRed);
	CIFillChar(nci, 0, 88 * 15, L' ');
	CIBorder(nci, 15, 88, L"Error message", L"Press any key to continue");
	CIText(nci, 15, 88, msg);
	WriteConsoleOutputW(hstdout, nci, { 88, 15 }, { 0,0 }, &sr);

	int g = _getch();
	if (g == 0xE0 || g == 0) _getch();

	WriteConsoleOutputW(hstdout, ci, { 88, 15 }, { 0,0 }, &sr);
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
	wcout << c(195) << l(196, 84) << c(193) << l(196, 20) << c(193) << l(196, 20) << c(180);
	wcout << c(179) << l(' ', 126) << c(179);
	wcout << c(192) << l(196, 126) << c(217);
	wcout << L" F2 Copy | F3 Delete | F4 Change Name | F5 Make an Archive";
	setlocale(2, "rus");
}

void Console::draw(bool req)
{
	setCursorPos(1, 36);
	setColor(FWhite | BBlack);
	wcout << cropt(fileExplorer.getPath(), 126);
	// а здесь будет страшная дичь
	static int e_first, e_last, e_cursorPos, path_size;
	if (e_first == fileExplorer.first && e_last == fileExplorer.last && path_size == fileExplorer.path.size() && !req) {
		if (e_cursorPos != fileExplorer.currentPos) {
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
			wcout << (fileExplorer.fileList[i].fAttr ? l(' ', 20) : cropf(fileExplorer.fileList[i].size, 20));
			setCursorPos(107, j);
			wcout << crop(fileExplorer.fileList[i].fAttr==DIR ? L"Directory" : (fileExplorer.fileList[i].fAttr==DRIVE ? L"Drive" :(fileExplorer.fileList[i].fAttr == DOTDOT ? L"": (fileExplorer.fileList[i].fAttr == SYS ? L"System" : L"File"))), 20);
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
	path_size = fileExplorer.path.size();
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

void Console::showRename()
{

}

void Console::work()
{
	FileCopy filecopy;
	FileDel filedel;
	Archive arch;
	fileExplorer.parsePath();

	INPUT_RECORD pin;
	DWORD event_count;

	drawExplorersBorder();
	draw();

	while (!WaitForSingleObject(hstdin, INFINITE)) {
		ReadConsoleInputW(hstdin, &pin, 1, &event_count);
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
					filecopy.StartCopy(fileExplorer.fileList[i].fullname, fileExplorer.fileList[i].fAttr);
					drawExplorersBorder();
					draw(true);
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F3) {
					int i = fileExplorer.currentPos;

					filedel.StartDel(fileExplorer.fileList[i].fullname, fileExplorer.fileList[i].fAttr);
					drawExplorersBorder();
					fileExplorer.parsePath();
					draw(true);
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F4) {
					system("cls");
					int i = fileExplorer.currentPos;

					filedel.ChName(fileExplorer.fileList[i].fullname, fileExplorer.fileList[i].fAttr);
					drawExplorersBorder();
					fileExplorer.parsePath();
					draw(true);
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_F5) {
					system("cls");
					int i = fileExplorer.currentPos;

					arch.StartArch(fileExplorer.fileList[i].fullname);
					drawExplorersBorder();
					draw(true);
				}
			}
		}
	}
}
