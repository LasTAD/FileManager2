#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "other.h"

// Запись действий в лог
void WriteToLog(std::wstring);

// Записать несколько символов
void CIFillChar(CHAR_INFO*, int, int, wchar_t);
// Записать несколько цветов
void CIFillColor(CHAR_INFO*, int, int, int);
// Записать заголовок
void CIHeader(CHAR_INFO*, int, int, std::wstring = L"");
// Записать footer
void CIFooter(CHAR_INFO*, int, int, std::wstring = L"");
// Записать рамку окна с заголовком и footer'ом
void CIBorder(CHAR_INFO*, int, int, std::wstring = L"", std::wstring = L"");
// Записать текст в окно
void CIText(CHAR_INFO*, int, int, std::wstring);