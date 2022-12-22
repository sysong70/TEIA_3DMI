#include "stdafx.h"
#include "resource.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



Facility::AppResources TheAppResources;



Facility::AppResources::~AppResources()
{
}



bool Facility::AppResources::Load()
{
	HRSRC hRes = ::FindResource(nullptr, MAKEINTRESOURCE(IDF_JSON_UI), L"TEXT");
	DWORD dwResourceSize = ::SizeofResource(nullptr, hRes);
	HGLOBAL hGlobal = ::LoadResource(nullptr, hRes);
	LPVOID pData = ::LockResource(hGlobal);

	char* pChar = new char[dwResourceSize + 1];
	memcpy_s(pChar, dwResourceSize, pData, dwResourceSize);
	pChar[dwResourceSize] = _T('\0');

	int nSize = ::MultiByteToWideChar(CP_UTF8, 0, pChar, -1, NULL, 0);
	if (nSize == 0) {
		RETURN_FALSE;
	}

	wchar_t* pWide = new wchar_t[nSize];
	DEBUG_VALID(pWide);

	::MultiByteToWideChar(CP_UTF8, 0, pChar, -1, pWide, nSize);

	CString stream = pWide;

	REMOVE_ARRAY(pChar);
	REMOVE_ARRAY(pWide);
	::FreeResource(hGlobal);

	return Load(stream);
}



bool Facility::AppResources::Load(CString stream)
{
	if (Json::Helper::Load(stream, m_data) == false) {
		return false;
	}

	m_pDialog = &m_data.GetObject("Dialog");
#ifdef _DEBUG
	m_pDebug = &m_data.GetObjectW("Debug");
#endif

	return true;
}



Json::Object& Facility::AppResources::GetDialog(CStringA name)
{
	DEBUG_VALID(m_pDialog);
	return m_pDialog->GetObject(name);
}

#ifdef _DEBUG

Json::Object& Facility::AppResources::GetDebug()
{
	return *m_pDebug;
}

#endif
