#include "stdafx.h"
#include "Json.h"
#include "Fio.h"
#include "WStr.h"

#ifdef _DEBUG
#ifndef DEBUG_NEW
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Json::Array theDummyArray;
Json::Object theDummyObject;
Json::Value theDummyValue;



namespace UnitTestJson
{
	using namespace Json;

	void Test()
	{
		// set data

		Object objectOut;

		Object& outParent = objectOut.CreateObject("Parent");
		outParent.SetBoolean("BoolValue", true);
		outParent.SetInteger("IntegerValue", 0);

		Object& outChild1 = outParent.CreateObject("Child1");
		outChild1.SetReal("RaalValue", 1.0);
		outChild1.SetString("StringValue", L"한글");

		Array& outChild2Array = outParent.CreateArray("Child2");
		for (int i = 0; i < 10; i++) {
			Object& obj = outChild2Array.AddObject();
			obj.SetInteger("Index", i);
			obj.SetString("Name", L"Name" + WStr::ToString(i));
		}

		// add other types to array (composite type)
		outChild2Array.AddInteger(1);
		outChild2Array.AddReal(1.0);
		outChild2Array.AddBoolean(true);

		CString outBuffer;
		objectOut.Serialize(outBuffer, 1);
		TRACE(L"\nOUT-BUFFER\n%s\n", outBuffer);

		// get data

		Object objectIn;
		wchar_t* pBuffer = (wchar_t*)outBuffer.GetBuffer();
		Reader::ReadObject(pBuffer, objectIn);

		CString inBuffer;
		objectIn.Serialize(inBuffer, 1);

		ASSERT(outBuffer == inBuffer);
	}
}

#pragma region Array Class

Json::Array::Array()
{
}



Json::Array::Array(CString& sourceStream)
{
	Helper::Load(sourceStream, *this);
}



Json::Array::Array(const Array& other)
{
	*this = other;
}



Json::Array& Json::Array::operator =(const Array& other)
{
	Clean();

	CString buffer;
	Array* pSource = (Array*)&other;
	pSource->Stringify(buffer);

	Helper::Load(buffer, *this);

	return *this;
}



Json::Array::~Array()
{
	Clean();
}

//--------------------------------------------------------------------------------------------------

Json::Value* Json::Array::GetAt(int i)
{
	ASSERT(0 <= i && i < (int)m_buffer.size());
	return m_buffer.at(i);
}



Json::Value* Json::Array::operator [](int i)
{
	ASSERT(0 <= i && i < (int)m_buffer.size());
	return GetAt(i);
}



int Json::Array::GetSize()
{
	return (int)m_buffer.size();
}



std::vector<Json::Value*>& Json::Array::GetBuffer()
{
	return m_buffer;
}

//--------------------------------------------------------------------------------------------------

void Json::Array::AddBoolean(bool value)
{
	AddValue(new Value(value));
}



void Json::Array::AddInteger(int value)
{
	AddValue(new Value(value));
}



void Json::Array::AddReal(double value)
{
	AddValue(new Value(value));
}



CString& Json::Array::AddString()
{
	Value* pValue = new Value();
	AddValue(pValue);

	return pValue->CreateString();
}



void Json::Array::AddString(CString value)
{
	Value* pValue = new Value(value);
	AddValue(pValue);
}



Json::Object& Json::Array::AddObject()
{
	Value* pValue = new Value();
	AddValue(pValue);

	return pValue->CreateObject();
}



void Json::Array::AddObject(Object& value)
{
	Value* pValue = new Value(value);
	AddValue(pValue);
}



void Json::Array::AddValue(Value* pValue)
{
	m_buffer.push_back(pValue);
}



bool Json::Array::ToArray(int& count, int*& pValues)
{
	count = GetSize();
	if (count == 0) {
		return false;
	}

	pValues = new int[count];
	if (pValues == nullptr) {
		RETURN_FALSE;
	}

	for (int i = 0; i < count; i++) {
		pValues[i] = m_buffer[i]->ToInteger();
	}

	return true;
}

bool Json::Array::ToArray(WStringArray& values)
{
	values.clear();

	for (int i = 0; i < GetSize(); i++) {
		Json::Value* pValue = m_buffer[i];
		values.push_back(pValue->ToString());
	}

	return true;
}

//--------------------------------------------------------------------------------------------------

void Json::Array::Clean()
{
	for (auto* pValue : m_buffer) {
		REMOVE_POINTER(pValue);
	}

	m_buffer.clear();
}



void Json::Array::Serialize(CString& buffer, int indent)
{
	buffer += L"[\n";

	for (auto* pValue : m_buffer) {
		if (pValue) {
			WStr::Add(buffer, L'\t', indent);
			pValue->Serialize(buffer, (pValue->IsObject() ? indent + 1 : 0));
			buffer += L",\n";
		}
	}


	buffer.TrimRight(L",\n");
	buffer += L'\n';
	WStr::Add(buffer, L'\t', indent - 1);
	buffer += L"]";
}



