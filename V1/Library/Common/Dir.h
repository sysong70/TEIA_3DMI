#pragma once
#pragma warning(disable: 4251)

#include "Common_Define.h"



namespace Dir
{
	/*
		Delete(L"c:\\temp\\sub1");
		Delete(L"c:\\temp\\sub2");

		ASSERT(IsExist(L"c:\\temp\\sub1") == false && Create(L"c:\\temp\\sub1\\sub2\\sub3"));
		ASSERT(File::Create(L"c:\\temp\\sub1\\sub2\\test.txt"));
		ASSERT(Move(L"c:\\temp\\sub1\\sub2", L"c:\\temp\\sub2"));
	*/

	bool Create(const wchar_t* path);

	bool Delete(const wchar_t* path, bool contentsOnly = false);

	bool IsExist(const wchar_t* path);

	bool Move(const wchar_t* sourcePath, const wchar_t* targetPath);
}
