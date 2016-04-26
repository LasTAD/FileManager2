#include "arch.h"
using namespace std;

void Archive::StartArch(wstring & inPath)
{
	wstring resPath;
	wcout << "\t\t\t***ARCHIVE_MAKER***\n";
	wcout << "Input path to archive: ";
	wcin >> resPath;
	MakingArchive(inPath, resPath);
}

void Archive::MakingArchive(wstring &src, wstring &res)
{	
	errno_t err;

	err = _wfopen_s(&fp, src.c_str(), L"rb");
	if (err) {
		MessageBoxW(NULL, L"Unable to open requested file!", L"Error", 0);
		return;
	}
	err = _wfopen_s(&fp2, L"temp.txt", L"wb");
	if (err) {
		MessageBoxW(NULL, L"Unable to create temp file!", L"Error", 0);
		return;
	}
	err = _wfopen_s(&fp3, res.c_str(), L"wb");
	if (err) {
		MessageBoxW(NULL, L"Unable to create compressed file!", L"Error", 0);
		return;
	}
	sym *symbols = (sym*)malloc(counUnSym*sizeof(sym));//�������� ������������� ������� �������� simbols
	sym **psum = (sym**)malloc(counUnSym*sizeof(sym*));//�������� ������������� ������� ���������� �� simbols

											   //�������� �������� ������ ���� � ���������� ������� �������������
	while ((chh = fgetc(fp)) != EOF)
	{
		for (int j = 0; j<256; j++)
		{
			if (chh == simbols[j].ch)
			{
				quan[j]++;
				counSym++;
				break;
			}
			if (simbols[j].ch == 0)
			{
				simbols[j].ch = (unsigned char)chh;
				quan[j] = 1;
				counUnSym++; counSym++;
				break;
			}
		}
	}

	// ������� ������� �������������
	for (int i = 0; i<counUnSym; i++)
		simbols[i].freq = (float)quan[i] / counSym;

	for (int i = 0; i<counUnSym; i++) //� ������ ���������� ������� ������ �������
		psym[i] = &simbols[i];

	//���������� �� �������� 
	sym tempp;
	for (int i = 1; i<counUnSym; i++)
		for (int j = 0; j<counUnSym - 1; j++)
			if (simbols[j].freq<simbols[j + 1].freq)
			{
				tempp = simbols[j];
				simbols[j] = simbols[j + 1];
				simbols[j + 1] = tempp;
			}

	for (int i = 0; i<counUnSym; i++)
	{
		sumFreMet += simbols[i].freq;
		printf("Ch= %d\tFreq= %f\tPPP= %c\t\n", simbols[i].ch, simbols[i].freq, psym[i]->ch, i);
	}
	printf("\n Words = %d\tSumFreMet=%f\n", counSym, sumFreMet);

	sym *root = makeTree(psym, counUnSym);//����� ������� �������� ������ �������

	makeCodes(root);//����� ������� ��������� ����

	rewind(fp);//���������� ��������� � ����� � ������ �����
			   //� ����� ������ �������� ����, � ���������� ���������� � �������� ���� � ������������� ����
	while ((chh = fgetc(fp)) != EOF)
	{
		for (int i = 0; i<counUnSym; i++)
			if (chh == simbols[i].ch)
				fputs(simbols[i].code, fp2);
	}
	fclose(fp2);

	//������ ��������� ���� � �������� �����, �� ������ ��� ������
	int i = 0;
	err = _wfopen_s(&fp3, L"temp.txt", L"rb");
	if (err) {
		MessageBoxW(NULL, L"Unable to create new file!", L"Error", 0);
		return;
	}
	//������� ������ ��������� �����(���������� �������� � ��)
	while ((chh = fgetc(fp2)) != EOF)
		fsize2++;

	remSize = fsize2 % 8;//������� �������, ���������� �������� �� ������� 8 (�����)

					//��������� ��������� ������� ����� ����� ���� ������
	fwrite("compresing!!!", sizeof(char), 24, fp3);//�������� �������
	fwrite(&counUnSym, sizeof(int), 1, fp3);//���������� ���������� ��������
	fwrite(&remSize, sizeof(int), 1, fp3);//�������� ������
									 //���������� � ������ ���� ������� �������������
	for (i = 0; i<counUnSym; i++)
	{
		fwrite(&simbols[i].ch, sizeof(sym), 1, fp3);
		fwrite(&simbols[i].freq, sizeof(sym), 1, fp3);
	}

	rewind(fp2);//���������� ��������� � ������������� ����� � ������ �����

	union code code1;		//�������������� ���������� code1
							//������ �������� ����, ������ ��������������� ������ 8 ��������� � ������ 
							//��� ����������� ��������� ��������� � �����������	union
	j = 0;
	for (int i = 0; i<fsize2 - remSize; i++)
	{
		mas[j] = fgetc(fp2);
		if (j == 7)
		{
			code1.byte.b1 = mas[0] - '0';
			code1.byte.b2 = mas[1] - '0';
			code1.byte.b3 = mas[2] - '0';
			code1.byte.b4 = mas[3] - '0';
			code1.byte.b5 = mas[4] - '0';
			code1.byte.b6 = mas[5] - '0';
			code1.byte.b7 = mas[6] - '0';
			code1.byte.b8 = mas[7] - '0';
			fputc(code1.chhh, fp3);
			j = 0;
		}
		j++;
	}
	//���������� �����
	j = 0;
	for (int i = 0; i <= remSize; i++)
	{
		mas[j] = fgetc(fp2);
		if (j == remSize)
		{
			code1.byte.b1 = mas[0] - '0';
			code1.byte.b2 = mas[1] - '0';
			code1.byte.b3 = mas[2] - '0';
			code1.byte.b4 = mas[3] - '0';
			code1.byte.b5 = mas[4] - '0';
			code1.byte.b6 = mas[5] - '0';
			code1.byte.b7 = mas[6] - '0';
			code1.byte.b8 = mas[7] - '0';
			fputc(code1.chhh, fp3);
		}
		j++;
	}

	_fcloseall();//��������� ��� �������� �����
	_getch();
	return;
}

sym * Archive::makeTree(sym * psym[], int k)
{
	sym *temp;
	temp = (sym*)malloc(sizeof(sym));
	temp->freq = psym[k - 1]->freq + psym[k - 2]->freq;
	temp->code[0] = 0;
	temp->left = psym[k - 1];
	temp->right = psym[k - 2];

	if (k == 2)
		return temp;
	else //�������� � ������ � ������ ����� �������� ������ �������
	{
		for (int i = 0; i<k; i++)
			if (temp->freq>psym[i]->freq)
			{
				for (int j = k - 1; j>i; j--)
					psym[j] = psym[j - 1];

				psym[i] = temp;
				break;
			}
	}
	return makeTree(psym, k - 1);
}

void Archive::makeCodes(sym * root)
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);
		strcat(root->left->code, "0");
		makeCodes(root->left);
	}
	if (root->right)
	{
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}
