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

	// Preferences

	if (File::IsExist((LPCTSTR)PreferencesPath)) {
		m_preferences.Clean();
		if (Json::Helper::Read(PreferencesPath, m_preferences) == false) {
			m_preferences = TheAppResources.GetPreferences();
		}
		else {
			if (m_preferences.GetString("version") != TheAppResources.GetPreferences().GetString("version")) {
				//:TODO - remove file
				m_preferences = TheAppResources.GetPreferences();
			}
		}
	}
	else {
		m_preferences = TheAppResources.GetPreferences();
	}

	// FileOptions

	if (File::IsExist((LPCTSTR)FileOptionsPath)) {
		m_fileOptions.Clean();
		if (Json::Helper::Read(FileOptionsPath, m_fileOptions) == false) {
			m_fileOptions = TheAppResources.GetFileOptions();
		}
		else {
			if (m_fileOptions.GetString("version") != TheAppResources.GetFileOptions().GetString("version")) {
				//:TODO - remove file
				m_fileOptions = TheAppResources.GetFileOptions();
			}
		}
	}
	else {
		m_fileOptions = TheAppResources.GetFileOptions();
	}

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



bool Facility::AppOptions::GetBoolean(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(m_preferences, path);
	if (pValue != nullptr) {
		return pValue->AsBoolean();
	}
	else {
		RETURN_FALSE;
	}
}



COLORREF Facility::AppOptions::GetColor(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(m_preferences, path);
	if (pValue != nullptr) {
		return Json::Helper::ToColor(pValue->AsString());
	}
	else {
		RETURN(-1);
	}
}



int Facility::AppOptions::GetInteger(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(m_preferences, path);
	if (pValue != nullptr) {
		return pValue->AsInteger();
	}
	else {
		RETURN(-1);
	}
}

#undef PRESET
