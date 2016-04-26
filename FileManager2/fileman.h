#pragma once
#include "other.h"
#include <vector>
#include <memory>
#include <string>
#include <iomanip>
#include <sstream>
#include <Windows.h> 
#include <stdio.h>
#include <iostream>
#include <vector>
#include <conio.h>

using namespace std;

/* �����-���������
 * ��������� � ���������
 */
class FileExplorer {
public:
	vector<wstring> path;
	int				currentPos = 0;		// �����, ��������� int ������ :)
	int				first, last;
	int				filesPerPage = 32;
	int				page;
	vector<File>	fileList;
	void			parsePath();
	wstring			getPath();			// ���������� ������� ����������, � ������� �� ��������
	void			down();				// ���������� ������ ����
	void			up();				// ���������� ������ �����
	void			enter();			// �������� ���� � ���������� ���� ��������� ���� � HEX-��������� (TODO)
protected:
	void			updateView();		// ������������� ���������� ������ � ��������� ������� �������
};

#define CONSOLE_SIZE_X 128
#define CONSOLE_SIZE_Y 40

class Console {
	HANDLE			hstdout, hstdin;
	FileExplorer	fileExplorer;
	void			drawExplorersBorder();
	void			draw(bool req = false);
public:
	Console();
	void			hideCursor();
	void			showCursor();
	void			work();
	void			setColor(int);
	void			setCursorPos(short, short);
	void			showError(wstring msg);
};