void Json::Array::Stringify(CString& buffer)
{
	buffer += L"[";

	for (auto* pValue : m_buffer) {
		if (pValue) {
			pValue->Stringify(buffer);
			buffer += L",";
		}
	}

	buffer.TrimRight(L",");
	buffer += L"]";
}

#pragma endregion //:REGION

#pragma region Value Class

Json::Value::Value()
	: m_eType(EValueType::Unknown)
{
	Initialize();
}



Json::Value::Value(int value)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetInteger(value);
}



Json::Value::Value(DWORD value)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetInteger((int)value);
}



Json::Value::Value(double value)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetReal(value);
}



Json::Value::Value(bool value)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetBoolean(value);
}



Json::Value::Value(CString value)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetString(value);
}



Json::Value::Value(CString number, bool bReal)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetNumber(number, bReal);
}



Json::Value::Value(Array& arrayData)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetArray(&arrayData);
}



Json::Value::Value(Object& objectData)
	: m_eType(EValueType::Unknown)
{
	Initialize();
	SetObject(&objectData);
}



Json::Value::Value(const Value& other)
{
	m_eType = other.m_eType;

	Initialize();

	switch (m_eType) {
	case EValueType::String:
		m_valueHolder.vString = new CString(*(other.m_valueHolder.vString));
		break;

	case EValueType::Array:
		m_valueHolder.vArray = new Array(*(other.m_valueHolder.vArray));
		break;

	case EValueType::Object:
		m_valueHolder.vObject = new Object(*(other.m_valueHolder.vObject));
		break;

	default:
		m_valueHolder = other.m_valueHolder;
		break;
	}
}



Json::Value::~Value()
{
	Clean();
}

//--------------------------------------------------------------------------------------------------

Json::EValueType Json::Value::GetType()
{
	return m_eType;
}



bool Json::Value::IsValid()
{
	return (m_eType != EValueType::Unknown);
}



bool Json::Value::IsArray()
{
	return (m_eType == EValueType::Array);
}



bool Json::Value::IsBoolean()
{
	return (m_eType == EValueType::Boolean);
}



bool Json::Value::IsInteger()
{
	return (m_eType == EValueType::Int || m_eType == EValueType::Uint);
}



bool Json::Value::IsNumber()
{
	return (m_eType == EValueType::Int || m_eType == EValueType::Uint || m_eType == EValueType::Real);
}



bool Json::Value::IsObject()
{
	return (m_eType == EValueType::Object);
}



bool Json::Value::IsReal()
{
	return (m_eType == EValueType::Real);
}



bool Json::Value::IsString()
{
	return (m_eType == EValueType::String);
}

//--------------------------------------------------------------------------------------------------

Json::Array* Json::Value::ToArray()
{
	return (IsArray() ? m_valueHolder.vArray : nullptr);
}



bool Json::Value::ToBoolean()
{
	switch (m_eType) {
	case EValueType::Boolean:
		return m_valueHolder.vBoolean;

	case EValueType::Int:
	case EValueType::Uint:
		return m_valueHolder.vInteger != 0;

	case EValueType::Real:
		return (int)m_valueHolder.vReal != 0;

	default:
		DEBUG_STOP;
		return false;
	}
}



int Json::Value::ToInteger()
{
	switch (m_eType) {
	case EValueType::Boolean:
		return m_valueHolder.vBoolean ? 1 : 0;

	case EValueType::Int:
	case EValueType::Uint:
		return m_valueHolder.vInteger;

	case EValueType::Real:
		return (int)m_valueHolder.vReal;

	case EValueType::String:
		if (m_valueHolder.vString->GetAt(0) == L'0' &&
			m_valueHolder.vString->GetAt(1) == L'x' || m_valueHolder.vString->GetAt(1) == L'X') {
			return WStr::FromHex(m_valueHolder.vString->GetBuffer());
		}
		else {
			return WStr::ToInteger(m_valueHolder.vString->GetBuffer());
		}

	default:
		DEBUG_STOP;
		return 0;
	}
}



Json::Object* Json::Value::ToObject()
{
	return (IsObject() ? m_valueHolder.vObject : nullptr);
}



double Json::Value::ToReal()
{
	switch (m_eType) {
	case EValueType::Boolean:
		return m_valueHolder.vBoolean ? 1.0 : 0.0;

	case EValueType::Int:
	case EValueType::Uint:
		return (double)m_valueHolder.vInteger;

	case EValueType::Real:
		return m_valueHolder.vReal;

	default:
		DEBUG_STOP;
		return 0;
	}
}



CString Json::Value::ToString()
{
	switch (m_eType) {
	case EValueType::Boolean:
		return m_valueHolder.vBoolean ? L"true" : L"false";

	case EValueType::Int:
	case EValueType::Uint:
		return WStr::ToString(m_valueHolder.vInteger);

	case EValueType::Real:
		return WStr::ToString(m_valueHolder.vReal, 3);

	case EValueType::String:
		return *m_valueHolder.vString;

	default:
		DEBUG_STOP;
		return L"";
	}
}

