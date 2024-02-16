#include "stdafx.h"
#include "Path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

void Path::AddBackslash(CString& path)
{
	if (path.GetAt(path.GetLength() - 1) != '\\') {
		path += '\\';
	}
}



void Path::AddExtension(CString& path, CString extension)
{
	CString sourceExt = extension.MakeLower();

	if (extension.GetAt(0) != '.') {
		sourceExt = L"." + sourceExt;
	}

	if (path.GetLength() < sourceExt.GetLength()) {
		path += sourceExt;
		return;
	}

	CString targetExt = path.Right(sourceExt.GetLength()).MakeLower();

	if (targetExt != sourceExt) {
		path += sourceExt;
	}
}



void Path::Divide(const wchar_t* pPath, CString& dir, CString& fileName)
{
	CString path(pPath);
	int pos = path.ReverseFind('\\');

	if (pos == -1) {
		fileName = path;
	}
	else {
		dir = path.Mid(0, pos);
		fileName = path.Mid(pos + 1);
	}
}



void Path::Divide(const wchar_t* pPath, CString& dir, CString& name, CString& extension)
{
	CString fileName;
	Divide(pPath, dir, fileName);

	if (fileName.IsEmpty()) {
		return;
	}

	int pos = fileName.ReverseFind('.');
	if (pos == -1) {
		name = fileName;
	}
	else {
		name = fileName.Mid(0, pos);
		extension = fileName.Mid(pos + 1);
	}
}



CString Path::GetDirectory(const wchar_t* pPath, bool backslash /*= true*/)
{
	CString path(pPath);
	CString dir;
	int pos = path.ReverseFind(L'\\');

	if (pos == -1) {
	}
	else {
		dir = path.Mid(0, pos + (backslash ? 1 : 0));
	}

	return dir;
}



CString Path::GetExtension(const wchar_t* path)
{
	CString dir, file, extension;
	Divide(path, dir, file, extension);

	return extension;
}



CString Path::GetFileName(const wchar_t* path)
{
	CString dir, fileName;
	Divide(path, dir, fileName);

	return fileName;
}

CString Path::GetFileTitle(const wchar_t * path)
{
	CString dir, fileName;
	Divide(path, dir, fileName);

	TrimExtension(fileName);

	return fileName;
}

void Path::RemoveBackslash(CString& path)
{
	if (path.IsEmpty() == false && path.GetAt(path.GetLength() - 1) == L'\\') {
		path.Delete(path.GetLength() - 1);
	}
}



CString Path::TrimFileName(CString& path)
{
	CString fileName;

	int pos = path.ReverseFind('\\');
	if (pos == -1) {
		fileName = path;
		path.Empty();
	}
	else {
		fileName = path.Mid(pos + 1);
		path = path.Mid(pos + 1);
	}

	return fileName;
}



CString Path::TrimExtension(CString& path)
{
	CString extension;

	int pos = path.ReverseFind('\\');
	int epos = path.ReverseFind('.');

	if (pos == -1) {
		if (epos == -1) {
		}
		else {
			extension = path.Mid(epos + 1);
			path.Delete(epos, path.GetLength());
		}
	}
	else {
		if (epos == -1) {
		}
		else if (epos > pos) {
			extension = path.Mid(epos + 1);
			path.Delete(epos, path.GetLength());
		}
	}

	return extension;
}
