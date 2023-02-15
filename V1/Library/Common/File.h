#pragma once
#pragma warning(disable: 4251)

#include "Common_Define.h"



namespace File
{
	class Finder
	{
	public:

		Finder();

		~Finder();

		bool FindFirst(const wchar_t* path);

		bool FindNext();

		bool IsDirectory();

		bool IsDots();

		CString GetFileName();

		CString GetFilePath();

		void Close();

	protected:

		// target path (include pattern)
		CString m_targetDir;

		HANDLE m_hFind;

		WIN32_FIND_DATA m_data;
	};

	/*
		WStringArray files = { L"test1.log", L"test2.log", L"test3.log" };
		CString path = L"c:\\temp\\";
		CString filter = L"txt;log";

		for (auto& fileName : files) {
			ASSERT(Create((path + fileName)));
		}

		files.clear();
		ASSERT(Gather(path.GetBuffer(), filter.GetBuffer(), files));
		ASSERT(Move(L"c:\\temp\\test1.log", L"c:\\temp\\test4.log"));
		ASSERT(IsExist(path + L"test4.log"));
	*/

	bool Create(const wchar_t* path);

	bool Gather(const wchar_t* path, const wchar_t* filter, WStringArray& files);

	unsigned long GetFileSize(const wchar_t* path);

	bool IsExist(const wchar_t* path);

	bool Move(const wchar_t* sourcePath, const wchar_t* targetPath);

	bool RemoveReadOnlyAttribute(const wchar_t* path);
}