//--------------------------------------------------------------------------------------------------

Json::Array& Json::Value::AsArray()
{
	ASSERT(IsArray() && m_valueHolder.vArray != nullptr);
	return *m_valueHolder.vArray;
}



bool Json::Value::AsBoolean()
{
	ASSERT(IsBoolean());
	return m_valueHolder.vBoolean;
}



int Json::Value::AsInteger()
{
	ASSERT(IsInteger());
	return m_valueHolder.vInteger;
}



Json::Object& Json::Value::AsObject()
{
	ASSERT(IsObject() && m_valueHolder.vObject != nullptr);
	return *m_valueHolder.vObject;
}



double Json::Value::AsReal()
{
	ASSERT(IsReal());
	return m_valueHolder.vReal;
}



CString& Json::Value::AsString()
{
	ASSERT(IsString() && m_valueHolder.vString != nullptr);
	return *m_valueHolder.vString;
}

//--------------------------------------------------------------------------------------------------

Json::Array& Json::Value::CreateArray()
{
	Clean();
	SetArray(new Array());

	return *m_valueHolder.vArray;
}



Json::Object& Json::Value::CreateObject()
{
	Clean();
	SetObject(new Object());

	return *m_valueHolder.vObject;
}



CString& Json::Value::CreateString()
{
	Clean();
	m_valueHolder.vString = new CString();

	return *m_valueHolder.vString;
}

//--------------------------------------------------------------------------------------------------

void Json::Value::SetArray(Array* pArray)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Array);

	if (m_eType == EValueType::Array && m_valueHolder.vArray != nullptr) {
		REMOVE_POINTER(m_valueHolder.vArray);
	}

	m_valueHolder.vArray = pArray;
	m_eType = EValueType::Array;
}



void Json::Value::SetBoolean(bool value)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Boolean);

	m_valueHolder.vBoolean = value;
	m_eType = EValueType::Boolean;
}



void Json::Value::SetInteger(int value)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Int);

	m_valueHolder.vInteger = value;
	m_eType = EValueType::Int;
}



void Json::Value::SetNull()
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Null);

	m_eType = EValueType::Null;
}



void Json::Value::SetNumber(CString& value, bool real)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Real || m_eType == EValueType::Int);

	if (real) {
		m_valueHolder.vReal = WStr::ToDouble(value.GetBuffer());
		m_eType = EValueType::Real;
	}
	else {
		m_valueHolder.vInteger = WStr::ToInteger(value.GetBuffer());
		m_eType = EValueType::Int;
	}
}



void Json::Value::SetObject(Object* pObject)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Object);

	if (m_eType == EValueType::Object && m_valueHolder.vObject != nullptr) {
		REMOVE_POINTER(m_valueHolder.vObject);
	}

	m_valueHolder.vObject = pObject;
	m_eType = EValueType::Object;
}



void Json::Value::SetReal(double value)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::Real);

	m_valueHolder.vReal = value;
	m_eType = EValueType::Real;
}



void Json::Value::SetString(const CString& value)
{
	ASSERT(m_eType == EValueType::Unknown || m_eType == EValueType::String);

	if (m_eType == EValueType::String && m_valueHolder.vString != nullptr) {
		REMOVE_POINTER(m_valueHolder.vString);
	}

	m_valueHolder.vString = new CString(value);
	m_eType = EValueType::String;
}

//--------------------------------------------------------------------------------------------------

void Json::Value::Clean()
{
	switch (m_eType) {
	case EValueType::String:
		REMOVE_POINTER(m_valueHolder.vString);
		break;

	case EValueType::Array:
		REMOVE_POINTER(m_valueHolder.vArray);
		break;

	case EValueType::Object:
		REMOVE_POINTER(m_valueHolder.vObject);
		break;

	default:
		break;
	}
}



void Json::Value::Initialize()
{
	m_valueHolder.vString = nullptr;
	m_valueHolder.vArray = nullptr;
	m_valueHolder.vObject = nullptr;
}



Json::Value* Json::Value::DeepCopy()
{
	CString buffer;
	Stringify(buffer);

	wchar_t* pBuffer = (wchar_t*)buffer.GetBuffer();
	Value* pNew = new Value();

	if (Reader::ReadValue(pBuffer, *pNew)) {
		return pNew;
	}
	else {
		DEBUG_STOP;
		return nullptr;
	}
}



void Json::Value::Serialize(CString& buffer, int indent)
{
	switch (m_eType) {
	case EValueType::Null:
		buffer += L"null";
		break;

	case EValueType::Boolean:
		buffer += (m_valueHolder.vBoolean ? L"true" : L"false");
		break;

	case EValueType::Int:
	case EValueType::Uint: {
		CString sValue = WStr::Format(L"%d", m_valueHolder.vInteger);
		buffer += sValue;
	} break;

	case EValueType::Real: {
		CString sValue = WStr::Format(L"%f", m_valueHolder.vReal);
		WStr::TrailingZero(sValue);
		buffer += sValue;
	} break;

	case EValueType::String:
		buffer += L"\"" + *(m_valueHolder.vString) + L"\"";
		break;

	case EValueType::Array:
		m_valueHolder.vArray->Serialize(buffer, indent);
		break;

	case EValueType::Object:
		m_valueHolder.vObject->Serialize(buffer, indent);
		break;

	default:
		DEBUG_STOP;
		break;
	}
}



