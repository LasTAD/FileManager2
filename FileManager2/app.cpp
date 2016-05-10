#include "app.h"
#include "gui.h"
#include "files.h"
#include <sstream>

// Функция проверки имени файла/директории на правильность
// todo проверка директории
bool validateFilename(wstring str) {
	if (str.length() > 0 && str.find(L'*') == wstring::npos && str.find(L'|') == wstring::npos &&/*str.find(L'\\') == wstring::npos &&*/
		/*str.find(L':') == wstring::npos &&*/str.find(L'"') == wstring::npos &&str.find(L'<') == wstring::npos &&
		str.find(L'>') == wstring::npos &&str.find(L'?') == wstring::npos/* &&str.find(L'/') == wstring::npos*/) return true;
	return false;
}

Console::Console()
{	
	setlocale(0, "Russian");
	// Запомним хэндлы для удобства
	hin = GetStdHandle(STD_INPUT_HANDLE);
	hout = GetStdHandle(STD_OUTPUT_HANDLE);
	// Установим размеры экрана
	COORD size{ CONSOLE_SIZE_X, CONSOLE_SIZE_Y };
	SMALL_RECT rect{ 0, 0, CONSOLE_SIZE_X - 1, CONSOLE_SIZE_Y - 1 };
	// Установим общую информацию
	SetConsoleScreenBufferSize(hout, size);
	SetConsoleWindowInfo(hout, true, &rect);
	SetConsoleTitleW(L"File Manager");
	// И спрячем курсор
	hideCursor(hout);
	// Заблокируем возможность изменения размера
	DWORD newStyle = GetWindowLong(GetConsoleWindow(), GWL_STYLE); // В гугле ничего не нашел, пришлось поиздеваться над WinAPI, вуаля - все пашет
	newStyle &= ~WS_MAXIMIZEBOX;
	newStyle &= ~WS_SIZEBOX;
	SetWindowLong(GetConsoleWindow(), GWL_STYLE, newStyle);
	SetWindowPos(GetConsoleWindow(), NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
}

void Console::updatePages()
{
	if (pos < 0) pos = (int)files.size() - 1;
	if (pos > (int)files.size() - 1) pos = 0;
	page = pos / fpp;
	first = fpp * page;
	last = min(fpp * (page + 1) - 1, (int)files.size() - 1);
}

// пашет, если уже гарантируется, что доступ к дирке есть
void Console::updateFiles()
{
	showStateString(L"Parsing dir...");
	if (path.size() != 0) {
		vector<PWIN32_FIND_DATAW> findFiles;
		getFiles(getPath(), findFiles);
		// -----------
		for (auto f : files) delete f;
		files.clear();
		if (path.size() > 0) { // если мы листуем не диски, то назад вернуться можно
			PWIN32_FIND_DATAW dd = new WIN32_FIND_DATAW;
			dd->dwReserved1 = 1;
			dd->cFileName[0] = L'.';
			dd->cFileName[1] = L'.';
			dd->cFileName[2] = 0;
			files.push_back(dd);
		}
		int counter_d = 0;
		for (auto f : findFiles) {
			showStateString(L"Parsing dir " + to_wstring(counter_d) + L'/' + to_wstring(findFiles.size()));
			if (f->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				auto ce = countFiles(getPath() + f->cFileName + L"\\");
				if (ce.all != -1) {
					f->dwReserved1 = true;
					f->nFileSizeLow = ce.files;
					f->nFileSizeHigh = ce.dirs;
				}
			}
			files.push_back(f);
			++counter_d;
		}
	}
	else {
		wchar_t drives[128];
		DWORD count = GetLogicalDriveStringsW(128, drives);
		// проверка на ошибки, ну на всякий
		if (count == 0) {
			showDialogWindowErrorOk(hout, L"Не удалось получить список носителей. Программа будет закрыта", L"Ошибка");
			exit(0);
		}
		// -----------
		for (auto f : files) delete f;
		files.clear();
		for (DWORD i = 0; i < count; i += 4) {
			PWIN32_FIND_DATAW d = new WIN32_FIND_DATAW;
			d->dwReserved1 = 2;
			d->cFileName[0] = drives[i];
			d->cFileName[1] = 0;
			d->dwFileAttributes = 0;
			files.push_back(d);
		}
	}
	pos = 0;
	hideStateString();
	updatePages();
}

void Console::drawFiles(bool force)
{
	static int lp = 0xFFFFFFFF, ls, lps;
	if (lp != page || ls != path.size() || force) {
		drawTableFM(hout, files, first, last, pos);
	} else {
		drawSelectRow(hout, lps - first, TextWhite | BgBlack);
		drawSelectRow(hout, pos - first, TextBlack | BgWhite);
	}
	ls = (int)path.size();
	lp = page;
	lps = pos;
}

wstring Console::getPath()
{
	if (path.size() == 0) return L"";
	wstringstream wss;
	wss << path[0] << L":\\";
	for (int i = 1; i < (int)path.size(); i++) {
		wss << path[i] << L'\\';
	}
	return wss.str();
}

void Console::work()
{
	// переменная для смещения пути
	int path_shift = 0;

	// однократная отрисовка окна
	drawWindowFM(hout);
	// --------------------------

	// установка текущей директории
	wchar_t *dir = new wchar_t[1024];
	if (GetCurrentDirectoryW(1024, dir) != 0) {
		wchar_t *buf;
		wchar_t* part = wcstok_s(dir, L":\\", &buf);
		while (part) {
			path.push_back(part);
			part = wcstok_s(NULL, L"\\", &buf);
		}
	}
	else {
		showDialogWindowErrorOk(hout, L"Не удалось определить рабочую директорию", L"Ошибка");
	}
	delete dir;
	updateFiles();
	// ----------------------------

	// отрисовка файлов и директории
	drawFiles();
	drawPath(hout, getPath(), path_shift);
	// ----------------------------

	while (true) {
		int b = _getch();
		if (b == 0xE0) {
			b = _getch();

#pragma region
			if (b == 72) {
				pos--;
				updatePages();
				drawFiles();
			}
			else if (b == 80) {
				pos++;
				updatePages();
				drawFiles();
			}
			else if (b == 73) {
				pos -= fpp;
				if (pos < 0) {
					pos = 0;
				}
				updatePages();
				drawFiles();
			}
			else if (b == 81) {
				pos += fpp;
				if (pos > (int)files.size() - 1) {
					pos = (int)files.size() - 1;
				}
				updatePages();
				drawFiles();
			}
#pragma endregion Логика перемещения по папкам

#pragma region
			else if (b == 75) {
				size_t size = getPath().size();
				if (size > 126 && path_shift > 0) --path_shift;
				drawPath(hout, getPath(), path_shift);
			}
			else if (b == 77) {
				size_t size = getPath().size();
				if (size > 126 && size - path_shift > 126) ++path_shift;
				drawPath(hout, getPath(), path_shift);
			}
			else if (b == 71) {
				if (path_shift == 0) continue;
				path_shift = 0;
				drawPath(hout, getPath(), path_shift);
			}
			else if (b == 79) {
				int size = (int)getPath().size();
				if (size < 126 || path_shift == size - 126) continue;
				path_shift = size - 126;
				drawPath(hout, getPath(), path_shift);
			}
#pragma endregion Логика просмотра всего пути в строке
		}
		else if (b == 0) {
			int b = _getch();
			
			if (b == 59) { // f1 help
				showDialogWindowOk(hout, TextWhite | BgGreen, TextBlack | BgLightGreen, L"В\nMcDonalds\nлучше\nне\nобедать", L"Сообщение от разработчиков");
			}
			else if (b == 60) { // f2 rename
				if (files[pos]->dwReserved1 == 1 || files[pos]->dwReserved1 == 2) {
					showDialogWindowErrorOk(hout, L"К данному объекту нельзя применить операцию переименования", L"Ошибка");
					continue;
				}
				auto input = showDialogWindowInputOkCancel(hout, L"Введите новое имя:", L"Переименование", validateFilename);
				if (!input.canceled) {
					wstring oldp = getPath() + files[pos]->cFileName;
					wstring newp = getPath() + input.data;
					showStateString(L"Renaming element...");
					BOOL r = MoveFileW(oldp.c_str(), newp.c_str());
					hideStateString();
					if (!r) {
						DWORD val = GetLastError();
						showDialogWindowErrorOk(hout, errorCodeToString(val), L"Ошибка");
					}
					else {
						updateFiles();
						for (int i = 0; i < (int)files.size(); ++i) {
							if (wstring(files[i]->cFileName) == input.data) {
								pos = i;
								updatePages();
								break;
							}
						}
						drawFiles(true);
					}
				}
			}
			else if (b == 61) { // f3 copy
				// TODO
				auto input = showDialogWindowInputOkCancel(hout, L"Введите имя для копии:", L"Копирование", validateFilename);
				if (!input.canceled) {
					showStateString(L"Copying...");
					BOOL k = _copy(getPath() + files[pos]->cFileName, input.data, files[pos]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
					hideStateString();
					if (!k) {
						DWORD val = GetLastError();
						showDialogWindowErrorOk(hout, errorCodeToString(val), L"Ошибка");
					}
					else {
						updateFiles();
						for (int i = 0; i < (int)files.size(); ++i) {
							if (wstring(files[i]->cFileName) == input.data) {
								pos = i;
								updatePages();
								break;
							}
						}
						drawFiles(true);
					}
				}
				//_copy(getPath() + files[pos]->cFileName, getPath() + files[pos]->cFileName + L"1", files[pos]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
			}

			else if (b == 62) { // f4 delete
				if (files[pos]->dwReserved1 == 1 || files[pos]->dwReserved1 == 2) {
					showDialogWindowErrorOk(hout, L"К данному объекту нельзя применить операцию удаления", L"Ошибка");
					continue;
				}
				if (showDialogWindowYN(hout, L"Действительно удалить этот элемент?", L"Вопрос")) {
					showStateString(L"Deleting element...");
					bool r = removeFile(getPath() + files[pos]->cFileName, files[pos]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
					if (!r) {
						hideStateString();
						showDialogWindowErrorOk(hout, L"Element: " + getLastErrorFilename() + L'\n' + errorCodeToString(getLastErrorCode()), L"Ошибка");
					}
					else {
						hideStateString();
						int oldpos = pos;
						updateFiles();
						pos = oldpos--;
						updatePages();
						drawFiles(true);
					}
				}
			}
			else if (b == 63) { // f5 create dir
				if (path.size() == 0) {
					showDialogWindowErrorOk(hout, L"Создание директории здесь невозможно", L"Ошибка");
					continue;
				}
				auto input = showDialogWindowInputOkCancel(hout, L"Введите новое имя:", L"Переименование", validateFilename);
				if (!input.canceled) {
					showStateString(L"Creating directory...");
					BOOL r = CreateDirectoryW(wstring(getPath() + input.data).c_str(), NULL);
					hideStateString();
					if (!r) {
						DWORD val = GetLastError();
						showDialogWindowErrorOk(hout, errorCodeToString(val), L"Ошибка");
					}
					else {
						updateFiles();
						for (int i = 0; i < (int)files.size(); ++i) {
							if (wstring(files[i]->cFileName) == input.data) {
								pos = i;
								updatePages();
								break;
							}
						}
						drawFiles(true);
					}
				}
			}
		}
		// вход в папку
		else if (b == 13) {
			if (files[pos]->dwReserved1 == 1) {
				path.pop_back();
				updateFiles();
				drawFiles();
			}
			else if (!(files[pos]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && files[pos]->dwReserved1 != 2) {
				startEditor(hout, getPath() + files[pos]->cFileName);
				//showDialogWindowOk(hout, TextWhite | BgGreen, TextBlack | BgLightGreen, L"Здесь будет открываться HEX-редактор...", L"Сообщение от разработчиков");
			}
			else {
				// проверка на доступ, грубовато, но сойдет
				wstring newpath = getPath() + files[pos]->cFileName + (path.size() != 0 ? L"\\" : L":\\") + L"*.*"; // фича с диском
				HANDLE fh;
				WIN32_FIND_DATAW fd;
				if ((fh = FindFirstFileW(newpath.c_str(), &fd)) == INVALID_HANDLE_VALUE) {
					DWORD val = GetLastError();
					showDialogWindowErrorOk(hout, errorCodeToString(val), L"Ошибка");
					FindClose(fh);
					continue;
				}
				FindClose(fh);
				// ------------------
				path.push_back(files[pos]->cFileName);
				updateFiles();
				drawFiles();
			}
			path_shift = 0;
			drawPath(hout, getPath(), path_shift);
		}
		// ------------

		// выход из менеджера
		else if (b == 27) {
			if (showDialogWindowYN(hout, L"Вы действительно хотите покинуть программу?", L"Подтверждение выхода")) break;
		}
		// ------------------
	}
}
