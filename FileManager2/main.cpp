#include "fileman.h"

int main() {
	/*
	HANDLE csb = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	wstring data = L"������ ������� ��������. Is this some kind of perverse game the criminal is playing he wondered. �� ����� �� ������� ����� ������ �������, ������� �� ������ �������� ���� ����, ���� ���� �������� ������ ���������. ��� �������� ����� ����� �����, ���� ��� ��������� �� ���� �������. ���� ���, ����������, ������ ���� ���������� ��������, ������ ����, ������� ������. Didn see them. ������ �� ���. ��������, ����� ����, ����� �� ������� �� ������ ����������, ��� ��� ���������. ������, ��� ���� ������� ����������� ���� �� �������� �������, �� �� �� ��������� �� ���������� ��� ��������. ����� ������������ ������������� ����� �������� ������� ����-�-���� �� ������, ������ ������, �� �� ������ ������� ��� �� ������ �� ������ ��� ��� ����� �������� ������ ������. ����� ������� �������� �������������� ������ �����. �� ������ ����� ������� ������ �� ��� ���, �� ����� ����������� ������� ������, ������, ��� �������� �����. �� ����� ����� ���������� ������ ���������. ��� ��� ��� ������ ������, ����� ����������� ���� ��� �������� ������ �����. ��� �������������� ������� ���������� ����� �������� �������� ����������� ��������������� ������� �� ������� �� ������ ����������. 65 ���������, ��� ����-����� �������� ����������� ����������� ����������� ����� ������ ����� ���������� ������ ���������: ���� ������� ���� ���������� ������ ������ ��. ������� ������ ������������� ��������� ������������� ����������� ��������� �� ������� ���� ��������� ������ ����� ���. �������� ���.";
	DWORD written;
	SetConsoleScreenBufferSize(csb, { 40, 1000 });
	bool success = WriteConsoleW(csb, data.c_str(), data.length(), &written, NULL);
	SetConsoleActiveScreenBuffer(csb);
	system("pause");*/

	/*HANDLE csb = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	wstring data = L"������ ������� ��������. Is this some kind of perverse game the criminal is playing he wondered. �� ����� �� ������� ����� ������ �������, ������� �� ������ �������� ���� ����, ���� ���� �������� ������ ���������. ��� �������� ����� ����� �����, ���� ��� ��������� �� ���� �������. ���� ���, ����������, ������ ���� ���������� ��������, ������ ����, ������� ������. Didn see them. ������ �� ���. ��������, ����� ����, ����� �� ������� �� ������ ����������, ��� ��� ���������. ������, ��� ���� ������� ����������� ���� �� �������� �������, �� �� �� ��������� �� ���������� ��� ��������. ����� ������������ ������������� ����� �������� ������� ����-�-���� �� ������, ������ ������, �� �� ������ ������� ��� �� ������ �� ������ ��� ��� ����� �������� ������ ������. ����� ������� �������� �������������� ������ �����. �� ������ ����� ������� ������ �� ��� ���, �� ����� ����������� ������� ������, ������, ��� �������� �����. �� ����� ����� ���������� ������ ���������. ��� ��� ��� ������ ������, ����� ����������� ���� ��� �������� ������ �����. ��� �������������� ������� ���������� ����� �������� �������� ����������� ��������������� ������� �� ������� �� ������ ����������. 65 ���������, ��� ����-����� �������� ����������� ����������� ����������� ����� ������ ����� ���������� ������ ���������: ���� ������� ���� ���������� ������ ������ ��. ������� ������ ������������� ��������� ������������� ����������� ��������� �� ������� ���� ��������� ������ ����� ���. �������� ���. ������ ������� ��������. Is this some kind of perverse game the criminal is playing he wondered. �� ����� �� ������� ����� ������ �������, ������� �� ������ �������� ���� ����, ���� ���� �������� ������ ���������. ��� �������� ����� ����� �����, ���� ��� ��������� �� ���� �������. ���� ���, ����������, ������ ���� ���������� ��������, ������ ����, ������� ������. Didn see them. ������ �� ���. ��������, ����� ����, ����� �� ������� �� ������ ����������, ��� ��� ���������. ������, ��� ���� ������� ����������� ���� �� �������� �������, �� �� �� ��������� �� ���������� ��� ��������. ����� ������������ ������������� ����� �������� ������� ����-�-���� �� ������, ������ ������, �� �� ������ ������� ��� �� ������ �� ������ ��� ��� ����� �������� ������ ������. ����� ������� �������� �������������� ������ �����. �� ������ ����� ������� ������ �� ��� ���, �� ����� ����������� ������� ������, ������, ��� �������� �����. �� ����� ����� ���������� ������ ���������. ��� ��� ��� ������ ������, ����� ����������� ���� ��� �������� ������ �����. ��� �������������� ������� ���������� ����� �������� �������� ����������� ��������������� ������� �� ������� �� ������ ����������. 65 ���������, ��� ����-����� �������� ����������� ����������� ����������� ����� ������ ����� ���������� ������ ���������: ���� ������� ���� ���������� ������ ������ ��. ������� ������ ������������� ��������� ������������� ����������� ��������� �� ������� ���� ��������� ������ ����� ���. �������� ���. ������ ������� ��������. Is this some kind of perverse game the criminal is playing he wondered. �� ����� �� ������� ����� ������ �������, ������� �� ������ �������� ���� ����, ���� ���� �������� ������ ���������. ��� �������� ����� ����� �����, ���� ��� ��������� �� ���� �������. ���� ���, ����������, ������ ���� ���������� ��������, ������ ����, ������� ������. Didn see them. ������ �� ���. ��������, ����� ����, ����� �� ������� �� ������ ����������, ��� ��� ���������. ������, ��� ���� ������� ����������� ���� �� �������� �������, �� �� �� ��������� �� ���������� ��� ��������. ����� ������������ ������������� ����� �������� ������� ����-�-���� �� ������, ������ ������, �� �� ������ ������� ��� �� ������ �� ������ ��� ��� ����� �������� ������ ������. ����� ������� �������� �������������� ������ �����. �� ������ ����� ������� ������ �� ��� ���, �� ����� ����������� ������� ������, ������, ��� �������� �����. �� ����� ����� ���������� ������ ���������. ��� ��� ��� ������ ������, ����� ����������� ���� ��� �������� ������ �����. ��� �������������� ������� ���������� ����� �������� �������� ����������� ��������������� ������� �� ������� �� ������ ����������. 65 ���������, ��� ����-����� �������� ����������� ����������� ����������� ����� ������ ����� ���������� ������ ���������: ���� ������� ���� ���������� ������ ������ ��. ������� ������ ������������� ��������� ������������� ����������� ��������� �� ������� ���� ��������� ������ ����� ���. �������� ���.";
	DWORD written;
	SetConsoleScreenBufferSize(csb, { 40, 1000 });
	bool success = WriteConsoleW(csb, data.c_str(), data.length(), &written, NULL);
	CONSOLE_SCREEN_BUFFER_INFOEX iex;
	GetConsoleScreenBufferInfoEx(csb, &iex);*/

	Console console;
	console.work();
}