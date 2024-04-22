#pragma once
#pragma warning(disable : 4244 4251 4996)

#include "Common_Define.h"

//--------------------------------------------------------------------------------------------------

namespace Json
{
	class Array;
	class Value;
	class Pair;
	class Object;

#ifdef _DEBUG
	void UnitTest();
#endif

	enum class EValueType
	{
		Null = 0,	// null value
		Boolean,	// bool value
		Int,		// signed integer value
		Uint,		// unsigned integer value
		Real,		// double value
		String,		// UTF-8 string value
		Array,		// array value (ordered list)
		Object,		// object value (collection of name/value pairs).
		Unknown,
	};



	class Array
	{
	public:

		Array();

		Array(CString& sourceStream);

		Array(const Array& other);

		virtual ~Array();

	public:

		Array& operator =(const Array& other);
		//:WARNING - hard compare by string
		bool operator ==(Array& other);
		bool operator !=(Array& other);
		//:WARNING - contents compare (ignore order)
		bool Compare(Array& other);

	public:

		int GetSize();

		std::vector<Value*>& GetBuffer();

	public:

		Value* GetAt(int i);

		Value* operator [](int i);

	public: //:WARNING - no type check

		Array& GetArray(int i);

		bool GetBoolean(int i);

		int GetInteger(int i);

		Object& GetObject(int i);

		double GetReal(int i);
		// sysong: 2024-04-18 추가
		double GetRealRaw(int i);

		CString& GetString(int i);

	public:

		void AddBoolean(bool value);

		void AddInteger(int value);

		void AddReal(double value);
		// sysong: 2024-04-18 추가
		void AddRealRaw(double value);

		// add string value and return reference
		CString& AddString();

		void AddString(CString value);
		// add object value and return reference
		Object& AddObject();

		void AddObject(Object& pValue);
		// standard type
		void AddValue(Value* pValue);

	public:

		//:WARNING - special
		void AddDwordPtr(DWORD_PTR value);

		DWORD_PTR GetDwordPtr(int i);

	public: // casting

		bool ToArray(int& count, int*& pValues);

		bool ToArray(WStringArray& values);

		bool ToArray(std::vector<DWORD_PTR>& values);

	public:

		void Clean();
		// pretty format
		void Serialize(CString& buffer, int indent);
		// compact format
		void Stringify(CString& buffer);

	protected:

		std::vector<Value*> m_buffer;
	};



	class Value
	{
	public:

		Value();

		Value(int value);

		Value(DWORD value);

		Value(double value);

		Value(bool value);

		Value(CString value);

		Value(CString number, bool bReal);

		Value(Array& arrayData);

		Value(Object& objectData);

		Value(const Value& other);

		~Value();

	public:

		//:CHECK
		Value& operator =(const Value& other);
		Value& operator =(bool value);
		Value& operator =(int value);
		Value& operator =(double value);
		Value& operator =(CString value);
		//:WARNING - hard compare by string
		bool operator ==(Value& other);
		bool operator !=(Value& other);
		//:WARNING - contents compare (ignore order)
		bool Compare(Value& other);

	public: // get value

		EValueType GetType();

		bool IsValid();

		bool IsArray();

		bool IsBoolean();

		bool IsInteger();

		bool IsNumber();

		bool IsObject();

		bool IsReal();

		bool IsString();

	public: //:WARNING - check type and return forced conversion value

		Array* ToArray();

		bool ToBoolean();

		int ToInteger();

		Object* ToObject();

		double ToReal();

		CString ToString();

	public: //:WARNING - no type check

		Array& AsArray();

		bool AsBoolean();

		int AsInteger();

		Object& AsObject();

		double AsReal();

		CString& AsString();

	public: // create and return reference

		Array& CreateArray();

		Object& CreateObject();

		CString& CreateString();

	public: // set value

		void SetArray(Array* pArray);

		void SetBoolean(bool value);

		void SetInteger(int value);

		void SetNull();

		void SetNumber(CString& value, bool real);

		void SetObject(Object* pObject);

		void SetReal(double value);

		void SetString(const CString& value);

	public:

		void Clean();

		void Initialize();

		Value* DeepCopy();
		// pretty format
		void Serialize(CString& buffer, int indent);
		// compact format
		void Stringify(CString& buffer);

	protected:

		EValueType m_eType;

		union ValueHolder
		{
			bool vBoolean;
			int vInteger;
			double vReal;
			CString* vString;
			Array* vArray;
			Object* vObject;
		}
		m_valueHolder;
	};



	class Pair
	{
	public:

		Pair();

		Pair(CStringA name);

		Pair(CStringA name, Value* p);

		~Pair();

	public:

		CStringA Name;

		Value* pValue;
	};



	class Object
	{
	public:

		Object();

		Object(CString& sourceStream);

		Object(const Object& other);

		virtual ~Object();

	public:

		Object& operator =(const Object& other);
		//:WARNING - hard compare by string
		bool operator ==(Object& other);
		bool operator !=(Object& other);
		//:WARNING - contents compare (ignore order)
		bool Compare(Object& other);

		Value& operator [](const char* name);

	public:

		// return new array
		Array& CreateArray(CStringA name);
		// return new object
		Object& CreateObject(CStringA name);
		// return new string
		CString& CreateString(CStringA name);

	public: // get child value, use only when certain or FindValue()