void Json::Value::Stringify(CString& buffer)
{
	switch (m_eType) {
	case EValueType::Null:
		buffer += L"null";
		break;

	case EValueType::Boolean:
		buffer += (m_valueHolder.vBoolean ? L"true" : L"false");
		break;

	case EValueType::Int:
	case EValueType::Uint: {
		CString sValue = WStr::Format(L"%d", m_valueHolder.vInteger);
		buffer += sValue;
	} break;

	case EValueType::Real: {
		CString sValue = WStr::Format(L"%f", m_valueHolder.vReal);
		WStr::TrailingZero(sValue);
		buffer += sValue;
	} break;

	case EValueType::String: {
		CString string = *(m_valueHolder.vString);
		buffer += L"\"" + string + L"\"";
	} break;

	case EValueType::Array:
		m_valueHolder.vArray->Stringify(buffer);
		break;

	case EValueType::Object:
		m_valueHolder.vObject->Stringify(buffer);
		break;

	default:
		DEBUG_STOP;
		break;
	}
}

#pragma endregion //:REGION

#pragma region Pair Class

Json::Pair::Pair()
	: pValue(nullptr)
{
}



Json::Pair::Pair(CStringA name)
	: Name(name)
	, pValue(new Value())
{
}



Json::Pair::Pair(CStringA name, Value* p)
	: Name(name)
	, pValue(p)
{
}



Json::Pair::~Pair()
{
	REMOVE_POINTER(pValue);
}

#pragma endregion //:REGION

#pragma region Object Class

Json::Object::Object()
{
}



Json::Object::Object(CString& sourceStream)
{
	Helper::Load(sourceStream, *this);
}



Json::Object::Object(const Object& other)
{
	*this = other;
}



Json::Object::~Object()
{
	Clean();
}



Json::Object& Json::Object::operator =(const Object& other)
{
	Clean();

	CString buffer;
	Object* pSource = (Object*)&other;
	pSource->Stringify(buffer);

	Helper::Load(buffer, *this);

	return *this;
}



Json::Value& Json::Object::operator [](const char* name)
{
	Pair* pPair = Look(name);
	if (pPair == nullptr) {
		pPair = new Pair(name, new Value());
		m_members.push_back(pPair);
	}

	return *(pPair->pValue);
}

//--------------------------------------------------------------------------------------------------

Json::Array& Json::Object::CreateArray(CStringA name)
{
	Value* pValue = FindValue(name);
	if (pValue == nullptr) {
		pValue = new Value();
		SetValue(name, pValue);
	}

	return pValue->CreateArray();
}



Json::Object& Json::Object::CreateObject(CStringA name)
{
	Value* pValue = FindValue(name);
	if (pValue == nullptr) {
		pValue = new Value();
		SetValue(name, pValue);
	}

	return pValue->CreateObject();
}



CString& Json::Object::CreateString(CStringA name)
{
	Value* pValue = FindValue(name);
	if (pValue == nullptr) {
		pValue = new Value();
		SetValue(name, pValue);
	}

	return pValue->CreateString();
}

//--------------------------------------------------------------------------------------------------

Json::Value* Json::Object::FindValue(CStringA name)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		return pPair->pValue;
	}

	return nullptr;
}



Json::Array& Json::Object::GetArray(CStringA name)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		return pPair->pValue->AsArray();
	}

	DEBUG_STOP;
	return theDummyArray;
}



Json::Object& Json::Object::GetAt(CStringA name)
{
	return GetObject(name);
}



Json::Object& Json::Object::GetObject(CStringA name)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		return pPair->pValue->AsObject();
	}

	DEBUG_STOP;
	return theDummyObject;
}



Json::Value& Json::Object::GetValue(CStringA name)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		return *(pPair->pValue);
	}

	DEBUG_STOP;
	return theDummyValue;
}

//--------------------------------------------------------------------------------------------------

bool Json::Object::GetBoolean(CStringA name, bool defaultValue)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		defaultValue = pPair->pValue->ToBoolean();
	}

	return defaultValue;
}



int Json::Object::GetInteger(CStringA name, int defaultValue)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		defaultValue = pPair->pValue->ToInteger();
	}

	return defaultValue;
}



double Json::Object::GetReal(CStringA name, double defaultValue)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		defaultValue = pPair->pValue->ToReal();
	}

	return defaultValue;

}



CString Json::Object::GetString(CStringA name, CString defaultValue)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		defaultValue = pPair->pValue->ToString();
	}

	return defaultValue;
}

