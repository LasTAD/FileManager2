#include "arch.h"

bool Coder::Encode(wstring  inputFilename, wstring  outputFilename)
{
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
	while (fscanf(inputFile, "%c", &ch) != EOF)
	{
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
	assert(outputFile);

	fprintf(outputFile, "%i" NL, tsize);
	for (i = 0; i<tsize; i++)
	{
		fprintf(outputFile, "%c\t%f\t%s" NL, ptable[i].ch, ptable[i].p, codes[ptable[i].ch].c_str());
	}

	fseek(inputFile, SEEK_SET, 0);
	fprintf(outputFile, NL);
	while (fscanf(inputFile, "%c", &ch) != EOF)
	{
		fprintf(outputFile, "%s", codes[ch].c_str());
	}

	codes.clear();
	delete[] ptable;
	_fcloseall();
	return true;
}

bool Coder::Decode(wstring  inputFilename, wstring  outputFilename)
{
	FILE *inputFile;
	errno_t err;
	err=_wfopen_s(&inputFile, inputFilename.c_str(), L"r");
	if (err)
		return false;
	assert(inputFile);

	fscanf(inputFile, "%i", &tsize);
	char ch, code[128];
	float p;
	int i;
	fgetc(inputFile); 
	for (i = 0; i<tsize; i++)
	{
		ch = fgetc(inputFile);
		fscanf(inputFile, "%f %s", &p, code);
		codes[ch] = code;
		fgetc(inputFile); 
	}
	fgetc(inputFile); 

	FILE *outputFile;
	err=_wfopen_s(&outputFile, outputFilename.c_str(), L"w");
	if (err)
		return false;
	assert(outputFile);

	string accum = "";
	map<char, string>::iterator ci;
	while ((ch = fgetc(inputFile)) != EOF)
	{
		accum += ch;
		for (ci = codes.begin(); ci != codes.end(); ++ci)
			if (!strcmp((*ci).second.c_str(), accum.c_str()))
			{
				accum = "";
				fprintf(outputFile, "%c", (*ci).first);
			}
	}
	_fcloseall();
	return true;
}

void Coder::EncHuffman()
{
	treenode *n;
	vector<treenode*> tops; 
	int i, numtop = tsize;
	for (i = 0; i<numtop; i++)
	{
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

