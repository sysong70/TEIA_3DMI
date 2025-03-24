#include "stdafx.h"

#include "Ast.h"
#include "Ctl.ToolBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

class Button : public CBCGPButton
{
public:

	static const COLORREF clrDefault = (COLORREF)-1;

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

	CMenu Menu;

public:

	void AddMenu(const Ctl::ResourceIds& menus)
	{
		BOOL success = Menu.CreateMenu();
		ASSERT(success);

		for (auto id : menus) {
			if (id == 0) {
				success = Menu.AppendMenu(MF_SEPARATOR);
			}
			else {
				success = Menu.AppendMenu(MF_STRING, id, Ast::GetTitle(id));
			}

			ASSERT(success);
		}

		m_bOSMenu = FALSE;
		m_hMenu = Menu.m_hMenu;
		DEBUG_VALID(m_hMenu);
	}

	void CheckMenu(UINT id, bool radioType)
	{
		if (radioType) {
			for (int i = 0; i < Menu.GetMenuItemCount(); i++) {
				UINT itemId = Menu.GetMenuItemID(i);
				Menu.CheckMenuItem(itemId, id == itemId ? MF_CHECKED : MF_UNCHECKED);
			}
		}
		else {
			Menu.CheckMenuItem(id, MF_CHECKED);
		}
	}
};

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlToolBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	// button click event of all button
	ON_CONTROL_RANGE(BN_CLICKED, COMMAND_START, COMMAND_END, OnCommand)
END_MESSAGE_MAP()



CtlToolBar::~CtlToolBar()
{
	for (auto holder : Buttons) {
		REMOVE_POINTER(holder);
	}
	Buttons.clear();

	DestroyWindow();
}



bool CtlToolBar::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, rect, pParentWnd, id) == FALSE) {
		RETURN_FALSE;
	}

	return true;
}



void CtlToolBar::SetPivot(Ctl::EPivot pivot, bool expandSize)
{
	Pivot = pivot;
	ExpandSize = expandSize;
}

//--------------------------------------------------------------------------------------------------

CBCGPButton* CtlToolBar::AddButton(UINT id, bool menu)
{
	CBCGPButton* button = CreateButton(id, menu);
	Buttons.push_back(button);

	return button;
}



void CtlToolBar::AddButtons(const Ctl::ResourceIds& ids)
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



void CtlToolBar::AddSeperator()
{
	Buttons.push_back(nullptr);
}

//--------------------------------------------------------------------------------------------------

CBCGPButton* CtlToolBar::AddToggle(UINT id, bool checked)
{
	CBCGPButton* button = CreateButton(id, false, true, checked);
	Buttons.push_back(button);

	return button;
}



bool CtlToolBar::GetCheck(UINT id)
{
	CBCGPButton* pButton = GetButton(id);
	if (pButton != nullptr) {
		return pButton->IsChecked();
	}

	return false;
}