//--------------------------------------------------------------------------------------------------

void Json::Object::SetArray(CStringA name, Array* pValue)
{
	SetValue(name, new Value(*pValue));
}



void Json::Object::SetBoolean(CStringA name, bool value)
{
	SetValue(name, new Value(value));
}



void Json::Object::SetInteger(CStringA name, int value)
{
	SetValue(name, new Value(value));
}



void Json::Object::SetObject(CStringA name, Object* pValue)
{
	SetValue(name, new Value(*pValue));
}



void Json::Object::SetReal(CStringA name, double value)
{
	SetValue(name, new Value(value));
}



void Json::Object::SetString(CStringA name, CString value)
{
	SetValue(name, new Value(value));
}



void Json::Object::SetValue(CStringA name, Value* pValue, bool deepCopy /*= false*/)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		REMOVE_POINTER(pPair->pValue);
		pPair->pValue = pValue;
	}
	else {
		if (deepCopy) {
			m_members.push_back(new Pair(name, pValue->DeepCopy()));
		}
		else {
			m_members.push_back(new Pair(name, pValue));
		}
	}
}

//--------------------------------------------------------------------------------------------------

DWORD_PTR Json::Object::GetDwordPtr(CStringA name, DWORD_PTR defaultValue)
{
	Pair* pPair = Look(name.GetBuffer());
	if (pPair != nullptr) {
		CString value = pPair->pValue->ToString();
		::swscanf_s(value, L"%llx", (DWORD_PTR*)&defaultValue);
	}

	return defaultValue;
}



void Json::Object::SetDwordPtr(CStringA name, DWORD_PTR value)
{
	CString cast;
	cast.Format(L"%llx", value);
	SetValue(name, new Value(cast));
}

//--------------------------------------------------------------------------------------------------

void Json::Object::Clean()
{
	for (Pair* pPair : m_members) {
		REMOVE_POINTER(pPair);
	}

	m_members.clear();
}



bool Json::Object::IsEmpty()
{
	return m_members.size() == 0;
}



void Json::Object::Remove(const char* pName)
{
	CStringA name(pName);

	std::vector<Pair*>::iterator iter;
	for (iter = m_members.begin(); iter != m_members.end(); iter++) {
		Pair* pPair = *iter;
		if (pPair->Name == name) {
			REMOVE_POINTER(pPair);
			m_members.erase(iter);
			break;
		}
	}
}



std::vector<Json::Pair*>& Json::Object::GetMembers()
{
	return m_members;
}



void Json::Object::GatherNames(AStringArray& names)
{
	for (auto* pPair : m_members) {
		if (pPair != nullptr) {
			names.push_back(pPair->Name);
		}
	}
}



void Json::Object::GatherNames(WStringArray& names)
{
	for (auto* pPair : m_members) {
		if (pPair != nullptr) {
			names.push_back(WStr::ToUtf16(pPair->Name.GetBuffer()));
		}
	}
}



void Json::Object::Serialize(CString& buffer, int indent)
{
	buffer += L"{\n";

	for (auto* pPair : m_members) {
		if (pPair->pValue) {
			WStr::Add(buffer, L'\t', indent);
			buffer += L"\"" + WStr::ToUtf16(pPair->Name.GetBuffer()) + L"\": ";
			pPair->pValue->Serialize(buffer, indent + 1);
		}
		else {
			DEBUG_STOP;
		}

		buffer += L",\n";
	}

	buffer.TrimRight(L",\n");
	buffer += L'\n';
	WStr::Add(buffer, L'\t', indent - 1);
	buffer += L'}';
}



void Json::Object::Stringify(CString& buffer)
{
	buffer += L"{";

	for (auto* pPair : m_members) {
		if (pPair->pValue) {
			//buffer += L"\"" + WStr::ToUtf16(pPair->Name.GetBuffer()) + L"\":";
			buffer += L"\"";
			buffer += WStr::ToUtf16(pPair->Name.GetBuffer());
			buffer += L"\":";
			pPair->pValue->Stringify(buffer);
		}
		else {
			DEBUG_STOP;
		}

		buffer += L",";
	}

	buffer.TrimRight(L",");
	buffer += L'}';
}



CString Json::Object::ToString()
{
	CString buffer;
	Stringify(buffer);

	return buffer;
}

//--------------------------------------------------------------------------------------------------

Json::Pair* Json::Object::Look(const char* pName) const
{
	CStringA name(pName);

	for (auto* pPair : m_members) {
		if (pPair->Name == name) {
			return pPair;
		}
	}

	return nullptr;
}



bool Json::Object::Look(const char* pName, Value*& pValue) const
{
	Pair* pPair = Look(pName);
	if (pPair) {
		pValue = pPair->pValue;
		return true;
	}

	return false;
}

#pragma endregion //:REGION

#pragma region Reader Namespace

