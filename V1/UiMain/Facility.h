#pragma once

//--------------------------------------------------------------------------------------------------

namespace Facility
{
	class Base;
	// Command
	// Command.Common.h,
	// Command.Resource.h
	class CommandIndexer;
	// Data
	class AppOptions;
	class AppResource;



	const int CodeLength = 3;



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

#pragma endregion // REGION

#pragma region Resource

	HBITMAP CreateBitmap(UINT id, const CSize& size);

	HICON CreateIcon(UINT id, const CSize& size);

	void GetResource(UINT id, CString& title, CString& tooltip);

	CString GetTitle(UINT id);

	void GetToolBarImages(CBCGPToolBarImages& target, CSize imageSize, const std::vector<UINT>& ids);

	CString GetTooltip(UINT id);

	bool LoadTextResource(UINT id, CString& result);

#pragma endregion // REGION

#pragma region Json

	CString GetDescription(Json::Object& source, CString dictionary = L"");

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

	void SetValue(Json::Value& target, CBCGPProp& source);

	void SetValue(CBCGPProp& target, Json::Value& source);

	void SetValueByPath(Json::Object& object, CString& path, CBCGPProp& value);

#pragma endregion // REGION
}
