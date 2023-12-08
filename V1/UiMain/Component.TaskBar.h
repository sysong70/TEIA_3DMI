#pragma once

#include "Component.h"
#include "Signal.h"



namespace Control { class TaskPanel; }
namespace Window { class View; }



namespace Component
{
	class TaskBar : public CWnd
	{
	public:

		TaskBar();

		~TaskBar() override;

		bool Initialize(CWnd* pMainFrame);

	public:

		CSize AdjustLayout();

		Signal::Delivery& GetDelivery();

		Window::View* GetView();

		void SetPanel(Control::TaskPanel* pPanel);
		// Delete previous panel and Show/Hide bar
		void Show(Window::View* pTargetView);

	protected:

		void GetArea(CRect& header, CRect& body, CRect& footer);

		void OnClose();

	protected:

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()

	protected: // Header

		int m_nHeaderHeight = 0;
		CBCGPStatic m_wndTitle;
		CBCGPButton m_wndCloseHandle;

	protected: // Body

		Control::TaskPanel* m_pPanel = nullptr;

	protected: // Footer

		int m_nFooterHeight = 0;
		CBCGPButton m_wndClose;
	};
}