bool Json::Reader::ReadArray(wchar_t*& pStream, Array& arrayData)
{
	if (!WStr::IsCharAfterBlank(pStream, L'[')) {
		return false;
	}
	if (WStr::IsCharAfterBlank(pStream, L']')) {
		return true; // null array
	}

	while (pStream[0] != 0) {
		Value* pValue = new Value();

		WStr::ShiftBlank(pStream);
		if (!ReadValue(pStream, *pValue)) {
			REMOVE_POINTER(pValue);
			return false;
		}

		arrayData.AddValue(pValue);
		// check continue
		if (!WStr::IsCharAfterBlank(pStream, L',')) {
			break;
		}
	}

	if (!WStr::IsCharAfterBlank(pStream, L']')) {
		return false;
	}

	return true;
}



bool Json::Reader::ReadBoolean(wchar_t*& pStream, bool& value)
{
	if (_tcsncmp(pStream, L"true", 4) == 0) {
		pStream += 4;
		value = true;
		return true;
	}
	else if (_tcsncmp(pStream, L"false", 5) == 0) {
		pStream += 5;
		value = false;
		return true;
	}
	else {
		return false;
	}
}



bool Json::Reader::ReadNumber(wchar_t*& pStream, CString& value, bool& bReal)
{
	int nMax = (int)_tcslen(pStream);
	int nIndex = 0;

	while (nIndex < nMax) {
		if (WStr::IsNumeric(pStream[0])) {
			value += pStream[0];
			nIndex++;
			pStream++;
		}
		else {
			if (value.GetLength() > 0) {
				bReal = (value.FindOneOf(L".eE") != -1);
				return true;
			}
			else {
				return false;
			}
		}
	}

	return false;
}



bool Json::Reader::ReadObject(wchar_t*& pStream, Object& objectData)
{
	if (!WStr::IsCharAfterBlank(pStream, L'{')) {
		return false;
	}
	if (WStr::IsCharAfterBlank(pStream, L'}')) {
		return true; // null object
	}

	while (pStream[0] != 0) {
		CString name;
		Value* pValue = new Value();

		WStr::ShiftBlank(pStream);
		if (!ReadString(pStream, name)) {
			return false;
		}
		if (!WStr::IsCharAfterBlank(pStream, L':')) {
			return false;
		}

		WStr::ShiftBlank(pStream);
		if (!ReadValue(pStream, *pValue)) {
			REMOVE_POINTER(pValue);
			return false;
		}

		CStringA aName;
		objectData.SetValue(WStr::ToUtf8(name.GetBuffer()), pValue);
		// check continue
		if (!WStr::IsCharAfterBlank(pStream, L',')) {
			break;
		}
	}

	if (!WStr::IsCharAfterBlank(pStream, L'}')) {
		return false;
	}

	return true;
}



bool Json::Reader::ReadString(wchar_t*& pStream, CString& value)
{
	if (pStream[0] != L'\"') {
		return false;
	}

	wchar_t* pStart = pStream + 1;
	wchar_t* pEnd = nullptr;
	while (pEnd = wcschr(pStart, L'\"')) {
		if (*(pEnd - 1) == L'\\') {
			pStart = pEnd + 1;
		}
		else {
			break;
		}
	}

	if (pEnd == nullptr) {
		return false;
	}

	int nCount = pEnd - pStream - 1;
	::wcsncpy(value.GetBufferSetLength(nCount), pStream + 1, nCount);

	//:CHECK
	value.Replace(L"\\\"", L"\"");
	value.Replace(L"\\\\", L"\\");

	pStream += (nCount + 2);

	return true;
}



bool Json::Reader::ReadValue(wchar_t*& pStream, Value& value)
{
	while (pStream[0] != 0) {
		switch (pStream[0]) {
		case L' ':
		case L'\t':
		case L'\r':
		case L'\n':
			pStream++;
			break;

		case L',':
			return true;

		case L'"': {
			CString stringValue;
			if (ReadString(pStream, stringValue)) {
				value.SetString(stringValue);
				return true;
			}
			RETURN_FALSE;
		}

		case L'[': {
			Array* pArray = new Array();
			if (ReadArray(pStream, *pArray)) {
				value.SetArray(pArray);
				return true;
			}
			else {
				REMOVE_POINTER(pArray);
				RETURN_FALSE;
			}
		}

		case L'{': {
			Object* pObject = new Object();
			if (ReadObject(pStream, *pObject)) {
				value.SetObject(pObject);
				return true;
			}
			else {
				REMOVE_POINTER(pObject);
				RETURN_FALSE;
			}
		}

		case L'n': {
			if (SkipNull(pStream)) {
				value.SetNull();
				return true;
			}
			RETURN_FALSE;
		}

		case L't':
		case L'f': {
			bool boolValue;
			if (ReadBoolean(pStream, boolValue)) {
				value.SetBoolean(boolValue);
				return true;
			}
			RETURN_FALSE;
		}

		default:
			if (WStr::IsNumeric(pStream[0])) {
				CString numberValue;
				bool bReal;
				if (ReadNumber(pStream, numberValue, bReal)) {
					value.SetNumber(numberValue, bReal);
					return true;
				}
				else {
					RETURN_FALSE;
				}
			}
			else {
				RETURN_FALSE;
			}
		}
	}

	return false;
}



