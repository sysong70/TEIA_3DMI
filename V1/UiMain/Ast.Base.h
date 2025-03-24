#pragma once

#include "Ast.h"

//--------------------------------------------------------------------------------------------------

#define GetBooleanValue(name)	pData->SetBoolean(#name, name)
#define GetEnumValue(name)		pData->SetInteger(#name, (int)name)
#define GetIntegerValue(name)	pData->SetInteger(#name, name)
#define GetObjectValue(name)	pData->SetObject(#name, name.Get())
#define GetRealValue(name)		pData->SetReal(#name, name)
#define GetStringValue(name)	pData->SetString(#name, name)

#define SetBooleanValue(name)		name = pData->GetBoolean(#name, name)
#define SetEnumValue(name, type)	name = (type)pData->GetInteger(#name, (int)name)
#define SetIntegerValue(name)		name = pData->GetInteger(#name, name)
#define SetObjectValue(name)		name.Set(&pData->GetAt(#name))
#define SetRealValue(name)			name = pData->GetReal(#name, name)
#define SetStringValue(name)		name = pData->GetString(#name, name)

//--------------------------------------------------------------------------------------------------

class AstBase
{
public:

	AstBase(CString name = L"")
		: Name(name)
	{}

	virtual ~AstBase() {}

public:

	// return new Json::Object
	virtual Json::Object* Get() = 0;

	virtual bool Set(Json::Object* pData) = 0;

	CString Name;
};
