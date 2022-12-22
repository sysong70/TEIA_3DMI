#pragma once

#include "Window.h"
#include "Signal.h"
#include "Component.HistoryBar.h"
#include "Component.LayerPanel.h"
#include "Component.ModelTreePanel.h"
#include "Component.PanelBar.h"
#include "Component.ScenePanel.h"
#include "Component.TabWnd.h"
#include "Component.TaskBar.h"
#include "Component.ViewPanel.h"



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

		Document* GetDocument() const;

		int GetId();

		void ReceiveSignal(Json::Object* pData);

		void SetFilePath(CString s);

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

		afx_msg void OnCommand(UINT id);

		afx_msg void OnContextMenu(CWnd*, CPoint point);

		afx_msg void OnPaint();

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnMButtonUp(UINT nFlags, CPoint point);

		afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);

		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint point);

		afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnTimer(UINT_PTR nIDEvent);

		DECLARE_MESSAGE_MAP()

	private:

		int m_nViewId = -1;
		CString m_sFilePath;
		bool m_bValid = false;
		bool m_bActivate = false;

		Signal::Delivery m_delivery;

		void Activate(bool value);
		// delay view activation by 0.1 seconds (for mouse)
		void DelayViewActivation();

		CRect GetClientArea();

		Window::MainFrame& GetMainFrame();

		bool IsValid();

	private: // ToolBar

		Component::ToolBar m_toolBar;
		Component::HistoryBar m_historyBar;

		void CreateHistoryBar();

		void CreateToolBar();

	private: // PanelBar

		Component::TabWnd m_tabs;

		Component::ModelTreePanel m_modelTreePanel;
		Component::ViewPanel m_viewPanel;
		Component::LayerPanel m_layerPanel;
		Component::ScenePanel m_scenePanel;

		void CreatePanelTabs();

	private:

		Component::TaskBar m_taskBar;

		void CreateTaskBar();

		void ShowTaskBar();
	};
}