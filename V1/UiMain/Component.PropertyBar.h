#pragma once

#include "Component.h"



namespace Component
{
	class PropertyBar : public CBCGPDockingControlBar
	{
	public:

		PropertyBar();

		~PropertyBar() override;

	protected:

		afx_msg LRESULT OnCommandClicked(WPARAM, LPARAM);

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg LRESULT OnMenuItemSelected(WPARAM, LPARAM);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnSetFocus(CWnd* pOldWnd);

		DECLARE_MESSAGE_MAP()

	private:

		void Initialize();

	private:

		CBCGPPropList m_wndControl;
	};
}
