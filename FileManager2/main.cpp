#include "fileman.h"

int main() {
	FileExplorer files;
	files.path.push_back(L"Windows"); 
	files.parsePath();

	INPUT_RECORD pin;
	DWORD event_count;

	for (int i = files.first; i <= files.last; i++) {
		wcout << (i == files.currentPos ? "> " : "  ") << files.fileList[i].name << endl;
	}

	HANDLE _hin = GetStdHandle(STD_INPUT_HANDLE);
	while (!WaitForSingleObject(_hin, INFINITE)) {
		ReadConsoleInput(_hin, &pin, 1, &event_count);
		if (pin.EventType == KEY_EVENT) {
			if (pin.Event.KeyEvent.bKeyDown) {
				if (pin.Event.KeyEvent.wVirtualKeyCode == VK_UP) {
					files.up();

					system("cls");
					for (int i = files.first; i <= files.last; i++) {
						wcout << (i == files.currentPos ? "> " : "  ") << files.fileList[i].name << endl;
					}
				}
				else if (pin.Event.KeyEvent.wVirtualKeyCode == VK_DOWN) {
					files.down();

					system("cls");
					for (int i = files.first; i <= files.last; i++) {
						wcout << (i == files.currentPos ? "> " : "  ") << files.fileList[i].name << endl;
					}
				}
				/*
				int min = max(files->currentPos - 20 + 1, 0);
				int max = min(min+20, files->fileList.size());
				for (int i = min; i < max; i++) {
				wcout << (i == files->currentPos ? "> " : "  ") << files->fileList[i].name << endl;
				}*/
			}
		}
	}
}