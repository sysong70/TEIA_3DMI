#pragma once

#include "Control.h"
#include "Window.h"
#include <vector>



namespace Control
{
	class ToolBar : public CWnd
	{
	public:

		enum class EItemSize
		{
			Small,
			Medium,
			Large,
		};

		ToolBar();

		~ToolBar() override;

		bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

		void SetPivot(EPivot pivot, bool expandSize = true);

	public:

		CBCGPButton* AddButton(UINT id, bool menu = false, bool toggle = false);

		void AddButtons(std::vector<UINT> ids);

		void AddSeperator();

		CSize AdjustLayout();

		CPoint AdjustLocation(CSize size);

		CBCGPButton* GetButton(UINT id);

		bool GetCheck(UINT id);

		void SetCheck(UINT id, bool value, bool uncheckOthers = true);

		void SetSize(CSize buttonSize, CSize buttonMargin, CSize imageSize, CSize seperatorMargin, CSize toolBarPadding);
		// image size, not button size
		void SetSize(EItemSize size);

		void SetToggle(UINT id);

	protected:

		afx_msg void OnCommand(UINT id);

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	protected:

		EPivot m_ePivot = EPivot::TopCenter;
		bool m_bExpandSize = false;

		std::vector<CBCGPButton*> m_buttons;

		CBCGPButton* CreateButton(UINT id, bool menu, bool toggle);

		bool IsHorizontal();

	protected:

		CSize m_buttonSize = CSize(28, 28);
		CSize m_buttonMargin = CSize(2, 2);
		CSize m_imageSize = CSize(24, 24);
		CSize m_seperatorMargin = CSize(6, 6);
		CSize m_toolBarPadding = CSize(3, 3);

		CSize GetButtonSize();

		CSize GetButtonMargin();

		CSize GetImageSize();

		CSize GetSeperatorMargin();

		CSize GetPadding();
	};
}
