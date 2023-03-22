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



int Layout::Stack::CalculateWidth()
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

		cx += m_padding.left + m_padding.right;
		cx += m_border.left + m_border.right;
	}

	cx += m_margin.left + m_margin.right;

	return cx;
}



int Layout::Stack::CalculateHeight()
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

		cy += m_padding.top + m_padding.bottom;
		cy += m_border.top + m_border.bottom;
	}

	cy += m_margin.top + m_margin.bottom;

	return cy;
}



void Layout::Stack::Update(CRect parent)
{
	CPoint point = CPoint(parent.left, parent.top) + CPoint(m_margin.left, m_margin.top);
	CSize size = parent.Size();

	if (m_eOrientation == EOrientation::Horizontal) {
		for (auto child : m_children) {
			CRect childRect = child->CalculateRect(EBoxModel::Border);

			childRect.MoveToXY(point);
			MoveWindow(childRect, FALSE);
		}
	}
	else {
	}
}
