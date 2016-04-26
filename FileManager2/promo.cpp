#include "promo.h"

void WriteToLog(std::wstring str)
{
	static std::wofstream file;
	
	time_t t = time(0);
	if (!file.is_open()) {
		// create file, name is milliseconds now
		file.open(toString(t));
		// check
		if (!file.is_open()) return;
	}
	tm now;
	localtime_s(&now, &t);

	file << now.tm_hour << ":" << now.tm_min << ":" << now.tm_sec << " " << str << std::endl;
	file.flush();
}

void CIFillChar(CHAR_INFO * ci, int from, int count, wchar_t c)
{
	for (int i = from; i < count; i++) {
		ci[i].Char.UnicodeChar = c;
	}
}

void CIFillColor(CHAR_INFO * ci, int from, int count, int color)
{
	for (int i = from; i < count; i++) {
		ci[i].Attributes = color;
	}
}

void CIHeader(CHAR_INFO * ci, int from, int length, std::wstring hdr)
{
	ci[from].Char.UnicodeChar = 9484;
	ci[from + 1].Char.UnicodeChar = 9472;
	ci[from + 2].Char.UnicodeChar = L' ';
	if (hdr != L"") {
		for (int i = from + 3, j = 0; i < from + length - 2; i++, j++) {
			if (j == hdr.length() || (i == from + length - 3 && j < hdr.length()))
				ci[i].Char.UnicodeChar = L' ';
			else if (j < hdr.length())
				ci[i].Char.UnicodeChar = hdr[j];
			else
				ci[i].Char.UnicodeChar = 9472;
		}
	}
	else {
		for (int i = from + 2; i < from + length - 2; i++) {
			ci[i].Char.UnicodeChar = 9472;
		}
	}
	ci[from + length - 2].Char.UnicodeChar = 9472;
	ci[from + length - 1].Char.UnicodeChar = 9488;
}

void CIFooter(CHAR_INFO * ci, int from, int length, std::wstring ftr)
{
	ci[from].Char.UnicodeChar = 9492;
	ci[from + 1].Char.UnicodeChar = 9472;
	if (ftr != L"") {
		ci[from + 2].Char.UnicodeChar = L' ';
		for (int i = from + 3, j = 0; i < from + length - 2; i++, j++) {
			if (j == ftr.length() || (i == from + length - 3 && j < ftr.length()))
				ci[i].Char.UnicodeChar = L' ';
			else if (j < ftr.length())
				ci[i].Char.UnicodeChar = ftr[j];
			else
				ci[i].Char.UnicodeChar = 9472;
		}
	}
	else {
		for (int i = from + 2; i < from + length - 2; i++) {
			ci[i].Char.UnicodeChar = 9472;
		}
	}
	ci[from + length - 2].Char.UnicodeChar = 9472;
	ci[from + length - 1].Char.UnicodeChar = 9496;
}

void CIBorder(CHAR_INFO * ci, int r, int c, std::wstring hdr, std::wstring ftr)
{
	if (r < 2) return;
	CIHeader(ci, 0, c, hdr);
	CIFooter(ci, r*c - c, c, ftr);
	for (int i = 1; i < r - 1; i++) {
		ci[c*i].Char.UnicodeChar = 9474;
		ci[c*i+c-1].Char.UnicodeChar = 9474;
	}
}

void CIText(CHAR_INFO * ci, int r, int c, std::wstring str)
{
	for (int i = 2, j = 0; i < r - 2; i++) {
		for (int k = 0; j < str.length() && k < c - 4; k++, j++) {
			if (j < str.length()) {
				if (str[j] == '\n') {
					j++;
					break;
				}
				ci[i*c + k + 2].Char.UnicodeChar = str[j];
			}
			else {
				ci[i].Char.UnicodeChar = L' ';
			}
		}
	}
}