bool Json::Reader::SkipNull(wchar_t*& pStream)
{
	if (_tcsncmp(pStream, L"null", 4) == 0) {
		pStream += 4;
		return true;
	}

	return false;
}

#pragma endregion //:REGION

#pragma region Helper Namespace

bool Json::Helper::Load(CString& value, Object& object)
{
	return Load(value.GetBuffer(), object);
}



bool Json::Helper::Load(const wchar_t* pValue, Object& object)
{
	wchar_t* pBuffer = (wchar_t*)pValue;
	return Reader::ReadObject(pBuffer, object);
}



bool Json::Helper::Load(CString& value, Array& array)
{
	return Load(value.GetBuffer(), array);
}



bool Json::Helper::Load(const wchar_t* pValue, Array& array)
{
	wchar_t* pBuffer = (wchar_t*)pValue;
	return Reader::ReadArray(pBuffer, array);
}



bool Json::Helper::Read(CString path, Object& object)
{
	Fio::TextFile file;

	if (file.Open(path.GetBuffer(), Fio::EMode::Read, Fio::EEncoding::UTF16LE) == false) {
		return false;
	}

	CString result;
	size_t read = file.Read(result);
	wchar_t* stream = (wchar_t*)result.GetBuffer();

	return Reader::ReadObject(stream, object);
}



bool Json::Helper::Write(CString path, Object& object, bool bSerialize /*= true*/)
{
	Fio::TextFile file;

	if (file.Open(path.GetBuffer(), Fio::EMode::Write, Fio::EEncoding::UTF16LE) == false) {
		return false;
	}

	CString buffer;
	if (bSerialize) {
		object.Serialize(buffer, 1);
	}
	else {
		object.Stringify(buffer);
	}

	file.Write(buffer.GetBuffer(), buffer.GetLength());

	return true;
}



Json::Value* Json::Helper::FindValueByPath(Object& object, CStringA path)
{
#define RETURN_INVALID root.Initialize(); return nullptr

	WStringArray paths;
	if (path.IsEmpty() || WStr::Split(CString(path).GetBuffer(), L'/', paths) == false) {
		return nullptr;
	}

	// add path itself
	if (paths.size() == 0) {
		paths.push_back(CString(path));
	}

	Json::Value root(object);
	Json::Value* pValue = &root;

	for (auto& sub : paths) {
		if (pValue == nullptr || pValue->IsValid() == false) {
			RETURN_INVALID;
		}

		if (pValue->GetType() == EValueType::Object) {
			Json::Object& target = pValue->AsObject();
			pValue = target.FindValue((CStringA)sub);
		}
		else if (pValue->GetType() == EValueType::Array) {
			Json::Array& target = pValue->AsArray();
			if (target.GetSize() == 0) {
				RETURN_INVALID;
			}

			if (WStr::IsDigit(sub) == false) {
				RETURN_INVALID;
			}
			int index = WStr::ToInteger(sub);
			if ((0 <= index && index < target.GetSize()) == false) {
				RETURN_INVALID;
			}

			pValue = target.GetAt(index);
		}
		else {
			RETURN_INVALID;
		}
	}

	root.Initialize();

	return pValue;

#undef RETURN_INVALID
}



Json::Object* Json::Helper::FindObjectByPath(Object& object, CStringA path)
{
	Json::Value* pValue = FindValueByPath(object, path);

	if (pValue != nullptr && pValue->GetType() == EValueType::Object) {
		return &pValue->AsObject();
	}
	else {
		return nullptr;
	}
}



CStringA Json::Helper::GetIdString(UINT id)
{
	CStringA buffer;
	buffer.Format("_Id%d_", id);

	return buffer;
}



DWORD_PTR Json::Helper::GetDwordPtr(Object& object, UINT id, DWORD_PTR defaultValue)
{
	return object.GetDwordPtr(GetIdString(id), defaultValue);
}



LONGLONG Json::Helper::GetLongLong(Object& object, UINT id, LONGLONG defaultValue)
{
	return (LONGLONG)object.GetDwordPtr(GetIdString(id), defaultValue);
}


double Json::Helper::GetReal(Object& object, UINT id, double defaultValue)
{
	return object.GetReal(GetIdString(id), defaultValue);
}



double Json::Helper::GetRealRawString(Object& object, UINT id, double defaultValue)
{
	CString value = object.GetString(GetIdString(id), L"");
	if (value.IsEmpty()) {
		return defaultValue;
	}

	::swscanf_s(value, L"%llx", (unsigned long long*)&defaultValue);

	return defaultValue;
}



CString Json::Helper::GetString(Object& object, UINT id, CString defaultValue)
{
	return object.GetString(GetIdString(id), defaultValue);
}



