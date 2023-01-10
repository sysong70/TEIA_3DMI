#include "stdafx.h"
#include "resource.h"
#include "Facility.AppSettings.h"
#include "Path.h"
#include "Dir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Facility::AppSettings TheAppSettings;

#define PRESET PresetAppSettings

namespace PRESET
{
	const CString PreferencesName = L"Preferences.Json";
	const CString FileOpeionsName = L"FileOptions.Json";
}



Facility::AppSettings::AppSettings()
{
}



Facility::AppSettings::~AppSettings()
{
}



void Facility::AppSettings::SetFolderPath(CString c)
{
	m_sFolderPath = c;
	Path::AddBackslash(m_sFolderPath);
}



Json::Object& Facility::AppSettings::GetPreferences()
{
	return m_preferences;
}



Json::Object& Facility::AppSettings::GetFileOptions()
{
	return m_fileOptions;
}



bool Facility::AppSettings::Load()
{
	const CString PreferencesPath = m_sFolderPath + PRESET::PreferencesName;
	const CString FileOptionsPath = m_sFolderPath + PRESET::FileOpeionsName;

	if (Dir::IsExist((LPCTSTR)PreferencesPath)) {
		m_preferences.Clean();
		if (Json::Helper::Read(PreferencesPath, m_preferences) == false) {
			return false;
		}
	}
	if (Dir::IsExist((LPCTSTR)FileOptionsPath)) {
		m_fileOptions.Clean();
		if (Json::Helper::Read(FileOptionsPath, m_fileOptions) == false) {
			return false;
		}
	}

	return true;
}



bool Facility::AppSettings::Save()
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
