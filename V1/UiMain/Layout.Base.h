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

		virtual void Style(CStringA name);

		virtual void HorizontalAlignment(EHorizontalAlignment value) IMPLEMENT();
		virtual EHorizontalAlignment HorizontalAlignment() IMPLEMENT(EHorizontalAlignment::Stretch);

		virtual void HorizontalContentAlignment(EHorizontalAlignment value) IMPLEMENT();
		virtual EHorizontalAlignment HorizontalContentAlignment() IMPLEMENT(EHorizontalAlignment::Stretch);

		virtual void VerticalAlignment(EVerticalAlignment value) IMPLEMENT();
		virtual EVerticalAlignment VerticalAlignment() IMPLEMENT(EVerticalAlignment::Stretch);

		virtual void VerticalContentAlignment(EVerticalAlignment value) IMPLEMENT();
		virtual EVerticalAlignment VerticalContentAlignment() IMPLEMENT(EVerticalAlignment::Stretch);

		virtual void Alignment(EHorizontalAlignment h, EVerticalAlignment v) IMPLEMENT();
		virtual void ContentAlignment(EHorizontalAlignment h, EVerticalAlignment v) IMPLEMENT();

		virtual void Visibility(EVisibility value) IMPLEMENT();

		virtual void Color(Control::EColor value, EColorType type) IMPLEMENT();

		virtual void Width(int value);
		virtual void Height(int value);
		virtual void Size(int cx, int cy);
		virtual void Size(CSize value);

		virtual int CalculateWidth(EBoxModel box = EBoxModel::Margin) IMPLEMENT(0);
		virtual int CalculateHeight(EBoxModel box = EBoxModel::Margin) IMPLEMENT(0);
		virtual CSize CalculateSize(EBoxModel box = EBoxModel::Margin);

		virtual void Thickness(int value, EBoxModel type);
		virtual void Thickness(int horizontal, int vertical, EBoxModel type);
		virtual void Thickness(int left, int top, int right, int bottom, EBoxModel type);
		virtual void Thickness(CRect value, EBoxModel type);
		virtual CRect Thickness(EBoxModel type);

		virtual void Update() IMPLEMENT();

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

		EVisibility m_eVisibility = EVisibility::Visible;

		Control::EColor m_borderColor = Control::EColor::Transparent;
		Control::EColor m_backColor = Control::EColor::Transparent;

		CSize m_size; // Window size - Content + Padding + Border
		CRect m_padding; // Thickness
		CRect m_border; // Thickness
		CRect m_margin; // Thickness

	private:

		EHorizontalAlignment m_eHorizontalAlignment = EHorizontalAlignment::Stretch;
		EVerticalAlignment m_eVerticalAlignment = EVerticalAlignment::Stretch;
		EHorizontalAlignment m_eHorizontalContentAlignment = EHorizontalAlignment::Stretch;
		EVerticalAlignment m_eVerticalContentAlignment = EVerticalAlignment::Stretch;

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()
	};
}

#undef IMPLEMENT
