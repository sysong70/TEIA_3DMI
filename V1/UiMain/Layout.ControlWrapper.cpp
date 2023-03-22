#include "stdafx.h"
#include "Layout.ControlWrapper.h"
#include <Json.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Layout::ControlWrapper::ControlWrapper()
{
}



Layout::ControlWrapper::~ControlWrapper()
{
	if (m_pContent != nullptr) {
		m_pContent->DestroyWindow();
	}
}



void Layout::ControlWrapper::Content(CWnd* pValue, EContent type)
{
	m_pContent = pValue;
	m_eType = type;

	switch (type) {
	case EContent::Button:
		Style(L"Button");
		break;

	default:
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