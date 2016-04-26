#pragma once
#include "fileman.h"
using namespace std;

struct sym	//��������� ��� ������
{
	unsigned char ch;
	float freq;
	char code[255];
	sym *left;
	sym *right;
};

union code
{
	unsigned char chhh;	//���������� ���������� ��� ��� ������ � ������ ����

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
	FILE *fp, *fp2, *fp3;			//��������� �� �����
	int chh;						// � ��� ���������� �������� ���������� �� �����
	int counUnSym = 0;				//������� ���������� ��������� ����, ���������� ��������
	int counSym = 0;				// ������� ���������� ���� ������ � �����
	int fsize2 = 0;					//������� ���������� �������� �� 0 � 1 � ������������� ����� teemp
	int remSize;					//������ ������ ����� (��, ��� �� ������ 8 � ������������� �����)
	int quan[256] = { 0 };			//�������������� ������ ���������� ���������� ��������
	sym simbols[256] = { 0 };		//�������������� ������ ������� 
	sym *psym[256];					//�������������� ������ ���������� �� ������
	float sumFreMet = 0;			//����� ������ �������������
	int mas[8];						//������ 0 � 1
	char j = 0;						//��������������� ����������
};
