#pragma once

#include "Common_Define.h"



namespace Path
{
	void AddBackslash(CString& path);

	/*
		CString source = L"c:\\temp\\test";

		AddExtension(source, L"log");
		ASSERT(source == L"c:\\temp\\test.log");
	*/
	void AddExtension(CString& path, CString extension);

	/*
		CString dir, fileName;

		Divide(L"c:\\temp\\source.txt", dir, fileName);
		ASSERT(dir == L"c:\\temp" && fileName == L"source.txt");
	*/
	void Divide(const wchar_t* path, CString& dir, CString& fileName);

	/*
		CString path = L"c:\\temp\\source.txt";
		CString dir, file, extension;

		Divide(path, dir, file, extension);
		ASSERT(dir == L"c:\\temp" && file == L"source" && extension == L"txt");
	*/
	void Divide(const wchar_t* path, CString& dir, CString& name, CString& extension);

	/*
		ASSERT(GetDirectory(L"c:\\temp\\log.txt") == L"c:\\temp\\");
	*/
	CString GetDirectory(const wchar_t* path, bool backslash = true);

	/*
		ASSERT(GetExtension(L"c:\\temp\\log.txt") == L"txt");
	*/
	CString GetExtension(const wchar_t* path);

	/*
		ASSERT(GetFileName(L"c:\\temp\\log.txt") == L"log.txt");
	*/
	CString GetFileName(const wchar_t* path);

	/*
		ASSERT(GetFileName(L"c:\\temp\\log.txt") == L"log");
	*/
	CString GetFileTitle(const wchar_t * path);

	/*
		CString path = L"c:\\temp\\";

		RemoveBackslash(path);
		ASSERT(path == L"c:\\temp");
	*/
	void RemoveBackslash(CString& path);

	/*
		CString path = L"c:\\temp\\source.txt";
		CString fileName = TrimFileName(path);
		ASSERT(path == L"c:\\temp\\" && fileName == L"source.txt");

		path = L"source.txt";
		fileName = TrimFileName(path);
		ASSERT(path == L"" && fileName == L"source.txt");
	*/
	CString TrimFileName(CString& path);

	/*
		CString path = L"c:\\temp\\source.txt";
		CString extension = TrimExtension(path);
		ASSERT(path == L"c:\\temp\\source" && extension == L"txt");

		path = L"source.txt";
		extension = TrimExtension(path);
		ASSERT(path == L"source" && extension == L"txt");
	*/
	CString TrimExtension(CString& path);
}
