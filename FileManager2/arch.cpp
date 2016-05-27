#include "arch.h"

bool Coder::Encode(wstring  inputFilename, wstring  outputFilename) {
	map<char, int> freqs; 
	int i;

	FILE *inputFile;
	errno_t err;
	err=_wfopen_s(&inputFile, inputFilename.c_str(), L"r");
	if (err)
		return false;
	assert(inputFile);

	char ch; // char
	unsigned total = 0;
	//while (fscanf(inputFile, "%c", &ch) != EOF) {
	while (!feof(inputFile) && fread(&ch, 1, 1, inputFile) != 0) {
		
		freqs[ch]++;
		total++;
	}
	tsize = (int)freqs.size();

	ptable = new pnode[tsize];
	assert(ptable);
	float ftot = float(total);
	map<char, int>::iterator fi;
	for (fi = freqs.begin(), i = 0; fi != freqs.end(); ++fi, ++i)
	{
		ptable[i].ch = (*fi).first;
		ptable[i].p = float((*fi).second) / ftot;
	}
	qsort(ptable, tsize, sizeof(pnode), pnode_compare);

	EncHuffman();

	FILE *outputFile;
	err=_wfopen_s(&outputFile, outputFilename.c_str(), L"wb");
	if (err)
		return false;
	
	//
	fwrite(&tsize, 2, 1, outputFile);
	for (i = 0; i < tsize; i++) {
		fwrite(&ptable[i].ch, 1, 1, outputFile);
		int length = codes[ptable[i].ch].length();
		fwrite(&length, 2, 1, outputFile);
		fwrite(codes[ptable[i].ch].c_str(), 1, length, outputFile);
	}
	//

	/*fprintf(outputFile, "%i" NL, tsize);
	for (i = 0; i<tsize; i++)
	{
		fprintf(outputFile, "%c\t%f\t%s" NL, ptable[i].ch, ptable[i].p, codes[ptable[i].ch].c_str());
	}*/



	fseek(inputFile, SEEK_SET, 0);
	// fprintf(outputFile, NL);

	int oldpos = ftell(outputFile);
	unsigned long long bitscount = 0;
	int bitswait = 8;
	char byte;
	fwrite(&bitscount, 8, 1, outputFile);
	string text;
	while (fscanf(inputFile, "%c", &ch) != EOF) {
	//while (!feof(inputFile) && fread(&ch, 1, 1, inputFile) != 0) {
		string &d = codes[ch];
		int deb = text.find("������ �� ��") != string::npos;
		text += ch;
		for (int i = 0; i < d.length(); ++i) {
			if (bitswait == 0) {
				bitswait = 8;
				fwrite(&byte, 1, 1, outputFile);
			}
			if (d[i] == '1') {
				byte |= 1 << --bitswait;
			}
			else {
				byte &= ~(1 << --bitswait);
			}
			bitscount++;
		}
	}
	if (bitswait != 8) {
		fwrite(&byte, 1, 1, outputFile);
	}
	fseek(outputFile, oldpos, SEEK_SET);
	fwrite(&bitscount, 8, 1, outputFile);

	codes.clear();
	delete[] ptable;
	_fcloseall();
	return true;
}

bool Coder::Decode(wstring  inputFilename, wstring  outputFilename) {
	FILE *inputFile;
	errno_t err;
	err=_wfopen_s(&inputFile, inputFilename.c_str(), L"rb");
	if (err)
		return false;

	//fscanf(inputFile, "%i", &tsize);
	fread(&tsize, 2, 1, inputFile);
	char ch;
	char code[128];
	float p;
	int i;
	//fgetc(inputFile); 
	for (i = 0; i<tsize; i++)
	{
		/*
		ch = fgetc(inputFile);
		fscanf(inputFile, "%f %s", &p, code);
		codes[ch] = code;
		fgetc(inputFile); */
		fread(&ch, 1, 1, inputFile);
		short length;
		fread(&length, 2, 1, inputFile);
		fread(code, 1, length, inputFile);
		code[length] = 0;
		codes[ch] = code;
	}
	//fgetc(inputFile); 

	FILE *outputFile;
	err=_wfopen_s(&outputFile, outputFilename.c_str(), L"w");
	if (err)
		return false;

	string accum = "";
	unsigned long long bitcount;
	fread(&bitcount, 8, 1, inputFile);
	while (fscanf(inputFile, "%c", &ch) != EOF && bitcount != 0) {
		for (int i = 7; i > -1; --i) {
			bool bit = (ch >> i) & 1;

			if (bit) accum += '1'; else accum += '0';

			for (auto it = codes.begin(); it != codes.end(); ++it) {
				if ((*it).second == accum) {
					fprintf(outputFile, "%c", (*it).first);
					accum = "";
					break;
				}
			}
			--bitcount;
		}
	}

	_fcloseall();
	return true;
}

void Coder::EncHuffman() {
	treenode *n;
	vector<treenode*> tops; 
	int i, numtop = tsize;
	for (i = 0; i<numtop; i++) {
		n = new treenode;
		assert(n);
		n->ch = ptable[i].ch;
		n->p = ptable[i].p;
		n->left = NULL;
		n->right = NULL;
		tops.push_back(n);
	}

	while (numtop > 1)
	{
		n = new treenode;
		assert(n);
		n->p = tops[numtop - 2]->p + tops[numtop - 1]->p;
		n->left = tops[numtop - 2];
		n->right = tops[numtop - 1];
		if (n->left->p < n->right->p)
		{
			n->lcode = '0';
			n->rcode = '1';
		}
		else
		{
			n->lcode = '1';
			n->rcode = '0';
		}
		tops.pop_back();
		tops.pop_back();
		bool isins = false;
		std::vector<treenode*>::iterator ti;
		for (ti = tops.begin(); ti != tops.end(); ++ti)
			if ((*ti)->p < n->p)
			{
				tops.insert(ti, n);
				isins = true;
				break;
			}
		if (!isins) tops.push_back(n);
		numtop--;
	}

	treenode *root = tops[0];
	GenerateCode(root);

	DestroyNode(root);
	tops.clear();
}

void Coder::GenerateCode(treenode * node)
{
	static string sequence = "";
	if (node->left)
	{
		sequence += node->lcode;
		GenerateCode(node->left);
	}

	if (node->right)
	{
		sequence += node->rcode;
		GenerateCode(node->right);
	}
	if (tsize == 1)
		codes[node->ch] = sequence = "0";
	if (!node->left && !node->right)
		codes[node->ch] = sequence;

	int l = (int)sequence.length();
	if (l > 1) sequence = sequence.substr(0, l - 1);
	else sequence = "";
}

void Coder::DestroyNode(treenode * node)
{
	if (node->left)
	{
		DestroyNode(node->left);
		delete node->left;
		node->left = NULL;
	}

	if (node->right)
	{
		DestroyNode(node->right);
		delete node->right;
		node->right = NULL;
	}
}

