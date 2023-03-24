#include "stdafx.h"
#include "Layout.ControlWrapper.h"
#include <Json.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Layout::ControlWrapper::ControlWrapper(CWnd* pValue, EContent type)
{
	Content(pValue, type);
}



Layout::ControlWrapper::~ControlWrapper()
{
	if (m_pContent != nullptr) {
		m_pContent->DestroyWindow();
	}
}



void Layout::ControlWrapper::HorizontalAlignment(EHorizontalAlignment value)
{
	m_eHorizontalAlignment = value;
}

Layout::EHorizontalAlignment Layout::ControlWrapper::HorizontalAlignment()
{
	return m_eHorizontalAlignment;
}



void Layout::ControlWrapper::HorizontalContentAlignment(EHorizontalAlignment value)
{
	m_eHorizontalContentAlignment = value;
}

Layout::EHorizontalAlignment Layout::ControlWrapper::HorizontalContentAlignment()
{
	return m_eHorizontalContentAlignment;
}



void Layout::ControlWrapper::VerticalAlignment(EVerticalAlignment value)
{
	m_eVerticalAlignment = value;
}

Layout::EVerticalAlignment Layout::ControlWrapper::VerticalAlignment()
{
	return m_eVerticalAlignment;
}



void Layout::ControlWrapper::VerticalContentAlignment(EVerticalAlignment value)
{
	m_eVerticalContentAlignment = value;
}

Layout::EVerticalAlignment Layout::ControlWrapper::VerticalContentAlignment()
{
	return m_eVerticalContentAlignment;
}



void Layout::ControlWrapper::Alignment(EHorizontalAlignment h, EVerticalAlignment v)
{
	m_eHorizontalAlignment = h;
	m_eVerticalAlignment = v;
}



void Layout::ControlWrapper::ContentAlignment(EHorizontalAlignment h, EVerticalAlignment v)
{
	m_eHorizontalContentAlignment = h;
	m_eVerticalContentAlignment = v;
}



int Layout::ControlWrapper::CalculateWidth(EBoxModel box)
{
	int cx = m_size.cx;

	if (cx == 0) {
		CRect rect;
		m_pContent->GetClientRect(rect);
		cx = rect.Width();

		if (box > EBoxModel::Content) cx += m_padding.left + m_padding.right;
		if (box > EBoxModel::Padding) cx += m_border.left + m_border.right;
		if (box > EBoxModel::Border) cx += m_margin.left + m_margin.right;
	}
	else {
		cx += m_margin.left + m_margin.right;

		if (box < EBoxModel::Margin) cx -= m_margin.left + m_margin.right;
		if (box < EBoxModel::Border) cx -= m_border.left + m_border.right;
		if (box < EBoxModel::Padding) cx -= m_padding.left + m_padding.right;
	}

	return cx;
}



int Layout::ControlWrapper::CalculateHeight(EBoxModel box)
{
	int cy = m_size.cy;

	if (cy == 0) {
		CRect rect;
		m_pContent->GetClientRect(rect);
		cy = rect.Height();

		if (box > EBoxModel::Content) cy += m_padding.top + m_padding.bottom;
		if (box > EBoxModel::Padding) cy += m_border.top + m_border.bottom;
		if (box > EBoxModel::Border) cy += m_margin.top + m_margin.bottom;
	}
	else {
		cy += m_margin.top + m_margin.bottom;

		if (box < EBoxModel::Margin) cy -= m_margin.top + m_margin.bottom;
		if (box < EBoxModel::Border) cy -= m_border.top + m_border.bottom;
		if (box < EBoxModel::Padding) cy -= m_padding.top + m_padding.bottom;
	}

	return 0;
}



void Layout::ControlWrapper::Update()
{
	CRect boundary;
	m_pContent->GetClientRect(boundary);
	boundary.DeflateRect(m_border);
	boundary.DeflateRect(m_padding);

	CPoint offset = boundary.TopLeft();
	CSize size = CalculateSize(EBoxModel::Content);
	CRect content = { offset, size };

	switch (HorizontalContentAlignment()) {
	case EHorizontalAlignment::Center: offset.x = boundary.CenterPoint().x  - size.cx / 2; break;
	case EHorizontalAlignment::Right: offset.x = boundary.right - content.Size().cx; break;
	case EHorizontalAlignment::Stretch: size.cx = boundary.Size().cx; break;
	default: break;
	}

	switch (VerticalContentAlignment()) {
	case EVerticalAlignment::Center: offset.y = boundary.CenterPoint().y - size.cy / 2; break;
	case EVerticalAlignment::Bottom: offset.y = boundary.bottom - content.Size().cy; break;
	case EVerticalAlignment::Stretch: size.cy = boundary.Size().cy; break;
	default: break;
	}

	m_pContent->SetWindowPos(nullptr, offset.x, offset.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOREDRAW);
}



void Layout::ControlWrapper::Content(CWnd* pValue, EContent type)
{
	m_pContent = pValue;
	m_eType = type;

	switch (type) {
	case EContent::Button:
		Style("Button");
		break;

	case EContent::Label:
		Style("Label");

	default:
		DEBUG_STOP;
		break;
	}
}



