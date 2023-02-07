#pragma once

#include "Component.h"
#include "Window.h"

#include <vector>



namespace Component
{
	class ToolBar : public CWnd
	{
	public:

		ToolBar();

		~ToolBar() override;

		void Initialize(CWnd* pParentWnd, const RECT& rect = {});

		void SetPivot(EPivot pivot, bool expandSize = true);

		void ViewChanged(UINT message, Window::View* pView);

	public:

		CBCGPButton* AddButton(UINT id, bool menu = false);

		void AddButtons(std::vector<UINT> ids);

		void AddSeperator();

		CSize AdjustLayout();

		CPoint AdjustLocation(CSize size);

		CBCGPButton* GetButton(UINT id);

	protected:

		void PostNcDestroy() override;

		afx_msg void OnCommand(UINT id);

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	protected:

		EPivot m_ePivot = EPivot::TopCenter;
		bool m_bExpandSize = false;

		std::vector<CBCGPButton*> m_buttons;

		CBCGPButton* CreateButton(UINT id, bool menu);

		bool IsHorizontal();
	};
}
