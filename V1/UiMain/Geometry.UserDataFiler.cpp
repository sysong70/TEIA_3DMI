#include "stdafx.h"
#include "Geometry.UserDataFiler.h"
#include <WStr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

Geometry::UserDataFiler::UserDataFiler()
{
}



Geometry::UserDataFiler::~UserDataFiler()
{
}

//--------------------------------------------------------------------------------------------------

bool Geometry::UserDataFiler::GetBoolean(int id, bool defaultValue)
{
	return __super::GetBoolean((CStringA)WStr::ToString(id), defaultValue);
}



DWORD_PTR Geometry::UserDataFiler::GetDwordPtr(int id, DWORD_PTR defaultValue)
{
	return __super::GetDwordPtr((CStringA)WStr::ToString(id), defaultValue);
}



int Geometry::UserDataFiler::GetInteger(int id, int defaultValue)
{
	return __super::GetInteger((CStringA)WStr::ToString(id), defaultValue);
}



double Geometry::UserDataFiler::GetReal(int id, double defaultValue)
{
	return __super::GetReal((CStringA)WStr::ToString(id), defaultValue);

}



CString Geometry::UserDataFiler::GetString(int id, CString defaultValue)
{
	return __super::GetString((CStringA)WStr::ToString(id), defaultValue);
}



void Geometry::UserDataFiler::SetBoolean(int id, bool value)
{
	//__super::SetBoolean((CStringA)WStr::ToString(id), value);
	GetValue((CStringA)WStr::ToString(id)) = value;
}



void Geometry::UserDataFiler::SetDwordPtr(int id, DWORD_PTR value)
{
	//__super::SetDwordPtr((CStringA)WStr::ToString(id), value);
	CString cast;
	cast.Format(L"%llx", value);
	GetValue((CStringA)WStr::ToString(id)) = cast;
}



void Geometry::UserDataFiler::SetInteger(int id, int value)
{
	//__super::SetInteger((CStringA)WStr::ToString(id), value);
	GetValue((CStringA)WStr::ToString(id)) = value;
}



void Geometry::UserDataFiler::SetReal(int id, double value)
{
	//__super::SetReal((CStringA)WStr::ToString(id), value);
	GetValue((CStringA)WStr::ToString(id)) = value;
}



void Geometry::UserDataFiler::SetString(int id, CString value)
{
	//__super::SetString((CStringA)WStr::ToString(id), value);
	GetValue((CStringA)WStr::ToString(id)) = value;
}
