#pragma once

#include "3DF.h"
#include "Json.h"
#include <HBaseView.h>

#define GetBooleanValue(name)	pcData->SetBoolean(#name, name)
#define GetEnumValue(name)		pcData->SetInteger(#name, (int)name)
#define GetIntegerValue(name)	pcData->SetInteger(#name, name)
#define GetObjectValue(name)	pcData->SetObject(#name, name.Get())
#define GetRealValue(name)		pcData->SetReal(#name, name)
#define GetStringValue(name)	pcData->SetString(#name, name)

#define SetBooleanValue(name)		name = pcData->GetBoolean(#name, name)
#define SetEnumValue(name, type)	name = (type)pcData->GetInteger(#name, (int)name)
#define SetIntegerValue(name)		name = pcData->GetInteger(#name, name)
#define SetObjectValue(name)		name.Set(&pcData->GetAt(#name))
#define SetRealValue(name)			name = pcData->GetReal(#name, name)
#define SetStringValue(name)		name = pcData->GetString(#name, name)

OPEN_3DF_NAMESPACE

namespace Facility
{
	class Base
	{
	public:

		Base(CString name = L"");

		virtual ~Base();

	public:

		// return new Json::Object
		virtual Json::Object* Get() = 0;

		virtual bool Set(Json::Object* pData) = 0;

		CString Name;
	};
}

CLOSE_3DF_NAMESPACE

//#undef GetBooleanValue
//#undef GetEnumValue
//#undef GetIntegerValue
//#undef GetObjectValue
//#undef GetRealValue
//#undef GetStringValue
//
//#undef SetBooleanValue
//#undef SetEnumValue
//#undef SetIntegerValue
//#undef SetObjectValue
//#undef SetRealValue
//#undef SetStringValue
