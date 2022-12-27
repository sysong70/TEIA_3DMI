#pragma once

namespace Facility
{
	class Base;

	class AppResource;
	class AppSettings;
	class CommandIndexer;
	class FileOption;
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

#pragma endregion //:REGION
}
