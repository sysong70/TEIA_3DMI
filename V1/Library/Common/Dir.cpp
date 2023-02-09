#include "stdafx.h"
#include "Dir.h"
#include "File.h"
#include "Path.h"

#include <shlobj_core.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



bool Dir::Create(const wchar_t* path)
{
	// recursive
	if (::SHCreateDirectory(NULL, path) == ERROR_SUCCESS) {
		return true;
	}
	else {
		DWORD err = ::GetLastError();
		switch (err) {
		case ERROR_BAD_PATHNAME:
		case ERROR_FILENAME_EXCED_RANGE:
			return false;

		case ERROR_FILE_EXISTS:
		case ERROR_ALREADY_EXISTS:
			return true;

		default:
			RETURN_TRUE;
		}
	}
}



bool Dir::Delete(const wchar_t* path, bool contentsOnly)
{
	if (IsExist(path) == false) {
		return false;
	}

	CString targetPath(path);
	Path::AddBackslash(targetPath);

	CString subPath;

	File::Finder finder;
	bool found = finder.FindFirst((targetPath + L"*").GetBuffer());
	while (found) {
		if (finder.IsDots()) {
		}
		else if (finder.IsDirectory()) {
			subPath = finder.GetFilePath();
			Delete(subPath.GetBuffer());
			::RemoveDirectory(subPath.GetBuffer());
		}
		else {
			subPath = finder.GetFilePath();
			File::RemoveReadOnlyAttribute(subPath.GetBuffer());
			::DeleteFile(subPath.GetBuffer());
		}

		found = finder.FindNext();
	}

	if (contentsOnly == false) {
		::RemoveDirectory(targetPath.GetBuffer());
	}

	return true;
}



bool Dir::IsExist(const wchar_t* path)
{
	DWORD attr = ::GetFileAttributes(path);
	if (attr == INVALID_FILE_ATTRIBUTES) {
		return false;
	}

	return (attr & FILE_ATTRIBUTE_DIRECTORY);
}



bool Dir::Move(const wchar_t* pSourcePath, const wchar_t* pTargetPath)
{
	CString sourcePath(pSourcePath), targetPath(pTargetPath);
	Path::AddBackslash(sourcePath);
	Path::AddBackslash(targetPath);

	if (IsExist(pSourcePath) == false) {
		RETURN_FALSE;
	}
	if (Create(targetPath.GetBuffer()) == false) {
		RETURN_FALSE;
	}

	CString subSource;
	CString subTarget;

	File::Finder finder;
	bool found = finder.FindFirst((sourcePath + L"*").GetBuffer());
	while (found) {
		if (finder.IsDots()) {
		}
		else if (finder.IsDirectory()) {
			subSource = finder.GetFilePath();
			subTarget = targetPath + finder.GetFileName();

			if (Move(subSource.GetBuffer(), subTarget.GetBuffer()) == false) {
				RETURN_FALSE;
			}
		}
		else {
			subSource = finder.GetFilePath();
			subTarget = targetPath + finder.GetFileName();

			if (File::Move(subSource.GetBuffer(), subTarget.GetBuffer()) == false) {
				RETURN_FALSE;
			}
		}

		found = finder.FindNext();
	}

	Delete(pSourcePath);

	return true;
}
