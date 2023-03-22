#pragma once

#include <Json.h>



namespace Facility
{
	class AppResource;
	class AppSettings;
	class Base;
	class CommandIndexer;
	class ImportOption;
	class Preference;



	enum class ELanguage
	{
		English = 0,
		Korean,
	};

#pragma region Language

	ELanguage GetLanguage();

	void SetLanguage(ELanguage e);

	CString Local(LPCTSTR pStrings);

	CString Local(CString& strings);

#pragma endregion //:REGION

#pragma region Resource

	HBITMAP CreateBitmap(UINT id, const CSize& size);

	HICON CreateIcon(UINT id, const CSize& size);

	void GetResource(UINT id, CString& title, CString& tooltip);

	CString GetTitle(UINT id);

	CString GetTooltip(UINT id);

	bool LoadTextResource(UINT id, CString& result);

#pragma endregion //:REGION

#pragma region Json

	CString GetDescription(Json::Object& source);

	UINT GetId(Json::Object& source);

	Json::Array* GetItems(Json::Object& source);

	void GetItems(Json::Object& source, std::vector<CString>& items);

	CString GetName(Json::Object& source);

	int GetPivot(Json::Object& source);

	CSize GetSize(Json::Object& source);

	CRect GetRect(Json::Object& source);

	CString GetTitle(Json::Object& source);

	CString GetType(Json::Object& source);

	Json::Object& SetData(Json::Object& target, UINT id = 0, const CString& title = L"", int ePivot = 0, const CRect& rect = {});

	void SetValue(Json::Value& target, _variant_t& source);

	void SetValueByPath(Json::Object& object, CString& path, _variant_t& value);

#pragma endregion //:REGION
}
