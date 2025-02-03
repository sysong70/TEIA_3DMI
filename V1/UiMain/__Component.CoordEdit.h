#pragma once

#include "Component.h"
#include "Window.h"

//--------------------------------------------------------------------------------------------------

namespace Component
{
	class FloatValueEdit : public CBCGPEdit
	{
		DECLARE_DYNAMIC(FloatValueEdit)

	public:

		FloatValueEdit();

		~FloatValueEdit();

	public:

		void SetDigit(int value);

		void SetIndex(int value);

		CString GetValue();

		void SetValue(CString value);

	protected:

		afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnKillFocus(CWnd* pNewWnd);

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		afx_msg void OnSetFocus(CWnd* pOldWnd);

		DECLARE_MESSAGE_MAP()

	protected:

		int m_nIndex = -1;
		CString m_valueFormat = L"%.4lf";
		CString m_value;
		CString m_originalValue;
	};



	class CoordEdit : public CWnd
	{
	public:

		friend class FloatValueEdit;

		enum class EFormat
		{
			Point2d = 2,
			Point3d = 3,
		};

		CoordEdit();

		~CoordEdit() override;

	public:

		void ChangeActive(int index);

		void Clear();

	public:

		CString GetValue();

		void SetValue(const CString& source, int activate = -1);

	protected:

		BOOL DestroyWindow() override;

		BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	protected:

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg void OnEnSetFocus();

		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

		afx_msg void OnSetFocus(CWnd* pOldWnd);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP();

	protected:

		FloatValueEdit m_wndEdit1;
		FloatValueEdit m_wndEdit2;
		FloatValueEdit m_wndEdit3;

		CString m_value;
		EFormat m_format = EFormat::Point3d;
		int m_nActivated = 0;
	};
}

