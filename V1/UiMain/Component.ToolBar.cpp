#include "stdafx.h"
#include "resource.h"
#include "Component.ToolBar.h"
#include "Facility.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetToolBar

namespace PresetToolBar
{
	const UINT Id = WM_USER;

	CSize ButtonSize()
	{
		return globalUtils.ScaleByDPI(CSize(28, 28));
	}

	CSize ButtonMargin()
	{
		return globalUtils.ScaleByDPI(CSize(2, 2));
	}

	CSize ImageSize()
	{
		return globalUtils.ScaleByDPI(CSize(24, 24));
	}

	CSize SeperatorMargin()
	{
		return globalUtils.ScaleByDPI(CSize(6, 6));
	}

	CSize ToolBarPadding()
	{
		return globalUtils.ScaleByDPI(CSize(3, 3));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(ToolBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	// button click event of all button
	ON_CONTROL_RANGE(BN_CLICKED, COMMAND_START, COMMAND_END, OnCommand)
END_MESSAGE_MAP()



Component::ToolBar::ToolBar()
{
}



Component::ToolBar::~ToolBar()
{
	for (auto holder : m_buttons) {
		REMOVE_POINTER(holder);
	}
	m_buttons.clear();
}



void Component::ToolBar::Initialize(CWnd* pParentWnd, const RECT& rect)
{
	__super::Create(nullptr, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, rect, pParentWnd, PRESET::Id);
}



void Component::ToolBar::SetPivot(EPivot pivot, bool expandSize)
{
	m_ePivot = pivot;
	m_bExpandSize = expandSize;
}



void Component::ToolBar::ViewChanged(UINT message, Window::View* pView)
{
	if (message == WM_CREATE || pView->GetSafeHwnd() == nullptr) {
		return;
	}

	if (message == WM_ACTIVATE) {
		ShowWindow(SW_HIDE);
		//:WARNING - not SetOwner()
		SetParent((CWnd*)pView);
		AdjustLayout();
		ShowWindow(SW_SHOW);
	}
}



CBCGPButton* Component::ToolBar::AddButton(UINT id, bool menu)
{
	CBCGPButton* button = CreateButton(id, menu);
	m_buttons.push_back(button);

	return button;
}



void Component::ToolBar::AddButtons(std::vector<UINT> ids)
{
	for (auto id : ids) {
		if (id != 0) {
			AddButton(id);
		}
		else {
			AddSeperator();
		}
	}
}



void Component::ToolBar::AddSeperator()
{
	m_buttons.push_back(nullptr);
}



CSize Component::ToolBar::AdjustLayout()
{
	const int ArrowWidth = 18;

	if (m_buttons.size() == 0 || GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	CSize buttonSize = PRESET::ButtonSize();
	CSize buttonMargin = PRESET::ButtonMargin();
	CSize padding = PRESET::ToolBarPadding();
	CPoint offset(padding.cx, padding.cy);
	CSize size;

	if (IsHorizontal()) {
		for (auto button : m_buttons) {
			if (button != nullptr) {
				int width = buttonSize.cx + (dynamic_cast<CBCGPMenuButton*>(button) == nullptr ? 0 : globalUtils.ScaleByDPI(ArrowWidth));
				button->MoveWindow(offset.x, offset.y, width, buttonSize.cy);
				offset.x += width + buttonMargin.cx;
			}
			else {
				offset.x += PRESET::SeperatorMargin().cx;
			}
		}

		size.cx = offset.x - buttonMargin.cx + padding.cx;
		size.cy = offset.y + buttonSize.cy + padding.cy;
	}
	else {
		for (auto button : m_buttons) {
			if (button != nullptr) {
				int height = buttonSize.cy + (dynamic_cast<CBCGPMenuButton*>(button) == nullptr ? 0 : globalUtils.ScaleByDPI(ArrowWidth));
				button->MoveWindow(offset.x, offset.y, buttonSize.cx, height);
				offset.y += height + buttonMargin.cy;
			}
			else {
				offset.y += PRESET::SeperatorMargin().cy;
			}
		}

		size.cx = offset.x + buttonSize.cx + padding.cx;
		size.cy = offset.y - buttonMargin.cy + padding.cy;
	}

	if (m_bExpandSize) {
		CRect parentArea;
		GetParent()->GetClientRect(parentArea);

		if (IsHorizontal()) {
			size.cx = parentArea.Width();
		}
		else {
			size.cy = parentArea.Height();
		}
	}

	CPoint pivot = AdjustLocation(size);
	SetWindowPos(nullptr, pivot.x, pivot.y, size.cx, size.cy, SWP_NOACTIVATE);

	return size;
}



CPoint Component::ToolBar::AdjustLocation(CSize size)
{
	CRect parentArea;
	GetParent()->GetClientRect(&parentArea);

	CSize margin; //:WARNING
	CPoint pivot;

	switch (m_ePivot) {
	case EPivot::TopLeft:
		pivot.x = margin.cx;
		pivot.y = margin.cy;
		break;

	case EPivot::TopCenter:
		pivot.x = parentArea.CenterPoint().x - (size.cx / 2);
		pivot.y = margin.cy;
		break;

	case EPivot::TopRight:
		pivot.x = parentArea.right - size.cx;
		pivot.y = margin.cy;
		break;

	case EPivot::MiddleLeft:
		pivot.x = margin.cx;
		pivot.y = parentArea.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::MiddleRight:
		pivot.x = parentArea.right - size.cx - margin.cx;
		pivot.y = parentArea.CenterPoint().y - (size.cy / 2);
		break;

	case EPivot::BottomLeft:
		pivot.x = margin.cx;
		pivot.y = parentArea.bottom - size.cy - margin.cy;
		break;

	case EPivot::BottomCenter:
		pivot.x = parentArea.CenterPoint().x - (size.cx / 2);
		pivot.y = parentArea.bottom - size.cy - margin.cy;
		break;

	case EPivot::BottomRight:
		pivot.x = parentArea.right - size.cx - margin.cx;
		pivot.y = parentArea.bottom - size.cy - margin.cy;
		break;

	default:
		DEBUG_STOP;
		break;
	}

	return pivot;
}



CBCGPButton* Component::ToolBar::GetButton(UINT id)
{
	for (auto button : m_buttons) {
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			return button;
		}
	}

	RETURN_NULL;
}



void Component::ToolBar::PostNcDestroy()
{
	__super::PostNcDestroy();
}



void Component::ToolBar::OnCommand(UINT id)
{
	GetParent()->SendMessage(WM_COMMAND, (WPARAM)id);
}



LRESULT Component::ToolBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL Component::ToolBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Component::ToolBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		AdjustLayout();
	}
}



CBCGPButton* Component::ToolBar::CreateButton(UINT id, bool menu)
{
	CBCGPButton* pButton = menu ? new CBCGPMenuButton() : new CBCGPButton();
	DEBUG_VALID(pButton);

	CBCGPButton& button = *pButton;
	button.m_nFlatStyle = CBCGPButton::BUTTONSTYLE_NOBORDERS;
	button.m_bDrawFocus = FALSE;
	button.m_bVisualManagerStyle = TRUE;

	button.Create(L"", BS_PUSHBUTTON | BS_ICON | WS_CHILD | WS_VISIBLE, {}, this, id);
	button.SetBitmap(Facility::CreateBitmap(id, PRESET::ImageSize()));
	button.SetTooltip(Facility::GetTitle(id));

	return pButton;
}



bool Component::ToolBar::IsHorizontal()
{
	return !(m_ePivot == EPivot::MiddleLeft || m_ePivot == EPivot::MiddleRight);
}

#undef PRESET
