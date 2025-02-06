#pragma once

#include "Window.h"
#include "Component.LayerPanel.h"
#include "Component.ModelPanel.h"
#include "Component.PanelBar.h"
#include "Component.ScenePanel.h"
#include "Component.ViewPanel.h"
#include "Command.h"
#include "Control.HistoryBar.h"
#include "Control.TabWnd.h"

//--------------------------------------------------------------------------------------------------

namespace Window
{
	class View : public CView
	{
	protected:

		View();

		DECLARE_DYNCREATE(View)

	public:

		friend class MainFrame;

		~View() override;

		virtual Signal::Delivery& GetDelivery();

		virtual void ReceiveSignal(Json::Object* pData) {}

		virtual bool SetContextMenu(Json::Object* pData) { return false; }

		virtual void ShowContextMenu(Json::Object* pData) {}

	public:

		void CancelCommand();

		Document* GetDocument() const;

		int GetId();

		EViewType GetViewType();

		Control::ToolBar& GetToolBar();

		Control::HistoryBar& GetHistoryBar();

	protected:

		void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;
		// WARNING - abstract
		void OnDraw(CDC* pDC) override;

		void OnInitialUpdate() override;

		BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	#ifdef _DEBUG
		void AssertValid() const override;

		void Dump(CDumpContext& dc) const override;
	#endif

		afx_msg LRESULT OnSignal(WPARAM wp, LPARAM lp);

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

		DECLARE_MESSAGE_MAP()

	protected:

		EViewType m_eType = EViewType::Unknown;
		int m_nViewId = -1;
		bool m_bRenderer = false;
		bool m_bActivate = false;

		void Activate(bool value);

		CRect GetClientArea();

		CSize GetClientSize();

		Window::MainFrame& GetMainFrame();

		bool IsValid();

	protected: // Commands

		Command::Base* m_pActiveCommand = nullptr;

		virtual void CreateCommandPrompt() {}

	protected: // ToolBar

		Control::ToolBar m_toolBar;
		Control::HistoryBar m_historyBar;

		virtual void CreateHistoryBar(Control::EPivot pivot = Control::EPivot::BottomCenter);

		virtual void CreateToolBar();

	protected: // PanelBar

		Control::TabWnd m_tabs;

		virtual void CreatePanelTabs();
	};
}
