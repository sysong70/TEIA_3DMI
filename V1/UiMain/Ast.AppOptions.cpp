#include "stdafx.h"

#include "Ast.AppOptions.h"
#include "Ast.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	const CString PreferencesName = L"Preferences.Json";
	const CString FileOpeionsName = L"FileOptions.Json";
}

AstAppOptions TheAppOptions;

//**************************************************************************************************

void AstAppOptions::SetFolderPath(CString c)
{
	FolderPath = c;
	Path::AddBackslash(FolderPath);
}



bool AstAppOptions::Load()
{
	const CString PreferencesPath = FolderPath + PreferencesName;
	const CString FileOptionsPath = FolderPath + FileOpeionsName;

	// Preferences

	if (File::IsExist((LPCTSTR)PreferencesPath)) {
		Preferences.Clean();
		if (Json::Helper::Read(PreferencesPath, Preferences) == false) {
			Preferences = TheAppResources.Preferences;
		}
		else {
			if (Preferences.GetString("version") != TheAppResources.Preferences.GetString("version")) {
				// TODO - remove file
				Preferences = TheAppResources.Preferences;
			}
		}
	}
	else {
		Preferences = TheAppResources.Preferences;
	}

	// FileOptions

	if (File::IsExist((LPCTSTR)FileOptionsPath)) {
		FileOptions.Clean();
		if (Json::Helper::Read(FileOptionsPath, FileOptions) == false) {
			FileOptions = TheAppResources.FileOptions;
		}
		else {
			if (FileOptions.GetString("version") != TheAppResources.FileOptions.GetString("version")) {
				// TODO - remove file
				FileOptions = TheAppResources.FileOptions;
			}
		}
	}
	else {
		FileOptions = TheAppResources.FileOptions;
	}

	return true;
}



bool AstAppOptions::Save()
{
#ifdef _DEBUG
	bool serialize = true;
#else
	bool serialize = false;
#endif

	if (Json::Helper::Write(FolderPath + PreferencesName, Preferences) == false) {
		RETURN_FALSE;
	}
	if (Json::Helper::Write(FolderPath + FileOpeionsName, FileOptions) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool AstAppOptions::GetBoolean(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(Preferences, path);
	if (pValue != nullptr) {
		return pValue->AsBoolean();
	}
	else {
		RETURN_FALSE;
	}
}



COLORREF AstAppOptions::GetColor(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(Preferences, path);
	if (pValue != nullptr) {
		return Json::Helper::ToColor(pValue->AsString());
	}
	else {
		RETURN(-1);
	}
}



int AstAppOptions::GetInteger(const char* path)
{
	Json::Value* pValue = Json::Helper::FindValueByPath(Preferences, path);
	if (pValue != nullptr) {
		return pValue->AsInteger();
	}
	else {
		RETURN(-1);
	}
}
