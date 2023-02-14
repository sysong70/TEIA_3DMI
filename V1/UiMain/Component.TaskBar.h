#pragma once

#include "Component.h"



namespace Component
{
	class TaskBar : public CWnd
	{
	public:

		TaskBar();

		~TaskBar() override;

		bool Initialize(CWnd* pParentWnd);

	public:

		CSize AdjustLayout();

	protected:

		void PostNcDestroy() override;

		afx_msg LRESULT OnDPIChangedAfterParent(WPARAM, LPARAM);

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()
	};
}
