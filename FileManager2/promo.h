#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "other.h"

// ������ �������� � ���
void WriteToLog(std::wstring);

// �������� ��������� ��������
void CIFillChar(CHAR_INFO*, int, int, wchar_t);
// �������� ��������� ������
void CIFillColor(CHAR_INFO*, int, int, int);
// �������� ���������
void CIHeader(CHAR_INFO*, int, int, std::wstring = L"");
// �������� footer
void CIFooter(CHAR_INFO*, int, int, std::wstring = L"");
// �������� ����� ���� � ���������� � footer'��
void CIBorder(CHAR_INFO*, int, int, std::wstring = L"", std::wstring = L"");
// �������� ����� � ����
void CIText(CHAR_INFO*, int, int, std::wstring);