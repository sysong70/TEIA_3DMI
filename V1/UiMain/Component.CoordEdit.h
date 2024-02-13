#pragma once

#include "Component.h"
#include "Control.FloatValueEdit.h"
#include "Window.h"

//--------------------------------------------------------------------------------------------------

namespace Component
{
	class CoordEdit : public CWnd
	{
	public:

		friend class Control::FloatValueEdit;

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

		using Coordinates = std::vector<CString>;

		CString GetValue();

		void SetValue(const CString& source);

	protected:

		BOOL DestroyWindow() override;

		BOOL PreCreateWindow(CREATESTRUCT& cs) override;

	protected:

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnEnSetFocus();

		DECLARE_MESSAGE_MAP();

	protected:

		Control::FloatValueEdit m_wndEdit1;
		Control::FloatValueEdit m_wndEdit2;
		Control::FloatValueEdit m_wndEdit3;

		CString m_value;
		EFormat m_format = EFormat::Point3d;
		int m_nActivated = 0;
	};
}

