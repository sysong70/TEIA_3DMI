#pragma once

#include "Common_Define.h"

//--------------------------------------------------------------------------------------------------

namespace Fio
{
	enum class EMode
	{
		Read,
		Write,
		Append,
	};

	enum class EEncoding
	{

		UTF8 = 0,    // Same as ANSI
		UTF16LE = 2, // BOM - 0xFFFE
		UTF8BOM = 3, // BOM - 0xEFBBBF
	};

	/*
		fio.cpp
		BinFile_UnitTest()
	*/

	class  BinFile
	{
	public:

		BinFile();

		virtual ~BinFile();

		virtual int GetLength();

		virtual int GetPosition();

		virtual void Jump(int offset);

		virtual void Rewind();

	public:

		void Close();

		template <class T> bool ReadValue(T& value)
		{
			char buffer[128];
			if (Read(buffer, sizeof(T)) == sizeof(T)) {
				value = *(T*)(buffer);
				return true;
			}

			return false;
		}

		template <class T> bool WriteValue(T value)
		{
			return Write((const char*)(&value), sizeof(T)) == sizeof(T);
		}

		bool Open(const wchar_t* path, EMode mode);

		size_t Read(const char* buffer, size_t length);

		size_t Write(const char* buffer, size_t length);

	protected:

		virtual CString GetMode(EMode e);

		FILE* m_pFile;
	};

	/*
		fio.cpp
		TextFile_UnitTest()
	*/

	class  TextFile : public BinFile
	{
	public:

		TextFile();

		~TextFile() override;

		int GetLength() override {
			DEBUG_STOP; return -1;
		}

		int GetPosition() override {
			DEBUG_STOP; return -1;
		}

		void Jump(int offset) override {
			DEBUG_STOP;
		}

		void Rewind() override {
			DEBUG_STOP;
		}

	public:

		bool Open(const wchar_t* path, EMode mode, EEncoding encoding);

		size_t Read(CString& result);

		bool ReadLine(CString& line);

		bool ReadLines(WStringArray& lines, bool skipBlank = true);

		size_t Write(const wchar_t* buffer, size_t length);

		bool WriteLine(const wchar_t* line, size_t length);

		bool WriteLines(WStringArray& lines);

	protected:

		CString GetMode(EMode e) override;

		CString GetEncodeing(EEncoding e);
	};
}

