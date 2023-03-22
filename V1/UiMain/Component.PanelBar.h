#pragma once

#include "Component.h"
#include "Control.TabWnd.h"



namespace Component
{
	class PanelBar : public CBCGPDockingControlBar
	{
	public:

		PanelBar();

		virtual ~PanelBar();

		bool Initialize(CWnd* pMainFrame);

		void ViewChanged(Control::TabWnd* pTabs);

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	private:

		Control::TabWnd* m_pActiveTabs = nullptr;
	};
}
