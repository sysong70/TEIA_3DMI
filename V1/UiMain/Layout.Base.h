#pragma once

#include "Layout.h"
#include "Control.h"

#define IMPLEMENT(x) { DEBUG_STOP; return x; }



namespace Layout
{
	class Base : public CWnd
	{
	public:

		friend class ControlWrapper;
		friend class Stack;

		Base();

		virtual ~Base();

		virtual void Parent(Base* pValue);

		virtual void Style(CString name);

		virtual void HorizontalAlignment(EHorizontalAlignment value);

		virtual void HorizontalContentAlignment(EHorizontalAlignment value);

		virtual void VerticalAlignment(EVerticalAlignment value);

		virtual void VerticalContentAlignment(EVerticalAlignment value);

		virtual void Alignment(EHorizontalAlignment h, EVerticalAlignment v);

		virtual void ContentAlignment(EHorizontalAlignment h, EVerticalAlignment v);

		virtual void Visibility(EVisibility value) IMPLEMENT();

		virtual void Color(Control::EColor value, EColorType type) IMPLEMENT();

		virtual void Width(int value);
		virtual void Height(int value);
		virtual void Size(int cx, int cy);
		virtual void Size(CSize value);

		virtual int CalculateWidth() IMPLEMENT(0);
		virtual int CalculateHeight() IMPLEMENT(0);
		virtual CSize CalculateSize();
		virtual CRect CalculateRect(EBoxModel type) IMPLEMENT(0);

		virtual void Thickness(int value, EBoxModel type);
		virtual void Thickness(int horizontal, int vertical, EBoxModel type);
		virtual void Thickness(int left, int top, int right, int bottom, EBoxModel type);
		virtual void Thickness(CRect value, EBoxModel type);
		virtual CRect Thickness(EBoxModel type);

		virtual void Update(CRect parent) IMPLEMENT();
		virtual void AdjustChildren() IMPLEMENT();
		virtual void AdjustParent() IMPLEMENT();

		virtual void Add(Base* pValue) IMPLEMENT();
		virtual void Remove(Base* pValue) IMPLEMENT();

	protected:

		CSize GetSize();

		CRect GetClientArea();

		void MovePositionX(int value);

		void MovePositionY(int value);

		void MovePosition(int x, int y);

		void ResizeWidth(int value);

		void ResizeHeight(int value);

		void Resize(int width, int height);

	protected:

		Base* m_pParent = nullptr;
		CString m_styleName;

		EHorizontalAlignment m_eHorizontalAlignment = EHorizontalAlignment::Stretch;
		EVerticalAlignment m_eVerticalAlignment = EVerticalAlignment::Stretch;
		EHorizontalAlignment m_eHorizontalContentAlignment = EHorizontalAlignment::Stretch;
		EVerticalAlignment m_eVerticalContentAlignment = EVerticalAlignment::Stretch;

		EVisibility m_eVisibility = EVisibility::Visible;

		Control::EColor m_borderColor = Control::EColor::Transparent;
		Control::EColor m_backColor = Control::EColor::Transparent;

		CSize m_size; // Content + Padding + Border
		CRect m_padding; // Thickness
		CRect m_border; // Thickness
		CRect m_margin; // Thickness

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()
	};
}

#undef IMPLEMENT
