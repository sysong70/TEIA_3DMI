#include "stdafx.h"
#include "WStr.h"
#include <memory>
#include <algorithm>



#define WSTR_EMPTY L""

#pragma region Stream Functions

int WStr::GetOffsetAfterBlank(const wchar_t* stream)
{
	int length = (int)::wcslen(stream);
	int index = 0;

	while (index < length) {
		switch (stream[index]) {
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
			index++;
			break;

		default:
			return index;
		}
	}

	return -1;
}



bool WStr::IsCharAfterBlank(wchar_t*& stream, wchar_t ch, bool peek)
{
	if (peek) {
		return (GetOffsetAfterBlank(stream) >= 0);
	}
	else {
		ShiftBlank(stream);

		if (stream[0] == ch) {
			stream++;
			return true;
		}
		else {
			return false;
		}
	}
}



bool WStr::IsDigit(CString& value)
{
	wchar_t* stream = (wchar_t*)value.GetBuffer();

	for (int i = 0; i < value.GetLength(); i++) {
		if (IsDigit(*stream) == false) {
			return false;
		}
		stream++;
	}

	return true;
}



bool WStr::IsDigit(wchar_t value)
{
	switch (value) {
	case L'0':
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
		return true;

	default:
		return false;
	}
}



bool WStr::IsNumeric(wchar_t value)
{
	switch (value) {
	case L'0':
	case L'1':
	case L'2':
	case L'3':
	case L'4':
	case L'5':
	case L'6':
	case L'7':
	case L'8':
	case L'9':
	case L'-':
	case L'+':
	case L'.':
	case L'e':
	case L'E':
		return true;

	default:
		return false;
	}
}



void WStr::ShiftBlank(wchar_t*& stream)
{
	int length = (int)::wcslen(stream);
	int index = 0;

	for (int index = 0; index < length; index++) {
		switch (stream[0]) {
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
			stream++;
			break;

		default:
			return;
		}
	}
}



bool WStr::ShiftTo(wchar_t*& stream, wchar_t ch)
{
	int length = (int)::wcslen(stream);
	if (length == 0) {
		return false;
	}

	int index = 0;
	wchar_t* origin = stream;

	for (int index = 0; index < length; index++) {
		if (origin[0] == ch) {
			stream = origin + 1;
			return true;
		}
		else {
			origin++;
		}
	}

	return false;
}



bool WStr::Split(const wchar_t* pSource, wchar_t token, WStringArray& atomArray)
{
	CString source(pSource);
	if (source.GetLength() == 0) {
		RETURN_FALSE;
	}

	CString atom;
	int previous = 0;
	int current = 0;

	while ((current = source.Find(token, previous)) != -1) {
		atom = source.Mid(previous, current - previous);
		if (!atom.IsEmpty()) {
			atomArray.push_back(atom);
		}
		previous = current + 1;
	}

	if (previous < source.GetLength()) {
		atomArray.push_back(source.Mid(previous, source.GetLength() - previous));
	}

	return (atomArray.size() > 0);
}

#pragma endregion //:REGION

#pragma region Startdard Functions

void WStr::Add(CString& source, wchar_t ch, int count)
{
	for (int i = 0; i < count; i++) {
		source += ch;
	}
}



bool WStr::Erase(CString& source, int start, wchar_t toCh, bool includeCh)
{
	int pos = source.Find(toCh, start);
	if (pos == -1) {
		return false;
	}

	int index = start;
	int count = pos - start + (includeCh ? 1 : 0);
	source.Delete(index, count);

	return true;
}

bool WStr::Erase(CString& source, int start, wchar_t fromCh, wchar_t toCh, bool includeCh)
{
	int from = source.Find(fromCh, start);
	if (from == -1) {
		return false;
	}

	int to = source.Find(toCh, from + 1);
	if (to == -1) {
		return false;
	}

	int index = 0;
	int count = 0;
	if (includeCh) {
		index = from;
		count = to - from + 1;
	}
	else {
		index = from + 1;
		count = to - from - 1;
	}

	source.Delete(index, count);

	return true;
}



CString WStr::Format(const wchar_t* pFormat, ...)
{
	CString stream;
	va_list argList;

	va_start(argList, pFormat);
	stream.FormatV(pFormat, argList);
	va_end(argList);

	return stream;
}



