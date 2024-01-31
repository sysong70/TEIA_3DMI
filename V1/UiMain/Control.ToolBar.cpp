#include "stdafx.h"
#include "resource.h"
#include "Control.ToolBar.h"
#include "Facility.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Control;

BEGIN_MESSAGE_MAP(ToolBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	// button click event of all button
	ON_CONTROL_RANGE(BN_CLICKED, COMMAND_START, COMMAND_END, OnCommand)
END_MESSAGE_MAP()



Control::ToolBar::ToolBar()
{
}



Control::ToolBar::~ToolBar()
{
	for (auto holder : m_buttons) {
		REMOVE_POINTER(holder);
	}
	m_buttons.clear();
}



bool Control::ToolBar::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, rect, pParentWnd, id) == FALSE) {
		RETURN_FALSE;
	}

	return true;
}



void Control::ToolBar::SetPivot(EPivot pivot, bool expandSize)
{
	m_ePivot = pivot;
	m_bExpandSize = expandSize;
}



CBCGPButton* Control::ToolBar::AddButton(UINT id, bool menu)
{
	CBCGPButton* button = CreateButton(id, menu);
	m_buttons.push_back(button);

	return button;
}



void Control::ToolBar::AddButtons(std::vector<UINT> ids)
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



void Control::ToolBar::AddSeperator()
{
	m_buttons.push_back(nullptr);
}



CSize Control::ToolBar::AdjustLayout()
{
	const int ArrowWidth = 18;

	if (m_buttons.size() == 0 || GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	CSize buttonSize = GetButtonSize();
	CSize buttonMargin = GetButtonMargin();
	CSize padding = GetPadding();
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
				offset.x += GetSeperatorMargin().cx;
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
				offset.y += GetSeperatorMargin().cy;
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
	SetWindowPos(NULL, pivot.x, pivot.y, size.cx, size.cy, SWP_NOACTIVATE);

	return size;
}



CPoint Control::ToolBar::AdjustLocation(CSize size)
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



CBCGPButton* Control::ToolBar::GetButton(UINT id)
{
	for (auto button : m_buttons) {
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			return button;
		}
	}

	RETURN_NULL;
}



void Control::ToolBar::IsCheckButton(bool value)
{
	m_bCheckButton = value;
}



void Control::ToolBar::SetCheck(UINT id, bool value, bool uncheckOthers)
{
	if (uncheckOthers) {
		for (auto button : m_buttons) {
			if (button != nullptr) {
				button->SetCheck(FALSE);
			}
		}
	}

	for (auto button : m_buttons) {
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			button->SetCheck((BOOL)value);
			return;
		}
	}
}



void Control::ToolBar::SetSize(CSize buttonSize, CSize buttonMargin, CSize imageSize, CSize seperatorMargin, CSize toolBarPadding)
{
	m_buttonSize = buttonSize;
	m_buttonMargin = buttonMargin;
	m_imageSize = imageSize;
	m_seperatorMargin = seperatorMargin;
	m_toolBarPadding = toolBarPadding;
}



void Control::ToolBar::SetSize(EItemSize size)
{
	DEBUG_STOP;

	switch (size) {
	case EItemSize::Small:
		m_imageSize = CSize(16, 16);
		break;

	case EItemSize::Large:
		m_imageSize = CSize(32, 32);
		break;

	case EItemSize::Medium:
	default:
		m_imageSize = CSize(24, 24);
		break;
	}
}



void Control::ToolBar::OnCommand(UINT id)
{
	GetParent()->SendMessage(WM_COMMAND, (WPARAM)id);
}



LRESULT Control::ToolBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL Control::ToolBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Control::ToolBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		AdjustLayout();
	}
}



CBCGPButton* Control::ToolBar::CreateButton(UINT id, bool menu)
{
	CBCGPButton* pButton = menu ? new CBCGPMenuButton() : new CBCGPButton();
	DEBUG_VALID(pButton);

	CBCGPButton& button = *pButton;
	button.m_nFlatStyle = CBCGPButton::BUTTONSTYLE_NOBORDERS;
	button.m_bDrawFocus = FALSE;
	button.m_bVisualManagerStyle = TRUE;

	DWORD dwStyle;
	if (m_bCheckButton) {
		dwStyle = BS_AUTOCHECKBOX | BS_PUSHLIKE | BS_ICON | WS_CHILD | WS_VISIBLE;
	}
	else {
		dwStyle = BS_PUSHBUTTON | BS_ICON | WS_CHILD | WS_VISIBLE;
	}

	button.Create(L"", dwStyle, {}, this, id);
	button.SetBitmap(Facility::CreateBitmap(id, GetImageSize()));
	button.SetTooltip(Facility::GetTitle(id));

	return pButton;
}



bool Control::ToolBar::IsHorizontal()
{
	return !(m_ePivot == EPivot::MiddleLeft || m_ePivot == EPivot::MiddleRight);
}



CSize Control::ToolBar::GetButtonSize()
{
	return globalUtils.ScaleByDPI(m_buttonSize);
}



CSize Control::ToolBar::GetButtonMargin()
{
	return globalUtils.ScaleByDPI(m_buttonMargin);
}



CSize Control::ToolBar::GetImageSize()
{
	return globalUtils.ScaleByDPI(m_imageSize);
}



CSize Control::ToolBar::GetSeperatorMargin()
{
	return globalUtils.ScaleByDPI(m_seperatorMargin);
}



CSize Control::ToolBar::GetPadding()
{
	return globalUtils.ScaleByDPI(m_toolBarPadding);
}