void Json::Helper::SetArray(Object& object, UINT id, Array* pValue)
{
	object.SetArray(GetIdString(id), pValue);
}



void Json::Helper::SetBoolean(Object& object, UINT id, bool value)
{
	object.SetBoolean(GetIdString(id), value);
}



void Json::Helper::SetDwordPtr(Object& object, UINT id, DWORD_PTR value)
{
	object.SetDwordPtr(GetIdString(id), value);
}



void Json::Helper::SetInteger(Object& object, UINT id, int value)
{
	object.SetInteger(GetIdString(id), value);
}



void Json::Helper::SetLongLong(Object& object, UINT id, LONGLONG value)
{
	CString buffer;
	buffer.Format(L"%llx", *(DWORD_PTR*)&value);

	object.SetValue(GetIdString(id), new Value(buffer));
}



void Json::Helper::SetObject(Object& object, UINT id, Object* pValue)
{
	object.SetObject(GetIdString(id), pValue);
}



void Json::Helper::SetReal(Object& object, UINT id, double value)
{
	object.SetReal(GetIdString(id), value);
}



void Json::Helper::SetRealRawString(Object& object, UINT id, double value, bool allDigit)
{
	// Assumes sizeof(long long) == 8.
	CString buffer;

	if (allDigit) {
		buffer.Format(L"%016llx", *(unsigned long long*)&value);
	}
	else {
		buffer.Format(L"%llx", *(unsigned long long*)&value);
	}

	object.SetValue(GetIdString(id), new Value(buffer));
}



void Json::Helper::SetString(Object& object, UINT id, CString value)
{
	CString buffer;
	buffer.Format(L"%llx", *(unsigned long long*) & value);

	object.SetValue(GetIdString(id), new Value(buffer));
}



COLORREF Json::Helper::ToColor(CString value)
{
	wchar_t* end;
	DWORD cast = wcstoul(value, &end, 16);
	BYTE* pCast = (BYTE*)&cast;

	return RGB(pCast[2], pCast[1], pCast[0]);
}



CString Json::Helper::ToString(COLORREF value)
{
	BYTE red = GetRValue(value);
	BYTE green = GetGValue(value);
	BYTE blue = GetBValue(value);

	CString buffer;
	buffer.Format(L"0x%.2X%.2X%.2X", red, green, blue);

	return buffer;
}

#pragma endregion //:REGION

#pragma region Builder Namespace

CString Json::Builder::Encode(const char* pName, bool bValue, bool bContinue /*= true*/)
{
	return WStr::Format(L"\"%s\":%s", pName, (bValue ? L"true" : L"false")) + (bContinue ? L"," : L"");
}



CString Json::Builder::Encode(const char* pName, CString sValue, bool bContinue /*= true*/)
{
	return WStr::Format(L"\"%s\":%s", pName, sValue) + (bContinue ? L"," : L"");
}



CString Json::Builder::Encode(const char* pName, double dValue, bool bContinue /*= true*/)
{
	return WStr::Format(L"\"%s\":%f", pName, dValue) + (bContinue ? L"," : L"");
}



CString Json::Builder::Encode(const char* pName, int nValue, bool bContinue /*= true*/)
{
	return WStr::Format(L"\"%s\":%d", pName, nValue) + (bContinue ? L"," : L"");
}



CString Json::Builder::Encode(const char* pName, void* pArray, int count, EValueType eType, bool bContinue /*= true*/)
{
	CString sArray = WStr::ToUtf16(pName);
	sArray += L"[";

	if (eType == EValueType::Boolean) {
		bool* pBoolean = (bool*)pArray;

		for (int i = 0; i < count; i++) {
			sArray += (pBoolean[i] ? L"true" : L"false");
			if (i < count - 1) {
				sArray += L",";
			}
		}
	}
	else if (eType == EValueType::Int) {
		int* pInt = (int*)pArray;
		for (int i = 0; i < count; i++) {
			sArray += WStr::Format(L"%d", pInt[i]);
			if (i < count - 1) {
				sArray += L",";
			}
		}
	}
	else if (eType == EValueType::Uint) {
		UINT* pInt = (UINT*)pArray;
		for (int i = 0; i < count; i++) {
			sArray += WStr::Format(L"%d", pInt[i]);
			if (i < count - 1) {
				sArray += L",";
			}
		}
	}
	else if (eType == EValueType::Real) {
		double* pDouble = (double*)pArray;
		for (int i = 0; i < count; i++) {
			sArray += WStr::Format(L"%f", pDouble[i]);
			if (i < count - 1) {
				sArray += L",";
			}
		}
	}
	else if (eType == EValueType::String) {
		CString* pString = (CString*)pArray;

		for (int i = 0; i < count; i++) {
			sArray += L"\"" + pString[i] + L"\"";;
			if (i < count - 1) {
				sArray += L",";
			}
		}
	}
	else {
		DEBUG_STOP;
	}

	sArray += L"]";
	if (bContinue) {
		sArray += L",";
	}

	return sArray;
}

#pragma endregion //:REGION
