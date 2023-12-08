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
	if (m_background != nullptr) {
		::DeleteObject(m_background);
		m_background = nullptr;
	}
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

	if (InitImages() == false) {
		return false;
	}

	return true;
}



HBITMAP Facility::AppResources::GetBackground()
{
	return m_background;
}



Json::Object& Facility::AppResources::GetDialog(CStringA name)
{
	return m_ui.GetAt("Dialogs").GetAt(name);
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



Json::Object& Facility::AppResources::GetTask(CStringA name)
{
	return m_ui.GetAt("Tasks").GetAt(name);
}



bool Facility::AppResources::Initialize()
{
	CString stream;
	if (LoadTextResource(IDF_JSON_UI, stream) == false ||
		Json::Helper::Load(stream, m_ui) == false) {
		RETURN_FALSE;
	}

	Json::Object& fileOptions = m_ui.GetAt("Dialogs").GetAt("FileOptions");
	if (fileOptions.FindValue("properties") != nullptr) {
		return true;
	}

	Json::Object& prop = fileOptions.CreateObject("properties");
	Json::Object& importProp = prop.CreateObject("Import");

	CString commonStream;
	Json::Array& common = fileOptions.GetArray("__DEFAULT__");
	common.Stringify(commonStream);

	Json::Array& importTree = Json::Helper::FindValueByPath(fileOptions, "tree/0/items")->AsArray();
	for (auto item : importTree.GetBuffer()) {
		CString name = item->AsObject().GetString("name");

		Json::Object& sub = importProp.CreateObject((CStringA)name);
		sub.SetString("type", L"root");
		sub.SetArray("items", new Json::Array(commonStream));
	}

	//:TODO
	//Json::Array& exportTree = Json::Helper::FindValueByPath(fileOptions, "tree/1/items")->AsArray();

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

bool Facility::AppResources::InitImages()
{
	CBCGPPngImage image;
	image.m_bUseBackground = FALSE;
	image.Load(IDF_PNG_BACKGROUND);
	m_background = (HBITMAP)image.Detach();

	if (m_background == nullptr) {
		RETURN_FALSE;
	}
	else {
		return true;
	}
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

	Json::Array& importTree = Json::Helper::FindValueByPath(m_ui, "Dialogs/FileOptions/tree/0/items")->AsArray();
	for (auto item : importTree.GetBuffer()) {
		CStringA name = (CStringA)item->AsObject().GetString("name");
		import.CreateObject(name) = common;
	}

	import.Remove("__DEFAULT__");

	return true;
}
