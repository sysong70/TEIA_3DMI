#pragma once

#include "Dialog.Base.h"
#include "Component.h"



namespace Dialog
{
	class Standard : public Base
	{
		DECLARE_DYNAMIC(Standard)

	public:

		// dialogName in Json
		Standard(CStringA dialogName, CWnd* pParent = nullptr);

		Standard(UINT nIDTemplate, CStringA dialogName, CWnd* pParent = nullptr);

		~Standard() override;

	public:

		Json::Object& GetUiData();

	protected:

		BOOL OnInitDialog() override;

		DECLARE_MESSAGE_MAP()

	protected: // Create and setup controls

		CSize SetupControl(CBCGPButton& control, Json::Object& data);

		CRect SetupControl(CBCGPCircularProgressIndicatorCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPListBox& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPListCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPStatic& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

	protected:

		// resize and move control
		static CRect AdjustLayout(CWnd* pControl, CRect frame, CSize baseSize, Component::EPivot ePivot);
		// move control
		static CRect AdjustPosition(CWnd* pControl, CRect frame, Component::EPivot ePivot);
		// resize control
		static CSize AdjustSize(CWnd* pControl, CSize baseSize);
		// align controls at base point
		static CRect AlignControls(Controls controls, CPoint basePoint, Component::EAlign eAlign);
		// destribute controls from base point with gap
		static CRect DestributeControls(Controls controls, CPoint basePoint, int gap, Component::EDirection eDir);

	protected: // Set Json data

		static UINT GetId(Json::Object& data);

		static void GetListItems(Json::Object& data, std::vector<CString>& list);

		static Component::EPivot GetPivot(Json::Object& data);

		static CRect GetRect(Json::Object& data);

		static CString GetTitle(Json::Object& data);

		static Json::Object& SetData(Json::Object& data, UINT id = 0, const CString& title = L"", Component::EPivot ePivot = Component::EPivot::Unknown, const CRect& rect = {});

	protected:

		// boundary on client area
		CRect GetControlRect(CWnd* pControl);

	protected:

		// dialog structure information from TheResource
		Json::Object* m_pDialogData;
	};
}
