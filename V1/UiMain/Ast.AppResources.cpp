#include "stdafx.h"

#include "Ast.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

AstAppResources TheAppResources;

//**************************************************************************************************

AstAppResources::~AstAppResources()
{
	if (BkHandle != nullptr) {
		::DeleteObject(BkHandle);
		BkHandle = nullptr;
	}
}



bool AstAppResources::Load()
{
#define LoadSub(x) if (Init##x() == false) { return false; }

	LoadSub(Dialogs);
	LoadSub(Dictionary);
	LoadSub(Tasks);
	LoadSub(Images);

	LoadSub(FileOptions);
	LoadSub(Preferences);

	if (Initialize() == false) {
		return false;
	}

	return true;

#undef LoadSub
}

//--------------------------------------------------------------------------------------------------

Json::Object& AstAppResources::GetDialog(CStringA name)
{
	return Dialogs.GetAt(name);
}



Json::Object& AstAppResources::GetStyles()
{
	return UI.GetAt("Styles");
}



CString AstAppResources::GetStringFrom(Json::Value* pValue, CString dictionary)
{
	CString value;

	if (pValue->IsInteger()) {
		CStringA code = (CStringA)WStr::Format(
			WStr::Format(L"%%0%dd", Ast::CodeLength),
			pValue->AsInteger()
		);

		if (dictionary.IsEmpty() == false) {
			code = (CStringA)dictionary + "." + code;
		}
		else {
			code = (CStringA)pValue->ToString();
		}

		value = Dictionary.GetString(code);
	}
	else {
		value = pValue->AsString();

		// Special case - pass dictionary prefix
		if (value.GetAt(1) == L'.') {
			switch (value.GetAt(0)) {
				case L'C': // Common
				case L'E': // Error
				case L'K': // Keyword
				case L'M': // Message
					value = Dictionary.GetString((CStringA)value);
					break;

				default:
					DEBUG_STOP;
					break;
			}
		}
	}

	value.Replace(L"<br>", L"\n");

	return Ast::Local(value);
}



Json::Object& AstAppResources::GetTask(CStringA name)
{
	return Tasks.GetAt(name);
}



bool AstAppResources::Initialize()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_UI, stream) == false ||
		Json::Helper::Load(stream, UI) == false) {
		RETURN_FALSE;
	}

	// Dialogs/FileOptions
	Json::Object& fileOptions = Dialogs.GetAt("FileOptions");
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

	// TODO
	//Json::Array& exportTree = Json::Helper::FindValueByPath(fileOptions, "tree/1/items")->AsArray();

	return true;
}



bool AstAppResources::InitDialogs()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_DIALOGS, stream) == false ||
		Json::Helper::Load(stream, Dialogs) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool AstAppResources::InitDictionary()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_DICTIONARY, stream) == false ||
		Json::Helper::Load(stream, Dictionary) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool AstAppResources::InitTasks()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_TASKS, stream) == false ||
		Json::Helper::Load(stream, Tasks) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool AstAppResources::InitImages()
{
	CBCGPPngImage image;
	image.m_bUseBackground = FALSE;
	image.Load(IDF_PNG_BACKGROUND);
	BkHandle = (HBITMAP)image.Detach();

	if (BkHandle == nullptr) {
		RETURN_FALSE;
	}
	else {
		return true;
	}
}



bool AstAppResources::InitFileOptions()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_DATA_FILEOPTIONS, stream) == false ||
		Json::Helper::Load(stream, FileOptions) == false) {
		RETURN_FALSE;
	}

	return true;
}



bool AstAppResources::InitPreferences()
{
	CString stream;
	if (Ast::LoadTextResource(IDF_JSON_DATA_PREFERENCES, stream) == false ||
		Json::Helper::Load(stream, Preferences) == false) {
		RETURN_FALSE;
	}

	return true;
}
