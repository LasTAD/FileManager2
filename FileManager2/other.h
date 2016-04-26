#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include <sstream>
#include "promo.h"

#define  _CRT_SECURE_NO_WARNINGS

struct File {
	std::wstring	name;
	std::wstring	fullname;
	ULONG64			size;
	int				fAttr;
};

typedef File* PFile;

enum FAttributes {
	DRIVE = 1,
	DOTDOT = 2,
	DIR = 3,
	FIL = 0
};
enum ConsoleColor
{
	FBlack = 0,
	FBlue = 1,
	FGreen = 2,
	FCyan = 3,
	FRed = 4,
	FMagenta = 5,
	FBrown = 6,
	FLightGray = 7,
	FDarkGray = 8,
	FLightBlue = 9,
	FLightGreen = 10,
	FLightCyan = 11,
	FLightRed = 12,
	FLightMagenta = 13,
	FYellow = 14,
	FWhite = 15,
	BBlack = 0,
	BBlue = 16,
	BGreen = 32,
	BCyan = 48,
	BRed = 64,
	BMagenta = 80,
	BBrown = 96,
	BLightGray = 112,
	BDarkGray = 128,
	BLightBlue = 144,
	BLightGreen = 160,
	BLightCyan = 176,
	BLightRed = 192,
	BLightMagenta = 208,
	BYellow = 224,
	BWhite = 240
};

// ������� ������� ��������� �� ������
void ErrorMessage(wchar_t*);
void ErrorMessage(std::wstring&);

// ������� ��� ������ ������, ���������� 0 � ������ ������
int GetFileList(wchar_t*, std::vector<PWIN32_FIND_DATAW>&);
int GetFileList(std::wstring, std::vector<PWIN32_FIND_DATAW>&);

// ������� ��� ��������� ������ � ����������� ��������� ������������ ����� (�������������� �����)
std::wstring l(char, int);

// ������� ��� ��������� ������ �� ������������� ���������� ���������� ����� (�������������� �����)
std::wstring l(char*, int);
std::wstring l(std::wstring, int);

// ������� ��� �������������� int � char (�������������� ����� ������ <(char)�����>)
char c(int);

// ������� ��� ������� ������/����� �� ������������� ���������� ��������, ���� ��� �������, ��� ���������� ���������, ���� ������ (�������������� �����)
std::wstring crop(std::wstring, int);
std::wstring cropf(std::wstring, int); // �� �� �����, �� �������
std::wstring cropt(std::wstring, int); // �� �� �����, �� ������� �������

// ������� ������� � ����������, �� ���������� � �������, ���������� �� � �����-����� �������� �������� ���
std::wstring crop(ULONG64,int);
std::wstring cropf(ULONG64, int); // �� �� �����, �� �������

// ������� ��� �������������� ���� � ������
template <class T>
std::wstring toString(const T& t)
{
	std::wstringstream ss;
	ss << t;
	return ss.str();
}