void CtlToolBar::SetCheck(UINT id, bool value, bool uncheckOthers)
{
	if (uncheckOthers) {
		for (auto button : Buttons) {
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



void CtlToolBar::SetUncheckOthers(UINT id)
{
	for (auto button : Buttons) {
		if (button != nullptr && button->GetDlgCtrlID() != id) {
			button->SetCheck(BST_UNCHECKED);
		}
	}
}

//--------------------------------------------------------------------------------------------------

CBCGPButton* CtlToolBar::AddButtonWithMenu(UINT id, const Ctl::ResourceIds& menus)
{
	ButtonWithMenu* button = (ButtonWithMenu*)CreateButton(id, true);
	Buttons.push_back(button);

	//button->m_bRightArrow = TRUE;
	button->AddMenu(menus);

	return button;
}



void CtlToolBar::CheckMenu(UINT id, UINT menuId, bool radioType)
{
	ButtonWithMenu* button = (ButtonWithMenu*)GetButton(id);
	button->CheckMenu(menuId, radioType);
}



UINT CtlToolBar::GetMenuResult(UINT id)
{
	ButtonWithMenu* button = (ButtonWithMenu*)GetButton(id);
	return button->m_nMenuResult;
}

//--------------------------------------------------------------------------------------------------

CSize CtlToolBar::AdjustLayout()
{
	if (Buttons.size() == 0 || GetSafeHwnd() == nullptr || GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	const int arrowWidth = globalUtils.ScaleByDPI(10);

	CSize buttonSize = GetButtonSize();
	CSize buttonMargin = GetButtonMargin();
	CSize padding = GetPadding();
	CPoint offset(padding.cx, padding.cy);
	CSize size;

	if (IsHorizontal()) {
		for (auto button : Buttons) {
			if (button != nullptr) {
				int width = buttonSize.cx + (dynamic_cast<CBCGPMenuButton*>(button) == nullptr ? 0 : arrowWidth);
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
		for (auto button : Buttons) {
			if (button != nullptr) {
				int height = buttonSize.cy + (dynamic_cast<CBCGPMenuButton*>(button) == nullptr ? 0 : arrowWidth);
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

	if (ExpandSize) {
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



CPoint CtlToolBar::AdjustLocation(CSize size)
{
	using namespace Ctl;

	CRect parentArea;
	GetParent()->GetClientRect(&parentArea);

	CSize margin; // WARNING
	CPoint pivot;

	switch (Pivot) {
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



CBCGPButton* CtlToolBar::GetButton(UINT id)
{
	for (auto button : Buttons) {
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			return button;
		}
	}

	RETURN_NULL;
}



void CtlToolBar::SetSize(CSize buttonSize, CSize buttonMargin, CSize imageSize, CSize seperatorMargin, CSize toolBarPadding)
{
	ButtonSize = buttonSize;
	ButtonMargin = buttonMargin;
	ImageSize = imageSize;
	SeperatorMargin = seperatorMargin;
	ToolBarPadding = toolBarPadding;
}



void CtlToolBar::SetSize(EIcon size)
{
	switch (size) {
	case EIcon::Small:
		ImageSize = CSize(16, 16);
		break;

	case EIcon::Large:
		ImageSize = CSize(32, 32);
		break;

	case EIcon::Medium:
	default:
		ImageSize = CSize(24, 24);
		break;
	}
}

//--------------------------------------------------------------------------------------------------

void CtlToolBar::OnCommand(UINT id)
{
	GetParent()->SendMessage(WM_COMMAND, (WPARAM)id);
	GetParent()->SetFocus();
}



LRESULT CtlToolBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	// TODO

	return result;
}



BOOL CtlToolBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Ctl::EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void CtlToolBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		AdjustLayout();
	}
}

//--------------------------------------------------------------------------------------------------

CBCGPButton* CtlToolBar::CreateButton(UINT id, bool menu, bool toggle, bool checked)
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
	button.SetBitmap(Ast::CreateBitmap(id, GetImageSize()));
	button.SetTooltip(Ast::GetTitle(id));

	if (toggle) {
		button.SetCheck(checked ? BST_CHECKED : BST_UNCHECKED);
	}

	return pButton;
}



bool CtlToolBar::IsHorizontal()
{
	return !(Pivot == Ctl::EPivot::MiddleLeft || Pivot == Ctl::EPivot::MiddleRight);
}



CSize CtlToolBar::GetButtonSize()
{
	return globalUtils.ScaleByDPI(ButtonSize);
}



CSize CtlToolBar::GetButtonMargin()
{
	return globalUtils.ScaleByDPI(ButtonMargin);
}



CSize CtlToolBar::GetImageSize()
{
	return globalUtils.ScaleByDPI(ImageSize);
}



CSize CtlToolBar::GetSeperatorMargin()
{
	return globalUtils.ScaleByDPI(SeperatorMargin);
}



CSize CtlToolBar::GetPadding()
{
	return globalUtils.ScaleByDPI(ToolBarPadding);
}

//**************************************************************************************************

void CtlHistoryBar::PushButton(UINT id)
{
	RemoveButton(id);

	if (Buttons.size() >= MaxCount) {
		REMOVE_POINTER(Buttons.back());
		Buttons.pop_back();
	}

	Buttons.push_back(CreateButton(id, false, false));
	AdjustLayout();
	ShowWindow(SW_SHOW);
}



void CtlHistoryBar::RemoveButton(UINT id)
{
	std::vector<CBCGPButton*>::iterator it;
	for (it = Buttons.begin(); it != Buttons.end(); it++) {
		auto button = *it;
		if (button != nullptr && button->GetDlgCtrlID() == id) {
			REMOVE_POINTER(button);
			Buttons.erase(it);
			break;
		}
	}
}
