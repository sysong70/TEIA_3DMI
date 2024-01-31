#pragma once

#include "Control.h"
#include "Control.ToolBar.h"
#include <unordered_map>



namespace Control
{
	class Panel : public CWnd
	{
	public:

		Panel();

		~Panel() override;

		bool Initialize(CWnd* pParentWnd, Window::View* pView, UINT id);

		Window::View* GetParentView();

	public:

		virtual void AdjustLayout(int cx, int cy);

		virtual void ConstructBody();

		virtual int ConstructFooter(int cx);
		// Construct toolbar
		virtual int ConstructHeader(int cx);
		// exclude margin
		virtual CRect GetBodyRect();
		// include margin
		virtual CSize GetBodySize();

	protected:

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	protected:

		Window::View* m_pView = nullptr;

		ToolBar m_toolBar;
		int m_nHeaderHeight = 0;
		int m_nFooterHeight = 0;
		CSize m_oldSize; // OnSize()
	};
}
