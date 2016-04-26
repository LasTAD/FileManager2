#pragma once
#include "fileman.h"
using namespace std;

struct sym	//структуры или записи
{
	unsigned char ch;
	float freq;
	char code[255];
	sym *left;
	sym *right;
};

union code
{
	unsigned char chhh;	//переменная содержащая код для записи в сжатый файл

	struct byte
	{
		unsigned b1 : 1;
		unsigned b2 : 1;
		unsigned b3 : 1;
		unsigned b4 : 1;
		unsigned b5 : 1;
		unsigned b6 : 1;
		unsigned b7 : 1;
		unsigned b8 : 1;
	}byte;
};

class Archive {
public:
	void StartArch(wstring &);
private:
	void MakingArchive(wstring &, wstring &);
	sym *makeTree(sym *psym[], int k);
	void makeCodes(sym *root);
private:
	FILE *fp, *fp2, *fp3;			//указатели на файлы
	int chh;						// в эту переменную читается информация из файла
	int counUnSym = 0;				//счётчик количества различных букв, уникальных символов
	int counSym = 0;				// счётчтк количества всех знаков в файле
	int fsize2 = 0;					//счётчик количества символов из 0 и 1 в промежуточном файле teemp
	int remSize;					//размер хвоста файла (то, что не кратно 8 в промежуточном файле)
	int quan[256] = { 0 };			//инициализируем массив количества уникальных символов
	sym simbols[256] = { 0 };		//инициализируем массив записей 
	sym *psym[256];					//инициализируем массив указателей на записи
	float sumFreMet = 0;			//сумма частот встречаемости
	int mas[8];						//массив 0 и 1
	char j = 0;						//вспомогательная переменная
};
