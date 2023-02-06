#pragma once

#include "Component.h"



namespace Component
{
	class CheckButton : public CWnd
	{
	public:

		CheckButton();

		~CheckButton() override;

	protected:

		void PostNcDestroy() override;

		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	protected:

		CBCGPSVGImage m_wndIcon;
		CBCGPStatic m_wndTitle;
		CBCGPSVGImage m_wndButton;
	};
}
