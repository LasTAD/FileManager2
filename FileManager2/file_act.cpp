#include "file_act.h"
using namespace std;


void FileCopy::StartCopy(wstring &srcPath, bool isDir) 
{
	wstring resPath;
	wcout << "Input path to paste: ";
	wcin >> resPath;
	if (!isDir) {
		_CopyFile(srcPath, resPath);
	}
	else
		_Copy(srcPath, resPath);
}

void FileCopy::_CopyFile(wstring &src, wstring &res)
{
	errno_t err;
	FILE *fileSrc, *fileRes;
	err = _wfopen_s(&fileSrc, src.c_str(), L"r+b");
	if (err) {
		MessageBox(NULL, L"Unable to open requested file!", L"Error", 0);
		return;
	}
	err = _wfopen_s(&fileRes, res.c_str(), L"w+b");
	if (err) {
		MessageBox(NULL, L"Unable to create new file!", L"Error", 0);
		return;
	}
	unsigned int fsize, n;
	fseek(fileSrc, 0, SEEK_END);
	fsize = ftell(fileSrc);
	rewind(fileSrc);
	char *buffer;
	n = fsize / 65536;
	for (int i = 0; i < n; ++i)
	{
		buffer = new char[65536];
		fread(buffer, 65536, sizeof(char), fileSrc);
		fwrite(buffer, 65536, sizeof(char), fileRes);
		delete[] buffer;
	}
	int rem = fsize - n * 65536;
	buffer = new char[rem];
	fread(buffer, rem, sizeof(char), fileSrc);
	fwrite(buffer, rem, sizeof(char), fileRes);
	delete[] buffer;
	_fcloseall();
}

void FileCopy::CreateDir(wstring &path)
{
	if (!CreateDirectoryW(path.c_str(),NULL)) {
		MessageBox(NULL, L"Unable to create directory!", L"Error", 0);
	}
}

void FileCopy::_Copy(wstring &src, wstring &res)
{
	for (int i = fileEx.first; i <= fileEx.last; i++) {
		if (fileEx.fileList[i].isDir) {
			CreateDir(res += fileEx.fileList[i].name);
			_Copy(src+= fileEx.fileList[i].name, res += fileEx.fileList[i].name);
		}
		else {
			_CopyFile(src += fileEx.fileList[i].name, res += fileEx.fileList[i].name);
		}
	}
}


void FileDel::DelFile(wstring &fName)
{
	if (!DeleteFileW(fName.c_str()))
	{
		MessageBox(NULL, L"Unable to delete file!", L"Error", 0);
	}
}

void FileDel::ChName(wstring &oldName)
{
	wstring newName;
	wcout << "Input new name: ";
	wcin >> newName;
	if(_wrename(oldName.c_str(), newName.c_str()))
		MessageBox(NULL, L"Unable to rename file!", L"Error", 0);
}