CString WStr::Get(CString& source, int start, int count, bool erase)
{
	CString result = source.Mid(start, count);
	if (erase) {
		source.Delete(start, count);
	}

	return result;
}

CString WStr::Get(CString& source, int start, wchar_t toCh, bool includeCh, bool erase)
{
	int pos = source.Find(toCh, start);
	if (pos == -1) {
		return WSTR_EMPTY;
	}

	int index = start;
	int count = pos - start + (includeCh ? 1 : 0);
	CString result = source.Mid(index, count);
	if (erase) {
		source.Delete(index, count);
	}

	return result;
}

CString WStr::Get(CString& source, int start, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase)
{
	int from = source.Find(fromCh, start);
	if (from == -1) {
		return WSTR_EMPTY;
	}

	int to = source.Find(toCh, from + 1);
	if (to == -1) {
		return WSTR_EMPTY;
	}

	int index = 0;
	int count = 0;
	if (includeCh) {
		index = from;
		count = to - from + 1;
	}
	else {
		index = from + 1;
		count = to - from - 1;
	}

	CString result = source.Mid(index, count);
	if (erase) {
		source.Delete(index, count);
	}

	return result;
}

#pragma endregion //:REGION

#pragma region Extended Functions

CString WStr::Back(CString& source, int count, bool erase)
{
	int start = source.GetLength() - count;
	CString result = source.Mid(start, count);
	if (erase) {
		source.Delete(start, count);
	}

	return result;
}

CString WStr::Back(CString& source, wchar_t toCh, bool includeCh, bool erase)
{
	int pos = source.ReverseFind(toCh);
	if (pos == -1) {
		return WSTR_EMPTY;
	}

	int index = pos + (includeCh ? 0 : 1);
	CString result = source.Mid(index);
	if (erase) {
		source.Delete(index, source.GetLength());
	}

	return result;
}

CString WStr::Back(CString& source, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase)
{
	int to = source.ReverseFind(toCh);
	int from = source.Find(fromCh);
	int pos = -1;
	while ((pos = source.Find(fromCh, from + 1)) > -1 && 0 <= pos && pos < to);

	if (to == -1 || from == -1 || to == from) {
		return WSTR_EMPTY;
	}

	int index = 0;
	int count = 0;
	if (includeCh) {
		index = from;
		count = to - from + 1;
	}
	else {
		index = from + 1;
		count = to - from - 1;
	}

	CString result = source.Mid(index, count);
	if (erase) {
		source.Delete(index, count);
	}

	return result;
}



CString WStr::Front(CString& source, int count, bool erase)
{
	return Get(source, 0, count, erase);
}

CString WStr::Front(CString& source, wchar_t toCh, bool includeCh, bool erase)
{
	return Get(source, 0, toCh, includeCh, erase);
}

CString WStr::Front(CString& source, wchar_t fromCh, wchar_t toCh, bool includeCh, bool erase)
{
	return Get(source, 0, fromCh, toCh, includeCh, erase);
}



void WStr::RemoveFrom(CString& source, wchar_t ch)
{
	int pos = source.Find(ch, 0);
	if (pos == -1) {
		return;
	}

	source.Delete(pos, source.GetLength());
}



void WStr::RemoveTo(CString& source, wchar_t ch)
{
	int pos = source.Find(ch, 0);
	if (pos == -1) {
		return;
	}

	source.Delete(0, pos + 1);
}

#pragma endregion //:REGION

#pragma region Cast Functions

double WStr::ToDouble(const wchar_t* value)
{
	return ::wcstod(value, nullptr);
}



int WStr::ToInteger(const wchar_t* value)
{
	return (int)::wcstol(value, nullptr, 0);
}



long WStr::ToLong(const wchar_t* value)
{
	return ::wcstol(value, nullptr, 0);
}



CString WStr::ToString(bool value)
{
	return (value ? L"True" : L"False");
}

CString WStr::ToString(int value)
{
	return Format(L"%d", value);
}

CString WStr::ToString(long value)
{
	return Format(L"%ld", value);
}

CString WStr::ToString(double value)
{
	return Format(L"%f", value);
}

CString WStr::ToString(double value, int digit)
{
	CString sDigit = Format(L"%%.%df", digit);

	return Format(sDigit.GetBuffer(), value);
}

CString WStr::ToString(DWORD_PTR value)
{
	CString sValue;
	sValue.Format(L"%llx", value);

	return sValue;
}

