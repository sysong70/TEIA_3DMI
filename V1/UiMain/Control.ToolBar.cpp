#include "stdafx.h"
#include "resource.h"
#include "Control.ToolBar.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

static const COLORREF clrDefault = (COLORREF)-1;



class Button : public CBCGPButton
{
protected:
	/*
	void DoDrawItem(CDC* pDCPaint, CRect rectClient, UINT itemState) override
	{
		CBCGPWindowDpiState state(this);

		CBCGPMemDC memDC(*pDCPaint, this);
		CDC* pDC = &memDC.GetDC();

		//m_clrText = clrDefault;

		BOOL bDefaultCheckRadio = (m_bCheckButton || m_bRadioButton) && (GetStyle() & BS_PUSHLIKE) == 0;
		double dblScaleRatio = 1.0;

		if (((m_bVisualManagerStyle && !m_bDontSkin) || bDefaultCheckRadio) && !m_bTransparent) {
			if (CBCGPVisualManager::GetInstance()->OnDrawPushButton(pDC, rectClient, this, m_clrText)) {
				const int nPad = globalUtils.ScaleByDPI(2, this);
				rectClient.DeflateRect(nPad, nPad);
			}
		}

		// Draw button content:
		OnDraw(pDC, rectClient, itemState);

		if ((itemState & ODS_FOCUS) && ((itemState & ODS_NOFOCUSRECT) == 0) && m_bDrawFocus) {
			OnDrawFocusRect(pDC, rectClient);
		}
	}*/
};

//**************************************************************************************************

class ButtonWithMenu : public CBCGPMenuButton
{
public:

	void AddMenu(const std::vector<UINT>& menus)
	{
		BOOL success = m_menu.CreateMenu();
		ASSERT(success);

		for (auto id : menus) {
			if (id == 0) {
				success = m_menu.AppendMenu(MF_SEPARATOR);
			}
			else {
				success = m_menu.AppendMenu(MF_STRING, id, Facility::GetTitle(id));
			}

			ASSERT(success);
		}

		m_bOSMenu = FALSE;
		m_hMenu = m_menu.m_hMenu;
		DEBUG_VALID(m_hMenu);
	}



	void CheckMenu(UINT id, bool radioType)
	{
		if (radioType) {
			for (int i = 0; i < m_menu.GetMenuItemCount(); i++) {
				UINT itemId = m_menu.GetMenuItemID(i);
				m_menu.CheckMenuItem(itemId, id == itemId ? MF_CHECKED : MF_UNCHECKED);
			}
		}
		else {
			m_menu.CheckMenuItem(id, MF_CHECKED);
		}
	}

private:

	CMenu m_menu;
};

//**************************************************************************************************

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

//--------------------------------------------------------------------------------------------------

CBCGPButton* Control::ToolBar::AddButton(UINT id, bool menu)
{
	CBCGPButton* button = CreateButton(id, menu);
	m_buttons.push_back(button);

	return button;
}



void Control::ToolBar::AddButtons(const std::vector<UINT>& ids)
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

//--------------------------------------------------------------------------------------------------

CBCGPButton* Control::ToolBar::AddToggle(UINT id, bool checked)
{
	CBCGPButton* button = CreateButton(id, false, true, checked);
	m_buttons.push_back(button);

	return button;
}



bool Control::ToolBar::GetCheck(UINT id)
{
	CBCGPButton* pButton = GetButton(id);
	if (pButton != nullptr) {
		return pButton->IsChecked();
	}

	return false;
}



void Control::ToolBar::SetCheck(UINT id, bool value, bool uncheckOthers)
{
	if (uncheckOthers) {
		for (auto button : m_buttons) {
			if (button != nullptr) {
				button->SetCheck(BST_UNCHECKED);
			}
		}
	}

	CBCGPButton* pButton = GetButton(id);
	if (pButton != nullptr) {
		pButton->SetCheck(value ? BST_CHECKED : BST_UNCHECKED);
	}
}



void Control::ToolBar::SetUncheckOthers(UINT id)
{
	for (auto button : m_buttons) {
		if (button != nullptr && button->GetDlgCtrlID() != id) {
			button->SetCheck(BST_UNCHECKED);
		}
	}
}

//--------------------------------------------------------------------------------------------------

CBCGPButton* Control::ToolBar::AddButtonWithMenu(UINT id, const std::vector<UINT>& menus)
{
	ButtonWithMenu* button = (ButtonWithMenu*)CreateButton(id, true);
	m_buttons.push_back(button);

	//button->m_bRightArrow = TRUE;
	button->AddMenu(menus);

	return button;
}



void Control::ToolBar::CheckMenu(UINT id, UINT menuId, bool radioType)
{
	ButtonWithMenu* button = (ButtonWithMenu*)GetButton(id);
	button->CheckMenu(menuId, radioType);
}



UINT Control::ToolBar::GetMenuResult(UINT id)
{
	ButtonWithMenu* button = (ButtonWithMenu*)GetButton(id);
	return button->m_nMenuResult;
}

//--------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------

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

//--------------------------------------------------------------------------------------------------

CBCGPButton* Control::ToolBar::CreateButton(UINT id, bool menu, bool toggle, bool checked)
{
	CBCGPButton* pButton = nullptr;
	if (menu) {
		pButton = new ButtonWithMenu();
	}
	else {
		pButton = new Button();
	}
	DEBUG_VALID(pButton);

	CBCGPButton& button = *pButton;
	button.m_nFlatStyle = CBCGPButton::BUTTONSTYLE_NOBORDERS;
	button.m_bDrawFocus = FALSE;
	button.m_bVisualManagerStyle = TRUE;

	DWORD dwStyle = (toggle ? BS_AUTOCHECKBOX | BS_PUSHLIKE : 0) | BS_ICON | WS_CHILD | WS_VISIBLE;
	button.Create(L"", dwStyle, {}, this, id);
	button.SetBitmap(Facility::CreateBitmap(id, GetImageSize()));
	button.SetTooltip(Facility::GetTitle(id));

	if (toggle) {
		button.SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
	}

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
