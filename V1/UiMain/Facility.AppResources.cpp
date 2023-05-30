#include "stdafx.h"
#include "resource.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




Facility::AppResources TheAppResources;




Facility::AppResources::~AppResources()
{
}



bool Facility::AppResources::Load()
{
	if (Initialize() == false) {
		return false;
	}

	if (InitPreferences() == false) {
		return false;
	}

	if (InitFileOptions() == false) {
		return false;
	}

	return true;
}



Json::Object& Facility::AppResources::GetDialog(CStringA name)
{
	return m_ui.GetAt("Dialog").GetAt(name);
}



Json::Object& Facility::AppResources::GetPreferences()
{
	return m_preferences;
}



Json::Object& Facility::AppResources::GetFileOptions()
{
	return m_fileOptions;
}



Json::Object& Facility::AppResources::GetStyles()
{
	return m_ui.GetAt("Styles");
}



bool Facility::AppResources::Initialize()
{
	CString stream;
	if (LoadTextResource(IDF_JSON_UI, stream) == false ||
		Json::Helper::Load(stream, m_ui) == false) {
		RETURN_FALSE;
	}

	Json::Object& fileOptions = m_ui.GetAt("Dialog").GetAt("FileOptions");
	if (fileOptions.FindValue("properties") != nullptr) {
		return true;
	}

	Json::Object& prop = fileOptions.CreateObject("properties");
	Json::Object& importProp = prop.CreateObject("Import");

	CString commonStream;
	Json::Array& common = fileOptions.GetArray("__DEFAULT__");
	common.Stringify(commonStream);

	Json::Array& importTree = fileOptions.GetArray("tree")[0]->AsObject().GetArray("items");
	for (auto item : importTree.GetBuffer()) {
		CString name = item->AsObject().GetString("name");

		Json::Object& sub = importProp.CreateObject((CStringA)name);
		sub.SetString("type", L"root");
		sub.SetArray("items", new Json::Array(commonStream));
	}

	return true;
}



bool Facility::AppResources::InitPreferences()
{
	CString stream;
	if (LoadTextResource(IDF_JSON_DATA_PREFERENCES, stream) == false ||
		Json::Helper::Load(stream, m_preferences) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool Facility::AppResources::InitFileOptions()
{
	CString stream;
	if (LoadTextResource(IDF_JSON_DATA_FILEOPTIONS, stream) == false ||
		Json::Helper::Load(stream, m_fileOptions) == false) {
		RETURN_FALSE;
	}

	Json::Object& import = m_fileOptions.GetAt("Import");
	Json::Object& common = import.GetAt("__DEFAULT__");
	if (common.IsEmpty()) {
		return true;
	}

	Json::Array& importTree = Json::Helper::FindValueByPath(m_ui, "Dialog/FileOptions/tree/0/items")->AsArray();
	for (auto item : importTree.GetBuffer()) {
		CStringA name = (CStringA)item->AsObject().GetString("name");
		import.CreateObject(name) = common;
	}

	import.Remove("__DEFAULT__");

	return true;
}
