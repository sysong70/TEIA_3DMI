#pragma once

#include "Component.h"
#include "Window.h"
#include <vector>



namespace Component
{
	class InputBar : public CWnd
	{
	public:

		InputBar();

		~InputBar() override;

		bool Initialize(Window::View* pView);

		bool IsVisible();

		void ChangeActive(int index);

		void Clear();

	public:

		void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		void OnMouseMove(UINT nFlags, CPoint point);

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

		DECLARE_MESSAGE_MAP();

	protected:

		CWnd* m_pView = nullptr;
		bool m_bVisible = false;

		CBCGPEdit m_wndEdit1;
		CBCGPEdit m_wndEdit2;
		CBCGPEdit m_wndEdit3;
		CBCGPEdit m_wndEdit4;
		int m_nActivated = 0;
	};
}

