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

		// align controls at base point
		CRect AlignControls(Controls controls, CPoint basePoint, Component::EAlign eAlign);
		// destribute controls from base point with gap
		CRect DestributeControls(Controls controls, CPoint basePoint, int gap, Component::EDirection eDir);

	protected:

		// boundary on client area
		CRect GetControlRect(CWnd* pControl);

	protected:

		// dialog structure information from TheResource
		Json::Object* m_pDialogData;
	};
}
