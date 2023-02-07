#pragma once

#include "Common_Define.h"



namespace WStr
{
	enum class EWrapper
	{
		Space,			// ' '
		SingleQuot,		// ''
		DoubleQuot,		// ""
		RoundBracket,	// ()
		CurlyBracket,	// {}
		SquareBracket,	// []
	};

#pragma region Stream Functions

	/*
		CString source = L"   \t{data}";
		const wchar_t* stream = source.GetBuffer();

		int offset = GetOffsetAfterBlank(stream);
		ASSERT(offset == 4);
	*/
	int GetOffsetAfterBlank(const wchar_t* pStream);

	/*
		CString source = L"object: \t{}";
		wchar_t* stream = (wchar_t*)source.GetBuffer();
		stream += wcslen(L"object:");

		bool found = IsCharAfterBlank(stream, '{');
		ASSERT(found && stream[0] == '}');
	*/
	bool IsCharAfterBlank(wchar_t*& stream, wchar_t ch, bool peek = false);

	/*
		CString source = L"data: 10.2e-2,";
		CString target;
		wchar_t* stream = (wchar_t*)source.GetBuffer();

		for (int i = 0; i < source.GetLength(); i++) {
			if (IsNumeric(*stream)) {
				target += stream[0];
			}
			stream++;
		}
		ASSERT(target == L"10.2e-2");
	*/
	bool IsDigit(CString& value);

	bool IsDigit(wchar_t value);

	bool IsNumeric(wchar_t value);

	/*
		CString source = L" \t\r\nData";
		wchar_t* stream = (wchar_t*)source.GetBuffer();

		ShiftBlank(stream);
		ASSERT(CString(stream) == L"Data");
	*/
	void ShiftBlank(wchar_t*& stream);

	/*
		CString source = L"01234-ABC";
		wchar_t* stream = (wchar_t*)source.GetBuffer();

		bool success = ShiftTo(stream, '-');
		ASSERT(success && CString(stream) == L"ABC");
	*/
	bool ShiftTo(wchar_t*& stream, wchar_t ch);

	/*
		CString source = L"01234-ABC-56789";
		WStringArray sa;

		ASSERT(Split(source.GetBuffer(), '-', sa) && sa[0] == L"01234" && sa[1] == L"ABC" && sa[2] == L"56789");
	*/
	bool Split(const wchar_t* source, wchar_t token, WStringArray& atomArray);

#pragma endregion //:REGION

#pragma region Startdard Functions

	void Add(CString& source, wchar_t ch, int count);

	/*
		CString source = L"01234-ABC-56789|DEF|01234";

		ASSERT(Erase(source, 0, '-', '-', true) && source == L"0123456789|DEF|01234");
		ASSERT(Erase(source, 0, '|', true) && source == L"DEF|01234");
	*/
	bool Erase(CString& source, int start, wchar_t toCh, bool includeCh);

	bool Erase(CString& source, int start, wchar_t fromCh, wchar_t toCh, bool includeCh);

	/*
	*
		ASSERT(Format(L"%d %.1f %s", 1, 2.0, L"3") == L"1 2.0 3");
	*/
	CString Format(const wchar_t* format, ...);

	/*
		CString source = L"01234-ABC-56789|DEF|01234";

		ASSERT(Get(source, 0, '-', '-', true, true) == L"-ABC-" && source == L"0123456789|DEF|01234");
		ASSERT(Get(source, 0, '|', true, true) == L"0123456789|" && source == L"DEF|01234");
		ASSERT(Get(source, 3, source.GetLength(), true) == L"|01234" && source == L"DEF");
	*/
	CString Get(CString& source, int start, int count, bool erase);

	CString Get(CString& source, int start, wchar_t toCh, bool includeCh, bool erase);

	CString Get(CString& source, int start, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase);

#pragma endregion //:REGION

#pragma region Extended Functions

	/*
		CString source = L"01234-ABC-56789|DEF|01234";

		ASSERT(Back(source, '-', '-', true, true) == L"-ABC-" && source == L"0123456789|DEF|01234");
		ASSERT(Back(source, '|', true, true) == L"|01234" && source == L"0123456789|DEF");
		ASSERT(Back(source, 3, true) == L"DEF" && source == L"0123456789|");
	*/
	CString Back(CString& source, int count, bool erase);

