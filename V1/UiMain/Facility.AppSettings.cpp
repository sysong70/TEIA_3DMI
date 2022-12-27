#include "stdafx.h"
#include "Facility.AppSettings.h"
#include "Path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::AppSettings TheAppSettings;
Facility::ImportOption* Facility::AppSettings::m_pImportOptionDefault = nullptr;



Facility::AppSettings::~AppSettings()
{
	for (Facility::ImportOption* pOption : m_importOptions) {
		REMOVE_POINTER(pOption);
	}
	m_importOptions.clear();
}



void Facility::AppSettings::SetFolderPath(CString c)
{
	m_sFolderPath = c;
	Path::AddBackslash(m_sFolderPath);
}



bool Facility::AppSettings::Load()
{
	Json::Object data;
	if (Json::Helper::Read(GetFilePath(), data) == false) {
		return false;
	}

	Preference.Set(data.GetValue("Preference").ToObject());
	SetImportOptions(data.GetValue("Import").ToObject());

	return true;
}



bool Facility::AppSettings::Save()
{
	Json::Object data;
	data.SetObject("Import", GetImportOptions());

	return Json::Helper::Write(GetFilePath(), data);
}



Facility::ImportOption* Facility::AppSettings::GetImportOption(CString name)
{
	for (Facility::ImportOption* pOption : m_importOptions) {
		if (pOption->Name == name) {
			return pOption;
		}
	}

	DEBUG_STOP;
	return m_pImportOptionDefault;
}



CString Facility::AppSettings::GetFilePath()
{
	return m_sFolderPath + L"Settings.json";
}



Json::Object* Facility::AppSettings::GetImportOptions()
{
	Json::Object* pData = new Json::Object();

	for (Facility::ImportOption* pOption : m_importOptions) {
		Json::Object* pResult = pOption->Get();
		if (pResult != nullptr) {
			pData->SetObject(CStringA(pOption->Name), pResult);
		}
		else {
			DEBUG_STOP;
		}
	}

	return pData;
}



bool Facility::AppSettings::SetImportOptions(Json::Object* pData)
{
	if (pData == nullptr) {
		RETURN_FALSE;
	}

	for (auto* pMember : pData->GetMembers()) {
		Facility::ImportOption* pOption = new Facility::ImportOption(CString(pMember->Name));
		if (pOption->Set(pMember->pValue->ToObject())) {
			m_importOptions.push_back(pOption);
		}
		else {
			REMOVE_POINTER(pOption);
			RETURN_FALSE;
		}
	}

	m_pImportOptionDefault = GetImportOption(L"_DEFAULT_");

	return true;
}
