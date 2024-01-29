#pragma once

#include "Control.h"



namespace Component
{
	class CoordEdit;
}



namespace Control
{
	class FloatValueEdit : public CBCGPEdit
	{
		DECLARE_DYNAMIC(FloatValueEdit)

	public:

		FloatValueEdit();

		~FloatValueEdit();

		void SetDigit(int value);

		void SetIndex(int value);

		CString GetValue();

		void SetValue(CString value);

	protected:

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		//afx_msg void OnLButtonUp(UINT nFlags, CPoint point);

		afx_msg void OnSetFocus(CWnd* pOldWnd);

		//afx_msg LRESULT OnClear(WPARAM, LPARAM);

		//afx_msg LRESULT OnCut(WPARAM, LPARAM);

		//afx_msg LRESULT OnGetText(WPARAM, LPARAM);

		//afx_msg LRESULT OnGetTextLength(WPARAM, LPARAM);

		//afx_msg LRESULT OnKillFocus(WPARAM, LPARAM);

		//afx_msg LRESULT OnPaste(WPARAM, LPARAM);

		//afx_msg LRESULT OnSetText(WPARAM, LPARAM);

		DECLARE_MESSAGE_MAP()

	protected:

		int m_nIndex = -1;
		CString m_valueFormat = L"%.4lf";
		CString m_value;
	};
}
