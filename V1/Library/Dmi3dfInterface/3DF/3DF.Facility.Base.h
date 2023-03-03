#pragma once

#include "3DF.h"

#include "Json.h"

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