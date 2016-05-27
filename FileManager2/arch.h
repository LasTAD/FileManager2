#ifdef _WIN32
#define _CRT_SECURE_NO_DEPRECATE
#define NL "\r\n"
#else
#define NL "\n"
#endif

#include <string>
#include <vector>
#include <map>
#include <cassert>
#include <bitset>

using namespace std;

struct pnode
{
	char ch; // char
	float p; // probability
};

static int pnode_compare(const void *elem1, const void *elem2)
{
	const pnode a = *(pnode*)elem1;
	const pnode b = *(pnode*)elem2;
	if (a.p < b.p) return 1; // 1 - less (reverse for decreasing sort)
	else if (a.p > b.p) return -1;
	return 0;
}

struct treenode : public pnode
{
	char lcode;
	char rcode;
	treenode *left; 
	treenode *right;
};

class Coder
{
private:
	int tsize; 
	pnode *ptable; 
	map<char, string> codes; 
public:
	bool Encode(wstring inputFilename, wstring outputFilename);
	bool Decode(wstring inputFilename, wstring outputFilename);
private:
	void EncHuffman();
	void GenerateCode(treenode *node); 
	void DestroyNode(treenode *node); 
};