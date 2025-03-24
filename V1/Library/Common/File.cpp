#include "stdafx.h"

#include "File.h"
#include "Dir.h"
#include "Path.h"
#include "WStr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region Finder Class

File::Finder::Finder()
{
	m_hFind = INVALID_HANDLE_VALUE;
	m_data = {};
}



File::Finder::~Finder()
{
	Close();
}



bool File::Finder::FindFirst(const wchar_t* path)
{
	m_targetDir = Path::GetDirectory(path);
	m_hFind = ::FindFirstFile(path, &m_data);
	return (m_hFind == INVALID_HANDLE_VALUE) == false;
}



bool File::Finder::FindNext()
{
	if (m_hFind == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		return ::FindNextFile(m_hFind, &m_data);
	}
}



bool File::Finder::IsDirectory()
{
	return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}



bool File::Finder::IsDots()
{
	return (wcscmp(m_data.cFileName, L".") == 0 || wcscmp(m_data.cFileName, L"..") == 0);
}



CString File::Finder::GetFileName()
{
	return (m_hFind == INVALID_HANDLE_VALUE ? L"" : m_data.cFileName);
}



CString File::Finder::GetFilePath()
{
	return m_targetDir + (m_hFind == INVALID_HANDLE_VALUE ? L"" : m_data.cFileName);
}



void File::Finder::Close()
{
	::FindClose(m_hFind);
	m_hFind = INVALID_HANDLE_VALUE;
}

#pragma endregion // REGION

//**************************************************************************************************

bool File::Create(const wchar_t* path)
{
	HANDLE hFile = ::CreateFile(path, GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return false;
	}
	else {
		::CloseHandle(hFile);
		return true;
	}
}



bool File::Gather(const wchar_t* path, const wchar_t* filter, WStringArray& files)
{
	WStringArray extArray;
	WStr::Split(filter, ';', extArray);
	for (auto& ext : extArray) {
		ext.MakeUpper();
	}
	
	CString targetPath(path);
	Path::AddBackslash(targetPath);
	targetPath += L"*.*";

	Finder finder;
	bool found = finder.FindFirst(targetPath.GetBuffer());
	while (found) {
		if (finder.IsDots()) {
		}
		else if (finder.IsDirectory()) {
		}
		else {
			CString fileName = finder.GetFileName();
			CString ext = Path::GetExtension(fileName.GetBuffer()).MakeUpper();

			for (const auto& e : extArray) {
				if (ext == e) {
					files.push_back(fileName);
				}
			}
		}

		found = finder.FindNext();
	}

	return (files.size() > 0);
}



unsigned long File::GetFileSize(const wchar_t* path)
{
	WIN32_FILE_ATTRIBUTE_DATA fad;
	if (!::GetFileAttributesEx(path, GetFileExInfoStandard, &fad)) {
		// error condition, could call GetLastError to find out more
		return -1;
	}

	LARGE_INTEGER size;
	size.HighPart = fad.nFileSizeHigh;
	size.LowPart = fad.nFileSizeLow;

	return (unsigned long)size.QuadPart;
}



bool File::IsExist(const wchar_t* path)
{
	WIN32_FIND_DATA data;
	return (::FindFirstFile(path, &data) == INVALID_HANDLE_VALUE ? false : true);
}



bool File::Move(const wchar_t* sourcePath, const wchar_t* targetPath)
{
	if (!IsExist(sourcePath)) {
		return false;
	}

	CString dir, fileName;
	Path::Divide(targetPath, dir, fileName);
	if (!Dir::Create(dir.GetBuffer())) {
		return false;
	}

	if (::CopyFile(sourcePath, targetPath, FALSE)) {
		return (bool)::DeleteFile(sourcePath);
	}
	else {
		RETURN_FALSE;
	}
}



bool File::RemoveReadOnlyAttribute(const wchar_t* path)
{
	DWORD attr = ::GetFileAttributes(path);
	if (attr == INVALID_FILE_ATTRIBUTES) {
		return false;
	}

	if (attr & FILE_ATTRIBUTE_READONLY) {
		attr &= ~FILE_ATTRIBUTE_READONLY;
		return (bool)::SetFileAttributes(path, attr);
	}

	return true;
}
