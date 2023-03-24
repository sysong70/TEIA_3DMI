#include "stdafx.h"
#include "Layout.Stack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



Layout::Stack::Stack()
{

}



Layout::Stack::~Stack()
{
	for (auto child : m_children) {
		REMOVE_POINTER(child);
	}

	m_children.clear();
}



void Layout::Stack::HorizontalAlignment(EHorizontalAlignment value)
{
	m_eHorizontalAlignment = value;
}

Layout::EHorizontalAlignment Layout::Stack::HorizontalAlignment()
{
	return m_eHorizontalAlignment;
}



void Layout::Stack::VerticalAlignment(EVerticalAlignment value)
{
	m_eVerticalAlignment = value;
}

Layout::EVerticalAlignment Layout::Stack::VerticalAlignment()
{
	return m_eVerticalAlignment;
}



int Layout::Stack::CalculateWidth(EBoxModel box)
{
	int cx = m_size.cx;

	if (cx == 0) {
		if (m_eOrientation == EOrientation::Horizontal) {
			for (auto child : m_children) {
				cx += child->CalculateWidth();
			}
		}
		else {
			for (auto child : m_children) {
				cx = max(cx, child->CalculateWidth());
			}
		}

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



int Layout::Stack::CalculateHeight(EBoxModel box)
{
	int cy = m_size.cy;

	if (cy == 0) {
		if (m_eOrientation == EOrientation::Horizontal) {
			for (auto child : m_children) {
				cy = max(cy, child->CalculateHeight());
			}
		}
		else {
			for (auto child : m_children) {
				cy += child->CalculateHeight();
			}
		}

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

	return cy;
}



void Layout::Stack::Update()
{
	CRect boundary = { CPoint(), CalculateSize(EBoxModel::Border) };
	boundary.DeflateRect(m_border);
	boundary.DeflateRect(m_padding);

	CPoint offset = boundary.TopLeft();

	if (m_eOrientation == EOrientation::Horizontal) {
		if (m_eFlow == EFlow::RightToLeft) {
			offset.x = boundary.right;
		}

		for (auto child : m_children) {
			CSize size = child->CalculateSize();

			switch (child->VerticalAlignment()) {
			case EVerticalAlignment::Center: offset.y = boundary.CenterPoint().y - size.cy / 2; break;
			case EVerticalAlignment::Bottom: offset.y = boundary.bottom - size.cy; break;
			default: break;
			}

			child->SetWindowPos(nullptr, offset.x, offset.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOREDRAW);
			child->Update();
			offset.x += (m_eFlow == EFlow::RightToLeft ? -size.cx : size.cx);
		}
	}
	else {
		if (m_eFlow ==  EFlow::BottomToTop) {
			offset.y = boundary.bottom;
		}

		for (auto child : m_children) {
			CSize size = child->CalculateSize();

			switch (child->HorizontalAlignment()) {
			case EHorizontalAlignment::Center: offset.x = boundary.CenterPoint().x - size.cx / 2; break;
			case EHorizontalAlignment::Right: offset.x = boundary.right - size.cy; break;
			default: break;
			}

			child->SetWindowPos(nullptr, offset.x, offset.y, size.cx, size.cy, SWP_NOZORDER | SWP_NOREDRAW);
			child->Update();
			offset.y += (m_eFlow == EFlow::BottomToTop ? -size.cy : size.cy);
		}
	}
}



void Layout::Stack::AddChild(Base* pValue)
{
	m_children.push_back(pValue);
}



void Layout::Stack::RemoveChild(Base* pValue)
{
	auto it = std::find(m_children.begin(), m_children.end(), pValue);
	if (it != m_children.end()) {
		m_children.erase(it);
	}
}



void Layout::Stack::Orientation(EOrientation value)
{
	m_eOrientation = value;

	if (m_eOrientation == EOrientation::Horizontal) {
		switch (m_eFlow) {
		case EFlow::LeftToRight:
		case EFlow::RightToLeft:
			break;

		default:
			m_eFlow = EFlow::LeftToRight;
			break;
		}
	}
	else {
		switch (m_eFlow) {
		case EFlow::TopToBottom:
		case EFlow::BottomToTop:
			break;

		default:
			m_eFlow = EFlow::TopToBottom;
			break;
		}
	}
}



void Layout::Stack::Flow(EFlow value)
{
	m_eFlow = value;

	if (m_eOrientation == EOrientation::Horizontal) {
		switch (m_eFlow) {
		case EFlow::LeftToRight:
		case EFlow::RightToLeft:
			break;

		default:
			DEBUG_STOP;
			m_eFlow = EFlow::LeftToRight;
			break;
		}
	}
	else {
		switch (m_eFlow) {
		case EFlow::TopToBottom:
		case EFlow::BottomToTop:
			break;

		default:
			DEBUG_STOP;
			m_eFlow = EFlow::TopToBottom;
			break;
		}
	}
}