		// use when uncertain
		Value* FindValue(CStringA name);

		Array& GetArray(CStringA name);
		// alias of GetObject()
		Object& GetAt(CStringA name);

		Object& GetObject(CStringA name);

		Value& GetValue(CStringA name);

	public: // get child single value. direct access

		bool GetBoolean(CStringA name, bool defaultValue = false);

		int GetInteger(CStringA name, int defaultValue = 0);

		double GetReal(CStringA name, double defaultValue = 0.0);

		CString GetString(CStringA name, CString defaultValue = L"");

	public: // set single value. if not exist, create new one

		void SetArray(CStringA name, Array* pValue);

		void SetBoolean(CStringA name, bool value);

		void SetInteger(CStringA name, int value);

		void SetObject(CStringA name, Object* pValue);

		void SetReal(CStringA name, double value);

		void SetString(CStringA name, CString value);
		// standard type
		void SetValue(CStringA name, Value* pValue, bool deepCopy = false);

	public:

		DWORD_PTR GetDwordPtr(CStringA name, DWORD_PTR defaultValue = 0);

		void SetDwordPtr(CStringA name, DWORD_PTR value);

	public:

		void Clean();

		bool IsEmpty();

		void Remove(const char* pName);

		std::vector<Pair*>& GetMembers();

		void GatherNames(AStringArray& names);

		void GatherNames(WStringArray& names);

		// pretty format
		void Serialize(CString& buffer, int indent);
		// compact format
		void Stringify(CString& buffer);

		CString ToString();

	protected:

		Pair* Look(const char* pName) const;

		bool Look(const char* pName, Value*& pValue) const;

		std::vector<Pair*> m_members;
	};



	namespace Reader
	{
		bool ReadArray(wchar_t*& pStream, Array& arrayData);

		bool ReadBoolean(wchar_t*& pStream, bool& value);

		bool ReadNumber(wchar_t*& pStream, CString& value, bool& bReal);

		bool ReadObject(wchar_t*& pStream, Object& objectData);

		bool ReadString(wchar_t*& pStream, CString& value);

		bool ReadValue(wchar_t*& pStream, Value& value);

		bool SkipNull(wchar_t*& pStream);
	}



	namespace Helper
	{
		bool Load(CString& value, Object& object);

		bool Load(CString& value, Array& array);

		bool Load(const wchar_t* pValue, Object& object);

		bool Load(const wchar_t* pValue, Array& array);

		bool Read(CString path, Object& object);

		bool Write(CString path, Object& object, bool serialize = true);

		// object/value, do not check last object
		Value* FindValueByPath(Json::Object& object, CStringA path);
		//:SAMPLE - FindObjectByPath("Dialog/FileOptions/Import/ACIS")
		Object* FindObjectByPath(Json::Object& object, CStringA path);

		CStringA GetIdString(UINT id);

		// string to number

		DWORD_PTR GetDwordPtr(Object& object, UINT id, DWORD_PTR defaultValue = 0);

		LONGLONG GetLongLong(Object& object, UINT id, LONGLONG defaultValue = 0);

		double GetReal(Object& object, UINT id, double defaultValue = 0.0);

		double GetRealRawString(Object& object, UINT id, double defaultValue = 0.0);

		CString GetString(Object& object, UINT id, CString defaultValue = L"");

		void SetArray(Object& object, UINT id, Array* pValue);

		void SetBoolean(Object& object, UINT id, bool value);

		void SetDwordPtr(Object& object, UINT nId, DWORD_PTR value);

		void SetInteger(Object& object, UINT id, int value);

		void SetLongLong(Object& object, UINT id, LONGLONG value);

		void SetObject(Object& object, UINT id, Object* pValue);

		void SetReal(Object& object, UINT id, double value);

		void SetRealRawString(Object& object, UINT id, double value, bool allDigit = false);

		void SetString(Object& object, UINT id, CString value);

		// Color

		// source: 3 byte Hex value, ex) 0x123456
		COLORREF ToColor(CString value);
		// target: Hex value, only RGB
		CString ToString(COLORREF value);

		// Coordinate

		/*
			array or object type, fill in pTarget
		
			Object source; 
			float coord[3] = {};

			if (GetCoordinate(source.FindValue("POINT1), coord) == true) {
				//:TODO
			}
		*/
		bool GetCoordinate(Value* pSource, float* pTarget);
		/*
			return new value(array or object) of coordinate

			float coord[3] = { 0.1, 0.2, 0.3 };
			Object data;
			data.SetValue("POINT1", GetCoordinate(coord));
		*/
		Value* GetCoordinate(float* pSource, bool useArray = true, bool is3d = true);
	}



	namespace Builder
	{
		// "Solid":true
		CString Encode(const char* pName, bool bValue, bool bContinue = true);
		// "ReadMode":1
		CString Encode(const char* pName, int nValue, bool bContinue = true);
		// "ChordHeightRatio":400.0
		CString Encode(const char* pName, double dValue, bool bContinue = true);
		// "Path":"c:/temp"
		CString Encode(const char* pName, CString sValue, bool bContinue = true);
		// pArray allowed - Boolean(bool*), Int(int*), Uint(unsigned int*), Real(double*), CString(CString*)
		CString Encode(const char* pName, void* pArray, int count, EValueType eType, bool bContinue = true);
	};
}