	CString Back(CString& source, wchar_t toCh, bool includeCh, bool erase);

	CString Back(CString& source, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase);

	/*
		CString source = L"01234-ABC-56789|DEF|01234";

		ASSERT(Front(source, '-', '-', true, true) == L"-ABC-" && source == L"0123456789|DEF|01234");
		ASSERT(Front(source, '|', true, true) == L"0123456789|" && source == L"DEF|01234");
		ASSERT(Front(source, 3, true) == L"DEF" && source == L"|01234");
	*/
	CString Front(CString& source, int count, bool erase);

	CString Front(CString& source, wchar_t toCh, bool includeCh, bool erase);

	CString Front(CString& source, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase);

	/*
		CString source = L"0123456789-ABC";

		RemoveFrom(source, '-');
		ASSERT(source == L"0123456789");
	*/
	void RemoveFrom(CString& source, wchar_t ch);

	/*
		CString source = L"0123456789-ABC";

		RemoveTo(source, '-');
		ASSERT(source == L"ABC");
	*/
	void RemoveTo(CString& source, wchar_t ch);

#pragma endregion //:REGION

#pragma region Cast Functions

	/*
		double result1 = ToDouble(L"  3.141592 * A");
		double result2 = ToDouble(L"  314.1592e-2 * A");

		ASSERT(std::abs(result1 - 3.141592) < 0.0000001);
		ASSERT(std::abs(result1 - result2) < 0.0000001);
	*/
	double ToDouble(const wchar_t* value);

	/*
		int result1 = ToInteger(L"  0x0012 * A");
		long result2 = ToLong(L"  18 * A");

		ASSERT(result1 == result2 && result1 == 18);
	*/
	int ToInteger(const wchar_t* value);

	long ToLong(const wchar_t* value);

	/*
		ASSERT(ToString(LONG_MAX) == L"2147483647");
		ASSERT(ToString(3.141592, 2) == L"3.14");
	*/
	CString ToString(bool value);

	CString ToString(int value);

	CString ToString(long value);

	CString ToString(double value);

	CString ToString(double value, int digit);

	CString ToString(DWORD_PTR value);

	/*
		CString source = L"한E";

		CStringA result1 = ToUtf8(source.GetBuffer());
		unsigned char* s = (unsigned char*)result1.GetBuffer();
		ASSERT(s[0] == 0xed && s[1] == 0x95 && s[2] == 0x9c && s[3] == 0x45);

		CString result2 = ToUtf16(result1.GetBuffer());
		ASSERT(result2 == source);
	*/
	CStringA ToUtf8(const wchar_t* value);

	CString ToUtf16(const char* value);

#pragma endregion //:REGION

#pragma region Helper Functions

	void GetWrapper(EWrapper e, wchar_t& prefix, wchar_t& postfix);

	/*
		CString source = L"**[Data]**";

		ASSERT(Get(source, EWrapper::SquareBracket, true) == L"[Data]" && source == L"****");
	*/
	CString Get(CString& source, EWrapper e, bool erase);

	/*
		WStringArray sa = { L"one", L"two", L"three" };

		ASSERT(Join(sa, '|') == L"one|two|three");
	*/
	CString Join(WStringArray& strArray, wchar_t token, bool removeLastToken = true);

	/*
		CString s1 = L"2.";
		CString s2 = L"2.0000";

		TrailingZero(s1);
		TrailingZero(s2);
		ASSERT(s1 == s2 && s1 == L"2.0");
	*/
	void TrailingZero(CString& source);

	/*
		CString source = L"[Data]";

		Unwrap(source, EWrapper::SquareBracket);
		ASSERT(source == L"Data");
	*/
	void Unwrap(CString& source, EWrapper e);

	/*
		CString source = L"Data";

		Wrap(source, '*', 3); ASSERT(source == L"***Data***");
		Wrap(source, '-', ':', 3); ASSERT(source == L"---***Data***:::");
		Wrap(source, EWrapper::CurlyBracket, 2); ASSERT(source == L"{{---***Data***:::}}");
	*/
	void Wrap(CString& source, wchar_t ch, int count = 1);

	void Wrap(CString& source, wchar_t frontCh, wchar_t backCh, int count = 1);

	void Wrap(CString& source, EWrapper e, int count = 1);

#pragma endregion //:REGION
}