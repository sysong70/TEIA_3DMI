#pragma once

#include "Control.h"

//--------------------------------------------------------------------------------------------------

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

	public: // CBCGPButton

		CBCGPButton* AddButton(UINT id, bool menu = false);

		void AddButtons(const ResourceIds& ids);

		void AddSeperator();

	public: // CBCGPButton (toggle)

		CBCGPButton* AddToggle(UINT id, bool checked = false);

		bool GetCheck(UINT id);

		void SetCheck(UINT id, bool value, bool uncheckOthers = true);

		void SetUncheckOthers(UINT id);

	public: // ButtonWithMenu

		CBCGPButton* AddButtonWithMenu(UINT id, const ResourceIds& menus);

		void CheckMenu(UINT id, UINT menuId, bool radioType = false);

		UINT GetMenuResult(UINT id);

	public:

		CSize AdjustLayout();

		CPoint AdjustLocation(CSize size);

		CBCGPButton* GetButton(UINT id);

		void SetSize(CSize buttonSize, CSize buttonMargin, CSize imageSize, CSize seperatorMargin, CSize toolBarPadding);
		// image size, not button size
		void SetSize(EItemSize size);

	protected:

		afx_msg void OnCommand(UINT id);

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	protected:

		EPivot m_ePivot = EPivot::TopCenter;
		bool m_bExpandSize = false;
		bool m_bTransparent = false;

		std::vector<CBCGPButton*> m_buttons;

		// return CBCGPButton or ButtonWithMenu
		CBCGPButton* CreateButton(UINT id, bool menu, bool toggle = false, bool checked = false);

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
