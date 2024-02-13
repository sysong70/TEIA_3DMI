#include "stdafx.h"
#include "Fio.h"
#include "WStr.h"

#include <locale.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

namespace UnitTestFio
{
	using namespace Fio;

	void TestBinFile()
	{
		BinFile file;

		if (file.Open(L"c:\\temp\\test.bin", EMode::Write)) {
			double data = 1.23;

			file.Write((char*)&data, sizeof(double));
			file.WriteValue((long)456);
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\test.bin", EMode::Read)) {
			double data1;
			long data2;

			size_t read = file.Read((char*)&data1, sizeof(double));
			file.ReadValue(data2);

			ASSERT(std::abs(data1 - 1.23) < 0.000001);
			ASSERT(data2 == 456);
		}
		else {
			ASSERT(false);
		}
	}

	void TestTextFile()
	{
		CString data = L"English1\r\n한글1\n한글2\r\n";
		CString other = L"Append\n\r\n";
		TextFile file;

		if (file.Open(L"c:\\temp\\utf8bom.txt", EMode::Write, EEncoding::UTF8BOM)) {
			file.Write(data.GetBuffer(), data.GetLength());
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf8bom.txt", EMode::Read, EEncoding::UTF8BOM)) {
			CString result;
			size_t read = file.Read(result);

			ASSERT(data == result);
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf8bom.txt", EMode::Read, EEncoding::UTF8BOM)) {
			WStringArray sa;
			bool success = file.ReadLines(sa, false);

			ASSERT(sa.size() == 4);
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf16le.txt", EMode::Write, EEncoding::UTF16LE)) {
			file.Write(data.GetBuffer(), data.GetLength());
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf16le.txt", EMode::Read, EEncoding::UTF16LE)) {
			CString result;
			size_t read = file.Read(result);

			ASSERT(data == result);
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf16le.txt", EMode::Append, EEncoding::UTF16LE)) {
			file.Write(other.GetBuffer(), other.GetLength());
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf16le.txt", EMode::Read, EEncoding::UTF16LE)) {
			WStringArray sa;
			bool success = file.ReadLines(sa);

			ASSERT(sa.size() == 4);
		}
		else {
			ASSERT(false);
		}

		if (file.Open(L"c:\\temp\\utf16le.txt", EMode::Write, EEncoding::UTF16LE)) {
			WStringArray sa = { L"Data1", L"데이터2", L"Data3" };
			bool success = file.WriteLines(sa);

			ASSERT(success);
		}
		else {
			ASSERT(false);
		}
	}
}

//--------------------------------------------------------------------------------------------------

#pragma region BinFile Class

Fio::BinFile::BinFile()
	: m_pFile(nullptr)
{
}



Fio::BinFile::~BinFile()
{
	Close();
}



int Fio::BinFile::GetLength()
{
	if (m_pFile == nullptr) {
		return 0;
	}
	else {
		::fseek(m_pFile, 0, SEEK_END);
		int length = GetPosition();
		::fseek(m_pFile, 0, SEEK_SET);

		return length;
	}
}



int Fio::BinFile::GetPosition()
{
	return ::ftell(m_pFile);
}



void Fio::BinFile::Jump(int offset)
{
	if (m_pFile == nullptr) {
		return;
	}

	::fseek(m_pFile, offset, SEEK_CUR);
}



void Fio::BinFile::Rewind()
{
	::fseek(m_pFile, 0, SEEK_SET);
}



void Fio::BinFile::Close()
{
	if (m_pFile != nullptr) {
		::fclose(m_pFile);
		m_pFile = nullptr;
	}
}



bool Fio::BinFile::Open(const wchar_t* path, EMode mode)
{
	Close();

	//:WARNING - share mode
	if ((m_pFile = ::_wfsopen(path, GetMode(mode).GetBuffer(), _SH_DENYNO)) == nullptr) {
		RETURN_FALSE;
	}

	return true;
}



