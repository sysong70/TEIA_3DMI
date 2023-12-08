#pragma once

#include "Window.h"
#include "Component.LayerPanel.h"
#include "Component.ModelPanel.h"
#include "Component.PanelBar.h"
#include "Component.ScenePanel.h"
#include "Component.ViewPanel.h"
#include "Control.HistoryBar.h"
#include "Control.TabWnd.h"
#include <Signal.h>



namespace Command
{
	class Base;
}



namespace Window
{
	class View : public CView
	{
	protected:

		View();

		DECLARE_DYNCREATE(View)

	public:

		enum EType
		{
			Unknown = -1,
			View3d,
			View2d,
		};

		friend class MainFrame;

		~View() override;

		virtual void ReceiveSignal(Json::Object* pData) {}

	public:

		void CancelCommand();

		Signal::Delivery& GetDelivery();

		Document* GetDocument() const;

		int GetId();

	protected:

		void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;

		void OnDraw(CDC* pDC) override;

		void OnInitialUpdate() override;

		BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	#ifdef _DEBUG
		void AssertValid() const override;

		void Dump(CDumpContext& dc) const override;
	#endif

		afx_msg LRESULT OnSignal(WPARAM wp, LPARAM lp);

		afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnContextMenu(CWnd*, CPoint point);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnPaint();

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

		afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnTimer(UINT_PTR nIDEvent);

		DECLARE_MESSAGE_MAP()

	protected:

		EType m_eType = EType::Unknown;
		int m_nViewId = -1;
		bool m_bRenderer = false;
		bool m_bActivate = false;

		Signal::Delivery m_delivery;

		void Activate(bool value);
		// delay view activation by 0.1 seconds (for mouse)
		void DelayViewActivation();

		CRect GetClientArea();

		CSize GetClientSize();

		Window::MainFrame& GetMainFrame();

		bool IsValid();

	protected: // Commands

		Command::Base* m_pActiveCommand = nullptr;

	protected: // ToolBar

		Control::ToolBar m_toolBar;
		Control::HistoryBar m_historyBar;

		virtual void CreateHistoryBar();

		virtual void CreateToolBar();

	protected: // PanelBar

		Control::TabWnd m_tabs;

		virtual void CreatePanelTabs();
	};
}
