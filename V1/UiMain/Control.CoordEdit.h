#pragma once

#include "Control.h"



namespace Control
{
	class CoordEdit : public CBCGPEdit
	{
		DECLARE_DYNAMIC(CoordEdit)

	public:

		CoordEdit();

		~CoordEdit();

	protected:

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnSetFocus();

		afx_msg LRESULT OnClear(WPARAM, LPARAM);

		afx_msg LRESULT OnCut(WPARAM, LPARAM);

		afx_msg LRESULT OnGetText(WPARAM, LPARAM);

		afx_msg LRESULT OnGetTextLength(WPARAM, LPARAM);

		afx_msg LRESULT OnPaste(WPARAM, LPARAM);

		afx_msg LRESULT OnSetText(WPARAM, LPARAM);

		DECLARE_MESSAGE_MAP()

	protected:

		CString m_buffer;
	};
}
