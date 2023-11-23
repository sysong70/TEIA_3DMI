#pragma once

#include "3DF.h"

#include "Facility.h"

#define GetBooleanValue(name)	pData->SetBoolean(#name, name)
#define GetEnumValue(name)		pData->SetInteger(#name, (int)name)
#define GetIntegerValue(name)	pData->SetInteger(#name, name)
#define GetObjectValue(name)	pData->SetObject(#name, name.Get())
#define GetRealValue(name)		pData->SetReal(#name, name)
#define GetStringValue(name)	pData->SetString(#name, name)

#define SetBooleanValue(name)		ASSERT(pData->FindValue(#name) != nullptr); name = pData->GetBoolean(#name, name)
#define SetEnumValue(name, type)	ASSERT(pData->FindValue(#name) != nullptr); name = (type)pData->GetInteger(#name, (int)name)
#define SetIntegerValue(name)		ASSERT(pData->FindValue(#name) != nullptr); name = pData->GetInteger(#name, name)
#define SetObjectValue(name)		ASSERT(pData->FindValue(#name) != nullptr); name.Set(&pData->GetAt(#name))
#define SetRealValue(name)			ASSERT(pData->FindValue(#name) != nullptr); name = pData->GetReal(#name, name)
#define SetStringValue(name)		ASSERT(pData->FindValue(#name) != nullptr); name = pData->GetString(#name, name)

namespace H3DF
{
	namespace Facility
	{
		class API_3DF Base
		{
		public:

			Base(CString name = L"");

			virtual ~Base();

		public:

			// return new Json::Object
			virtual Json::Object * Get() = 0;

			virtual bool Set(Json::Object * pData) = 0;

			CString Name;
		};
	}
}