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
	wchar_t			drive = L'C';
public:
	vector<wstring> path;
	int				currentPos = 0;		// �����, ��������� int ������ :)
	int				first, last;
	int				filesPerPage = 32;
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
	void			draw();
public:
	Console();
	void			hideCursor();
	void			showCursor();
	void			work();
	void			setColor(int);
	void			setCursorPos(short, short);
};