//:WARNING - wstring_convert deprecated (C++20)
//#include <codecvt>

CStringA WStr::ToUtf8(const wchar_t* value)
{
	//:WARNING - wstring_convert deprecated (C++20)
	//CString_convert<std::codecvt_utf8<wchar_t>> conv;
	//return conv.to_bytes(value);

	int length = (int)::wcslen(value);
	const auto needed = ::WideCharToMultiByte(CP_UTF8, 0, value, length, nullptr, 0, nullptr, nullptr);
	if (needed <= 0) {
		return "";
	}

	CStringA result;
	::WideCharToMultiByte(CP_UTF8, 0, value, length, result.GetBufferSetLength(needed), needed, nullptr, nullptr);

	return result;
}



CString WStr::ToUtf16(const char* value)
{
	//:WARNING - wstring_convert deprecated (C++20)
	//CString_convert<std::codecvt_utf8<wchar_t>> conv;
	//return conv.from_bytes(value);

	int length = (int)::strlen(value);
	const auto needed = ::MultiByteToWideChar(CP_UTF8, 0, value, length, nullptr, 0);
	if (needed <= 0) {
		return L"";
	}

	CString result;
	::MultiByteToWideChar(CP_UTF8, 0, value, length, result.GetBufferSetLength(needed), needed);

	return result;
}

#pragma endregion //:REGION

#pragma region Helper Functions

void WStr::GetWrapper(EWrapper e, wchar_t& prefix, wchar_t& postfix)
{
	switch (e) {
	case EWrapper::Space:
		prefix = postfix = ' ';
		break;

	case EWrapper::SingleQuot:
		prefix = postfix = '\'';
		break;

	case EWrapper::DoubleQuot:
		prefix = postfix = '\"';
		break;

	case EWrapper::RoundBracket:
		prefix = '(';
		postfix = ')';
		break;

	case EWrapper::CurlyBracket:
		prefix = '{';
		postfix = '}';
		break;

	case EWrapper::SquareBracket:
		prefix = '[';
		postfix = ']';
		break;

	default:
		prefix = 0;
		postfix = 0;
		DEBUG_STOP;
		break;
	}
}



CString WStr::Get(CString& source, EWrapper e, bool erase)
{
	if (source.IsEmpty()) {
		return L"";
	}

	wchar_t prefix, postfix;
	GetWrapper(e, prefix, postfix);

	int from = source.Find(prefix);
	if (from == -1) {
		return L"";
	}

	int to = source.Find(postfix, from + 1);
	if (to == -1) {
		return L"";
	}

	int count = to - from + 1;
	CString result = source.Mid(from, count);
	source.Delete(from, count);

	return result;
}



CString WStr::Join(WStringArray& strArray, wchar_t token, bool removeLastToken /*= true*/)
{
	CString result;

	for (const auto& str : strArray) {
		result += str + token;
	}

	if (!result.IsEmpty() && removeLastToken) {
		result.Delete(result.GetLength() - 1);
	}

	return result;
}



void WStr::TrailingZero(CString& source)
{
	source.TrimRight(L'0');
	if (!source.IsEmpty() && source.GetAt(source.GetLength() - 1) == L'.') {
		source += L'0';
	}
}



void WStr::Unwrap(CString& source, EWrapper e)
{
	wchar_t prefix, postfix;
	GetWrapper(e, prefix, postfix);

	int front = source.Find(prefix);
	int back = source.Find(postfix);
	if (front == 0 || back == source.GetLength() - 1) {
		source.Delete(front);
		source.Delete(back - 1);
	}
}



void WStr::Wrap(CString& source, wchar_t ch, int count /*= 1*/)
{
	for (int i = 0; i < count; i++) {
		source.Insert(0, ch);
		source.AppendChar(ch);
	}
}



void WStr::Wrap(CString& source, wchar_t frontCh, wchar_t backCh, int count)
{
	for (int i = 0; i < count; i++) {
		source.Insert(0, frontCh);
		source.AppendChar(backCh);
	}
}



void WStr::Wrap(CString& source, EWrapper e, int count /*= 1*/)
{
	wchar_t prefix = 0;
	wchar_t postfix = 0;
	GetWrapper(e, prefix, postfix);

	for (int i = 0; i < count; i++) {
		source.Insert(0, prefix);
		source.AppendChar(postfix);
	}
}

#pragma endregion //:REGION