size_t Fio::BinFile::Read(const char* buffer, size_t length)
{
	if (m_pFile == nullptr) {
		return 0;
	}

	size_t read = ::fread((void*)buffer, sizeof(char), length, m_pFile);

	return read;
}



size_t Fio::BinFile::Write(const char* buffer, size_t length)
{
	if (m_pFile == nullptr) {
		return 0;
	}

	return ::fwrite((void*)buffer, sizeof(char), length, m_pFile);
}



CString Fio::BinFile::GetMode(EMode e)
{
	switch (e) {
	case EMode::Read:   return L"rb";
	case EMode::Write:  return L"wb";
	case EMode::Append: return L"ab";
	default:
		DEBUG_STOP;
		return L"rb";
	}
}

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

#pragma region TextFile Class

#define MAX_BUFFER	1024



Fio::TextFile::TextFile()
	: BinFile()
{}



Fio::TextFile::~TextFile()
{
}



bool Fio::TextFile::Open(const wchar_t* path, EMode mode, EEncoding encoding)
{
	Close();

	::_wsetlocale(LC_ALL, L"Korean");

	CString wmode = GetMode(mode) + GetEncodeing(encoding);
	if ((m_pFile = ::_wfsopen(path, wmode.GetBuffer(), _SH_DENYNO)) == nullptr) {
		RETURN_FALSE;
	}

	return true;
}



size_t Fio::TextFile::Read(CString& result)
{
	if (m_pFile == nullptr) {
		return 0;
	}

	wchar_t buffer[MAX_BUFFER + 1];
	size_t read = 0;
	size_t total = 0;

	while ((read = ::fread((void*)buffer, sizeof(wchar_t), MAX_BUFFER, m_pFile)) > 0) {
		buffer[read] = '\0';
		result += buffer;
		total += read;
	}

	return total;
}



bool Fio::TextFile::ReadLine(CString& line)
{
	if (m_pFile == nullptr) {
		return false;
	}

	wchar_t buffer[MAX_BUFFER + 1];

	if (::feof(m_pFile)) {
		return false;
	}

	if (::fgetws(buffer, MAX_BUFFER, m_pFile) != nullptr) {
		line += buffer;
	}

	line.TrimRight();

	return ::ferror(m_pFile) == 0;
}



bool Fio::TextFile::ReadLines(WStringArray& lines, bool skipBlank /*= true*/)
{
	CString line;
	while (ReadLine(line)) {
		if (!(skipBlank && line.IsEmpty())) {
			lines.push_back(line);
		}

		line.Empty();
	}

	return (line.GetLength() > 0);
}



size_t Fio::TextFile::Write(const wchar_t* buffer, size_t length)
{
	if (m_pFile == nullptr) {
		return 0;
	}

	return ::fwrite((void*)buffer, sizeof(wchar_t), length, m_pFile);
}



bool Fio::TextFile::WriteLine(const wchar_t* line, size_t length)
{
	size_t size = Write(line, length);
	size_t crlf = Write(L"\n", 1);

	return (size == length && crlf == 1);
}



bool Fio::TextFile::WriteLines(WStringArray& lines)
{
	for (auto& line : lines) {
		if (WriteLine(line.GetBuffer(), line.GetLength()) == false) {
			return false;
		}
	}

	return true;
}



CString Fio::TextFile::GetMode(EMode e)
{
	switch (e) {
	case EMode::Read:   return L"rt";
	case EMode::Write:  return L"wt";
	case EMode::Append: return L"at";
	default:
		DEBUG_STOP;
		return L"rt";
	}
}



CString Fio::TextFile::GetEncodeing(EEncoding e)
{
	switch (e) {
	case EEncoding::UTF8:    return L",ccs=UTF-8";
	case EEncoding::UTF16LE: return L",ccs=UTF-16LE";
	case EEncoding::UTF8BOM: return L",ccs=UTF-8";
	default:
		DEBUG_STOP;
		return L"";
	}
}

#pragma endregion //:REGION
