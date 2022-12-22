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

	protected:

		CRect SetupControl(CBCGPCircularProgressIndicatorCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPListBox& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPListCtrl& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

		CRect SetupControl(CBCGPStatic& control, Json::Object& data, Component::EPivot ePivot, CRect rect);

	protected:

		CRect AdjustLayout(CWnd* pControl, CSize maxSize, Component::EPivot ePivot, CRect boundary);

		CSize AdjustSize(CWnd* pControl, CSize size);

		CRect AlignControl(CWnd* pControl, Component::EPivot ePivot, CRect boundary);
		// return total boundary
		CRect AlignControls(Controls controls, Component::EAlign eAlign, CPoint startPoint);
		// return total boundary
		CRect DistributeControls(Controls controls, Component::EAlign eBase, CPoint startPoint, int gap);

		void GetListItems(Json::Object& data, std::vector<CString>& list);

	protected:

		CRect GetClientArea();
		// boundary on client area
		CRect GetControlRect(CWnd* pControl);

		CSize GetControlSize(CWnd* pControl);

	protected:

		// dialog structure information from TheResource
		Json::Object* m_pDialogData;
	};
}
