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

namespace PRESET
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

	if (File::IsExist((LPCTSTR)FileOptionsPath)) {
		m_fileOptions.Clean();
		if (Json::Helper::Read(FileOptionsPath, m_fileOptions) == false) {
			return false;
		}
	}
	else {
		m_fileOptions = TheAppResources.GetFileOptions();
	}

	return true;
}



bool Facility::AppOptions::Save()
{
	if (Json::Helper::Write(m_sFolderPath + PRESET::PreferencesName, m_preferences) == false) {
		RETURN_FALSE;
	}
	if (Json::Helper::Write(m_sFolderPath + PRESET::FileOpeionsName, m_fileOptions) == false) {
		RETURN_FALSE;
	}

	return true;
}

#undef PRESET
