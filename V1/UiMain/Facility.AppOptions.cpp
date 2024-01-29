#include "stdafx.h"
#include "resource.h"
#include "Facility.AppOptions.h"
#include "Facility.AppResources.h"
#include <File.h>
#include <Path.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::AppOptions TheAppOptions;



#define PRESET PresetAppOptions

namespace PresetAppOptions
{
	const CString PreferencesName = L"Preferences.Json";
	const CString FileOpeionsName = L"FileOptions.Json";
}



Facility::AppOptions::AppOptions()
{
}



Facility::AppOptions::~AppOptions()
{
}



void Facility::AppOptions::SetFolderPath(CString c)
{
	m_sFolderPath = c;
	Path::AddBackslash(m_sFolderPath);
}



Json::Object& Facility::AppOptions::GetPreferences()
{
	return m_preferences;
}



Json::Object& Facility::AppOptions::GetFileOptions()
{
	return m_fileOptions;
}



bool Facility::AppOptions::Load()
{
	const CString PreferencesPath = m_sFolderPath + PRESET::PreferencesName;
	const CString FileOptionsPath = m_sFolderPath + PRESET::FileOpeionsName;

	if (File::IsExist((LPCTSTR)PreferencesPath)) {
		m_preferences.Clean();
		if (Json::Helper::Read(PreferencesPath, m_preferences) == false) {
			return false;
		}
	}
	else {
		m_preferences = TheAppResources.GetPreferences();
	}
	ASSERT(m_preferences.GetReal("version") == TheAppResources.GetPreferences().GetReal("version"));

	if (File::IsExist((LPCTSTR)FileOptionsPath)) {
		m_fileOptions.Clean();
		if (Json::Helper::Read(FileOptionsPath, m_fileOptions) == false) {
			return false;
		}
	}
	else {
		m_fileOptions = TheAppResources.GetFileOptions();
	}
	ASSERT(m_fileOptions.GetReal("version") == TheAppResources.GetFileOptions().GetReal("version"));

	return true;
}



bool Facility::AppOptions::Save()
{
#ifdef _DEBUG
	bool serialize = true;
#else
	bool serialize = false;
#endif

	if (Json::Helper::Write(m_sFolderPath + PRESET::PreferencesName, m_preferences) == false) {
		RETURN_FALSE;
	}
	if (Json::Helper::Write(m_sFolderPath + PRESET::FileOpeionsName, m_fileOptions) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool Facility::AppOptions::BooleanValue(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(m_preferences, path);
	if (pValue != nullptr) {
		return pValue->AsBoolean();
	}
	else {
		RETURN_FALSE;
	}
}

#undef PRESET