/*

void Layout::ControlWrapper::Width(int value)
{
	m_size.cx = value;
}



int Layout::ControlWrapper::Width()
{
	return Width(m_eBoxModel);
}



int Layout::ControlWrapper::Width(EBoxModel type)
{
	if (m_eHorizontalAlignment == EHorizontalAlignment::Stretch) {
		CSize size = GetParentSize(EBoxModel::Content);
		if (m_pBack == nullptr) {
			return size.cx;
		}
		else {

		}
	}

	int cx = m_size.cx;

	switch (m_eBoxModel) {
	case EBoxModel::Margin:

	case EBoxModel::Border:
		cx += m_margin.left + m_margin.right;

	case EBoxModel::Padding:
		cx += m_border.left + m_border.right;

	case EBoxModel::Content:
		cx += m_padding.left + m_padding.right;
	}

	switch (type) {
	case EBoxModel::Margin:
		break;

	case EBoxModel::Border:
		cx -= m_margin.left + m_margin.right;

	case EBoxModel::Padding:
		cx += m_border.left + m_border.right;

	case EBoxModel::Content:
		cx += m_padding.left + m_padding.right;
	}

	return cx;
}



void Layout::ControlWrapper::Height(int value)
{
	m_size.cy = value;
}



int Layout::ControlWrapper::Height()
{
	return Height(m_eBoxModel);
}



int Layout::ControlWrapper::Height(EBoxModel type)
{
	int cy = 0;

	switch (type) {
	case EBoxModel::Margin:

	case EBoxModel::Border:
		cy -= m_margin.left + m_margin.right;

	case EBoxModel::Padding:
		cy -= m_border.left + m_border.right;

	case EBoxModel::Content:
		cy -= m_padding.left + m_padding.right;
	}

	return m_size.cy - cy;
}



void Layout::ControlWrapper::Size(int x, int y)
{
	Width(x);
	Height(y);
}



void Layout::ControlWrapper::Size(CSize value)
{
	Width(value.cx);
	Height(value.cy);
}



CSize Layout::ControlWrapper::Size(EBoxModel type)
{
	return { Width(type), Height(type) };
}



void Layout::ControlWrapper::Thickness(int value, EBoxModel type)
{
	Thickness(value, value, value, value, type);
}



void Layout::ControlWrapper::Thickness(int horizontal, int vertical, EBoxModel type)
{
	Thickness(horizontal, vertical, horizontal, vertical, type);
}



void Layout::ControlWrapper::Thickness(int left, int top, int right, int bottom, EBoxModel type)
{
	CRect& rect = CRect();

	switch (type) {
	case EBoxModel::Padding: rect = m_padding; break;
	case EBoxModel::Border: rect = m_border; break;
	case EBoxModel::Margin: rect = m_margin; break;

	case EBoxModel::Content:
	default:
		DEBUG_RETURN;
	}

	rect = { left, top, right, bottom };
}



void Layout::ControlWrapper::Thickness(CRect value, EBoxModel type)
{
	Thickness(value.left, value.top, value.right, value.bottom, type);
}



CRect Layout::ControlWrapper::Thickness(EBoxModel type)
{
	switch (type) {
	case EBoxModel::Padding: return m_padding;
	case EBoxModel::Border: return m_border;
	case EBoxModel::Margin: return m_margin;

	case EBoxModel::Content:
	default:
		DEBUG_STOP;
		return {};
	}
}



void Layout::ControlWrapper::HorizontalAlignment(EHorizontalAlignment value)
{
	m_eHorizontalAlignment = value;
}



void Layout::ControlWrapper::VerticalAlignment(EVerticalAlignment value)
{
	m_eVerticalAlignment = value;
}



void Layout::ControlWrapper::HorizontalContentAlignment(EHorizontalAlignment value)
{
	m_eHorizontalContentAlignment = value;
}



void Layout::ControlWrapper::VerticalContentAlignment(EVerticalAlignment value)
{
	m_eVerticalContentAlignment = value;
}



void Layout::ControlWrapper::Visibility(EVisibility value)
{
	m_eVisibility = value;
}



void Layout::ControlWrapper::Color(Control::EColor value, EColorType type)
{
	switch (type) {
	case EColorType::Border:
		m_borderColor = value;
		break;

	case EColorType::Background:
		m_backColor = value;
		break;
	}
}



void Layout::ControlWrapper::Update()
{
}



CSize Layout::ControlWrapper::GetParentSize(EBoxModel type)
{
	if (m_pParent == nullptr) {
		DEBUG_VALID(GetParent());
		CRect rect;
		GetParent()->GetClientRect(&rect);
		return rect.Size();
	}
	else {
		return m_pParent->Size(type);
	}
}



CRect Layout::ControlWrapper::GetWindowRect()
{
	CRect parentRect;
	CRect frontRect;
	CRect resultRect;

	if (m_pParent != nullptr) {
		parentRect = m_pParent->m_border + m_pParent->m_padding;
	}
	if (m_pFront != nullptr) {
		frontRect = m_pFront->GetWindowRect();
	}

	return resultRect;
}



BOOL Layout::ControlWrapper::OnEraseBkgnd(CDC* pDC)
{
	CSize size = { Width(EBoxModel::Margin), Height(EBoxModel::Margin) };

	CRect rect = { { 0, 0 }, size };
	pDC->FillRect(rect, (CBrush*)&CBrush(GetTransparent()));

	rect.DeflateRect(m_margin);
	if (m_border.IsRectNull() == FALSE && m_borderColor != Control::EColor::Transparent) {
		pDC->FillRect(rect, (CBrush*)&CBrush((COLORREF)m_borderColor));
	}

	rect.DeflateRect(m_border);
	if (m_padding.IsRectNull() == FALSE) {
		pDC->FillRect(rect, (CBrush*)&CBrush(GetTransparent()));
	}

	rect.DeflateRect(m_padding);
	if (m_backColor != Control::EColor::Transparent) {
		pDC->FillRect(rect, (CBrush*)&CBrush((COLORREF)m_backColor));
	}

	return __super::OnEraseBkgnd(pDC);
}

*/