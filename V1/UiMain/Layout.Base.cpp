#include "stdafx.h"
#include "Layout.Base.h"
#include "Facility.AppResources.h"
#include <Json.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Layout;

BEGIN_MESSAGE_MAP(Base, CWnd)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



Layout::Base::Base()
{
}



Layout::Base::~Base()
{
}



void Layout::Base::Parent(Base* pValue)
{
	m_pParent = pValue;
}



void Layout::Base::Style(CString name)
{
	auto AssignThickness = [](Json::Object& style, CStringA name, CRect& thickness, bool scale = true) {
		if (style.FindValue(name) == false) {
			return;
		}

		Json::Array& arr = style.GetArray(name);
		thickness = {
			arr[0]->ToInteger(),
			arr[1]->ToInteger(),
			arr[2]->ToInteger(),
			arr[3]->ToInteger()
		};

		if (scale) {
			thickness = globalUtils.ScaleByDPI(thickness);
		}
	};

	if (TheAppResources.GetStyles().FindValue((CStringA)name) == false) {
		return;
	}
	Json::Object& style = TheAppResources.GetStyles().GetAt((CStringA)name);

	if (style.FindValue("size")) {
		Json::Array& size = style.GetArray("size");
		Size(globalUtils.ScaleByDPI(CSize(size[0]->ToInteger(), size[1]->ToInteger())));
	}

	AssignThickness(style, "padding", m_padding);
	AssignThickness(style, "border", m_border, false);
	AssignThickness(style, "margin", m_margin);

	m_eHorizontalAlignment = (EHorizontalAlignment)style.GetInteger("horizontalAlignment", (int)m_eHorizontalAlignment);
	m_eVerticalAlignment = (EVerticalAlignment)style.GetInteger("verticalAlignment", (int)m_eVerticalAlignment);
	m_eHorizontalContentAlignment = (EHorizontalAlignment)style.GetInteger("horizontalContentAlignment", (int)m_eHorizontalContentAlignment);
	m_eVerticalContentAlignment = (EVerticalAlignment)style.GetInteger("verticalContentAlignment", (int)m_eVerticalContentAlignment);

	m_eVisibility = (EVisibility)style.GetInteger("visibility", (int)m_eVisibility);

	m_borderColor = (Control::EColor)style.GetInteger("borderColor", (int)m_borderColor);
	m_backColor = (Control::EColor)style.GetInteger("backColor", (int)m_backColor);

}



void Layout::Base::HorizontalAlignment(EHorizontalAlignment value)
{
	m_eHorizontalAlignment = value;
}



void Layout::Base::HorizontalContentAlignment(EHorizontalAlignment value)
{
	m_eHorizontalContentAlignment = value;
}



void Layout::Base::VerticalAlignment(EVerticalAlignment value)
{
	m_eVerticalAlignment = value;
}



void Layout::Base::VerticalContentAlignment(EVerticalAlignment value)
{
	m_eVerticalContentAlignment = value;
}



void Layout::Base::Alignment(EHorizontalAlignment h, EVerticalAlignment v)
{
	m_eHorizontalAlignment = h;
	m_eVerticalAlignment = v;
}



void Layout::Base::ContentAlignment(EHorizontalAlignment h, EVerticalAlignment v)
{
	m_eHorizontalContentAlignment = h;
	m_eVerticalContentAlignment = v;
}



void Layout::Base::Width(int value)
{
	m_size.cx = value;
}



void Layout::Base::Height(int value)
{
	m_size.cy = value;
}



void Layout::Base::Size(int cx, int cy)
{
	m_size.cx = cx;
	m_size.cy = cy;
}



void Layout::Base::Size(CSize value)
{
	m_size = value;
}



CSize Layout::Base::CalculateSize()
{
	return { CalculateWidth(), CalculateHeight() };
}



void Layout::Base::Thickness(int value, EBoxModel type)
{
	Thickness(value, value, value, value, type);
}



void Layout::Base::Thickness(int horizontal, int vertical, EBoxModel type)
{
	Thickness(horizontal, vertical, horizontal, vertical, type);
}



void Layout::Base::Thickness(int left, int top, int right, int bottom, EBoxModel type)
{
	CRect& target = m_padding;
	switch (type) {
	case EBoxModel::Border: target = m_border; break;
	case EBoxModel::Margin: target = m_margin; break;
	default:
		break;
	}

	target.SetRect(left, top, right, bottom);
}



void Layout::Base::Thickness(CRect value, EBoxModel type)
{
	Thickness(value.left, value.top, value.right, value.bottom, type);
}



CRect Layout::Base::Thickness(EBoxModel type)
{
	switch (type) {
	case EBoxModel::Padding: return m_padding;
	case EBoxModel::Border: return m_border;
	case EBoxModel::Margin: return m_margin;
	default:
		RETURN({});
	}
}



CSize Layout::Base::GetSize()
{
	CRect rect;
	GetClientRect(&rect);

	return rect.Size();
}



CRect Layout::Base::GetClientArea()
{
	CRect rect;
	GetClientRect(&rect);
	MapWindowPoints(GetParent(), rect);

	return rect;
}



void Layout::Base::MovePositionX(int value)
{
	CRect rect = GetClientArea();
	rect.right = rect.Width() + value;
	rect.left = value;

	MoveWindow(rect, FALSE);
}



void Layout::Base::MovePositionY(int value)
{
	CRect rect = GetClientArea();
	rect.bottom = rect.Height() + value;
	rect.top = value;

	MoveWindow(rect, FALSE);
}



void Layout::Base::MovePosition(int x, int y)
{
	CRect rect = GetClientArea();
	rect.right = rect.Width() + x;
	rect.left = x;
	rect.bottom = rect.Height() + y;
	rect.top = y;

	MoveWindow(rect, FALSE);
}



void Layout::Base::ResizeWidth(int value)
{
	CRect rect = GetClientArea();
	rect.right = rect.left + value;

	MoveWindow(rect, FALSE);
}



void Layout::Base::ResizeHeight(int value)
{
	CRect rect = GetClientArea();
	rect.bottom = rect.top + value;

	MoveWindow(rect, FALSE);
}



void Layout::Base::Resize(int width, int height)
{
	CRect rect = GetClientArea();
	rect.right = rect.left + width;
	rect.bottom = rect.top + height;

	MoveWindow(rect, FALSE);
}



BOOL Layout::Base::OnEraseBkgnd(CDC* pDC)
{
	if (GetSafeHwnd() == nullptr) {
		return __super::OnEraseBkgnd(pDC);
	}

	CRect rect;
	GetClientRect(rect);
	if (rect.IsRectEmpty()) {
		return __super::OnEraseBkgnd(pDC);
	}

	if (m_backColor != Control::EColor::Transparent) {
		pDC->FillRect(rect, (CBrush*)&CBrush((COLORREF)m_backColor));
	}
	else {
		pDC->FillRect(rect, (CBrush*)&CBrush(GetTransparent()));
	}

	if (m_border.IsRectNull() == FALSE && m_borderColor != Control::EColor::Transparent) {
		COLORREF fill = (COLORREF)m_borderColor;
		pDC->FillSolidRect(0, 0, m_border.left, rect.bottom, fill); // left
		pDC->FillSolidRect(0, 0, rect.right, m_border.top, fill); // top
		pDC->FillSolidRect(rect.right - m_border.right, 0, m_border.right, rect.bottom, fill); // right
		pDC->FillSolidRect(0, rect.bottom - m_border.bottom, rect.right, m_border.bottom, fill); // bottom
	}

	return __super::OnEraseBkgnd(pDC);
}
