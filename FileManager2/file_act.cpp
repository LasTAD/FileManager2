
#include "file_act.h"
#include <string>
#include <Windows.h>
#include "gui.h"

using namespace std;

// !!! ВНИМАНИЕ
// TODO для совместимости я сделаю так: isDir = 1 если директория, 0 если файл
void FileCopy::StartCopy(wstring &srcPath, int isDir) 
{
	// TODO я сделаю проверку в консоли
	/*
	if (isDir == SYS) {
		ErrorMessage(L"Unable to copy this file!");
		return;
	}*/
	wstring resPath;
	wcout << "Input path to paste: ";
	wcin >> resPath;
	if (isDir==1) // TODO
		_CopyFile(srcPath, resPath);
	else if (isDir==1) { // TODO
		size_t pos = srcPath.find_last_of(L'\\', srcPath.length());
		wstring res1;
		res1 = srcPath.substr(pos + 1, srcPath.length() - pos - 1);
		CreateDir(resPath + L'\\' + res1);
		_Copy(srcPath, resPath + L'\\' + res1);
	}
}

void FileCopy::_CopyFile(wstring &src, wstring &res)
{
	errno_t err;
	FILE *fileSrc, *fileRes;
	err = _wfopen_s(&fileSrc, src.c_str(), L"rb");
	if (err) {
		// TODO
		//ErrorMessage(L"Unable to open requested file!");
		return;
	}
	err = _wfopen_s(&fileRes, res.c_str(), L"wb");
	if (err) {
		// TODO
		// ErrorMessage(L"Unable to create new file!");
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
	if (!CreateDirectoryW(path.c_str(),NULL))
		// TODO
		//ErrorMessage(L"Unable to create directory!");
}

void FileCopy::_Copy(wstring src, wstring res)
{
	vector<PWIN32_FIND_DATAW> dirFiles;
	
	GetFileList(src + L'\\',dirFiles);
	for (int i = 0; i < dirFiles.size(); i++) {
		if ((bool)(dirFiles[i]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			CreateDir(res + L'\\' + dirFiles[i]->cFileName);
			_Copy(src + L'\\' + dirFiles[i]->cFileName, res + L'\\' + dirFiles[i]->cFileName);
		}
		else
			_CopyFile(src + L'\\' + dirFiles[i]->cFileName, res + L'\\' + dirFiles[i]->cFileName);
		delete dirFiles[i];
	}
}


void FileDel::StartDel(wstring & path, int isDir)
{
	if (isDir == SYS) {
		ErrorMessage(L"Unable to delete this file!");
		return;
	}
	if (isDir==FIL)
		DelFile(path);
	else if(isDir==DIR)
		_Del(path);
}

void FileDel::ChName(wstring &oldName, int isDir)
{
	if (isDir == SYS) {
		ErrorMessage(L"Unable to change name of this file!");
		return;
	}
	wstring newName, path;
	wcout << "Input new name: ";
	wcin >> newName;
	size_t pos = oldName.find_last_of(L'\\', oldName.length());
	path = oldName.substr(0, pos+1);
	if(_wrename(oldName.c_str(), (path + newName).c_str()))
		ErrorMessage(L"Unable to rename file or directory!");
}

void FileDel::DelFile(wstring &fName)
{
	if (!DeleteFileW(fName.c_str()))
		ErrorMessage(L"Unable to delete file!");
}

void FileDel::DelDir(wstring &fName)
{
	if (!RemoveDirectoryW(fName.c_str()))
		ErrorMessage(L"Unable to remove directory!");
}

void FileDel::_Del(wstring src)
{
	vector<PWIN32_FIND_DATAW> dirFiles;
	GetFileList(src + L'\\', dirFiles);
	for (int i = 0; i < dirFiles.size(); i++) {
		if (!(bool)(dirFiles[i]->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			DelFile(src + L'\\' + dirFiles[i]->cFileName);
		}
		else {
			_Del(src + L'\\' + dirFiles[i]->cFileName);
			DelDir(src + L'\\' + dirFiles[i]->cFileName);
		}
		delete dirFiles[i];
	}
	DelDir(src);
}
