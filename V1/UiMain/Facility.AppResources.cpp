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

	// Dialogs/FileOptions
	Json::Object& fileOptions = m_ui.GetAt("Dialogs").GetAt("FileOptions");
	if (fileOptions.FindValue("properties") != nullptr) {
		return true;
	}

	// Dialogs/FileOptions/common
	Json::Array& common = fileOptions.GetArray("common");
	// Dialogs/FileOptions/special
	Json::Object& special = fileOptions.GetAt("special");
	// Dialogs/FileOptions/tree/0(Import)/items
	Json::Array& importTree = Json::Helper::FindValueByPath(fileOptions, "tree/0/items")->AsArray();

	// create object - Dialogs/FileOptions/properties
	Json::Object& newProp = fileOptions.CreateObject("properties");
	// create object - Dialogs/FileOptions/properties/Import
	Json::Object& newImportProp = newProp.CreateObject("Import");

	for (auto item : importTree.GetBuffer()) {
		// {"name":"3MF", "title":"3MF", "ext":["3MF"], "group":[1,0,1,0,0], "General":["ReadingMode"]}
		Json::Object& treeItem = item->AsObject();
		if (treeItem.GetBoolean("visible", true) == false) {
			continue;
		}

		// "group":[1,0,1,0,0]
		Json::Array& group = treeItem.GetArray("group");
		// "name":"3MF"
		CString name = treeItem.GetString("name");

		// create object - Dialogs/FileOptions/properties/Import/3MF
		Json::Object& root = newImportProp.CreateObject((CStringA)name);
		root.SetString("type", L"root");
		// create array - Dialogs/FileOptions/properties/Import/3MF/items
		Json::Array& propItems = root.CreateArray("items");

		// add common category - from Dialogs/FileOptions/common, exclude last one(special)
		for (int i = 0; i < group.GetSize() - 1; i++) {
			// 1:visible
			if (group.GetInteger(i) != 0) {
				// append cateogry
				propItems.AddValue(common.GetAt(i)->DeepCopy());
			}
		}

		// add special cateogry - from Dialogs/FileOptions/special
		if (group.GetInteger(group.GetSize() - 1) != 0) {
			// find category - Dialogs/FileOptions/special/3MF
			Json::Object& target = special.GetAt((CStringA)name);
			target.SetString("name", L"Special");
			target.SetString("title", treeItem.GetString("title"));

			// append cateogry
			CString buffer;
			target.Stringify(buffer);
			propItems.AddObject(*(new Json::Object(buffer)));
		}

		// enable check

		std::vector<CStringA> categories = {
			"General",
			"Tessellation",
			"Search",
			"PMI",
			"Special"
		};

		int catIndex = 0;
		for (auto category : categories) {
			if (treeItem.FindValue(category) == nullptr ||
				treeItem.GetArray(category).GetSize() == 0) {
				continue;
			}

			Json::Array& enables = treeItem.GetArray(category);
			Json::Array& general = propItems.GetObject(catIndex).GetArray("items");

			for (auto enable : enables.GetBuffer()) {
				int index = enable->AsInteger();
				Json::Object& target = general.GetObject(index);
				target.SetBoolean("enable", false);
			}

			catIndex++;
		}
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

	return true;
}
