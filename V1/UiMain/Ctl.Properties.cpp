#include "stdafx.h"

#include "Ast.h"
#include "Ctl.Properties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

// in BCGPPropList.cpp
#define PROP_HAS_LIST 0x0001
#define ICON_PADDING(parent) globalUtils.ScaleByDPI(3, parent)

using namespace Prop;

//**************************************************************************************************

#pragma region DurationCtrl Class

BEGIN_MESSAGE_MAP(DurationCtrl, CBCGPDurationCtrl)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



void Prop::DurationCtrl::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	CBCGPPopupWindow::CloseActivePopup();

	CBCGPPropList* pParent = DYNAMIC_DOWNCAST(CBCGPPropList, GetParent());
	if (pParent == nullptr || pNewWnd->GetSafeHwnd() == pParent->GetSafeHwnd()) {
		return;
	}

	CBCGPProp* pSel = pParent->GetCurSel();
	if (pSel != nullptr && pSel->IsInPlaceEditing() && pSel->IsEnabled()) {
		if (pParent->EndEditItem() == FALSE) {
			SetFocus();
		}
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region IconComboBoxCtrl Class

BEGIN_MESSAGE_MAP(IconComboBoxCtrl, CBCGPComboBox)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()



Prop::IconComboBoxCtrl::IconComboBoxCtrl(CBCGPToolBarImages& imageListIcons, CStringList& lstIconNames)
	: Icons(imageListIcons)
	, IconNames(lstIconNames)
{
	m_bVisualManagerStyle = TRUE;
}



Prop::IconComboBoxCtrl::~IconComboBoxCtrl()
{
}



void Prop::IconComboBoxCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	ASSERT_VALID(pDC);

	CRect rect = lpDIS->rcItem;
	rect.left += ICON_PADDING(this);

	int index = lpDIS->itemID;
	if (index < 0) {
		return;
	}

	COLORREF clrText = OnFillLbItem(pDC, index, rect, lpDIS->itemState & ODS_SELECTED, lpDIS->itemState & ODS_SELECTED);

	Icons.DrawEx(pDC, rect, index, CBCGPToolBarImages::ImageAlignHorzLeft, CBCGPToolBarImages::ImageAlignVertCenter);

	if (IconNames.IsEmpty() == false) {
		CString name = IconNames.GetAt(IconNames.FindIndex(index));
		CBCGPFontSelector fs(*pDC, &globalUtils.GetFontRegular(this), clrText, TRUE);
		CRect rectText = rect;
		rectText.left += Icons.GetImageSize().cx + ICON_PADDING(this);

		pDC->DrawText(name, rectText, DT_SINGLELINE | DT_VCENTER);
	}
}



void Prop::IconComboBoxCtrl::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	int nTextWidth = 0;
	int nTextHeight = 0;

	if (IconNames.IsEmpty() == false) {
		nTextHeight = globalUtils.GetTextHeight(this);

		CBCGPClientDC dc(this);
		CBCGPFontSelector fs(dc, &globalUtils.GetFontRegular(this));

		for (POSITION pos = IconNames.GetHeadPosition(); pos != nullptr;) {
			CString name = IconNames.GetNext(pos);
			nTextWidth = max(nTextWidth, dc.GetTextExtent(name).cx + ICON_PADDING(this));
		}
	}

	lpMeasureItemStruct->itemWidth = Icons.GetImageSize().cx + nTextWidth + 2 * ICON_PADDING(this);
	lpMeasureItemStruct->itemHeight = max(nTextHeight, Icons.GetImageSize().cy + 2 * ICON_PADDING(this));
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region SliderCtrl Class

BEGIN_MESSAGE_MAP(SliderCtrl, CBCGPSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL_REFLECT()
END_MESSAGE_MAP()



Prop::SliderCtrl::SliderCtrl(Slider* pProp, COLORREF clrBack)
{
	m_bVisualManagerStyle = TRUE;
	m_bDrawFocus = FALSE;

	PropPtr = pProp;
	BkColor = clrBack;
	BkBrush.CreateSolidBrush(BkColor);
}



Prop::SliderCtrl::~SliderCtrl()
{
}



HBRUSH Prop::SliderCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkColor(BkColor);

	return BkBrush;
}



void Prop::SliderCtrl::HScroll(UINT /*nSBCode*/, UINT /*nPos*/)
{
	ASSERT_VALID(PropPtr);

	PropPtr->OnUpdateValue();
	PropPtr->Redraw();
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Color Class

Prop::Color::Color(const CString& name, const COLORREF& color, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CBCGPColorProp(name, 0, color, NULL, lpszDescr)
{
}



BOOL Prop::Color::IsDroppedDown() const
{
	return m_pPopup != NULL;
}



void Prop::Color::OnCloseCombo()
{
	DEBUG_VALID(m_pPopup);

	m_pPopup->ShowWindow(SW_HIDE);
	m_pPopup = NULL;

	__super::OnCloseCombo();
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Coordinate

#include "Cmd.Resource.h"
#include "Ctl.CoordEdit.h"

Prop::Coordinate::Coordinate(const CString& name, const CString& value, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, 0, (LPCTSTR)value, lpDescr)
{
	LoadImages();

	// TODO - 3d options

	//std::vector<UINT> ids = {
	//	HOME_2D_CMD_ObjectSnap_Point,
	//	HOME_2D_CMD_ObjectSnap_End,
	//	HOME_2D_CMD_ObjectSnap_Mid,
	//	HOME_2D_CMD_ObjectSnap_Intersection,
	//	HOME_2D_CMD_ObjectSnap_Perpendicular,
	//	HOME_2D_CMD_ObjectSnap_Center,
	//	HOME_2D_CMD_ObjectSnap_Quadrant,
	//	HOME_2D_CMD_ObjectSnap_Near,
	//};

	//Ast::GetToolBarImages(m_icons, Ctl::ImageSize(), ids);

	//for (auto id : ids) {
	//	m_iconNames.AddTail(Ast::GetTitle(id));
	//	AddOption(Ast::GetTitle(id));
	//}
}



void Prop::Coordinate::AdjustButtonRect()
{
	__super::AdjustButtonRect();

	m_rectButton.left -= m_rectButton.Width();
}



CComboBox* Prop::Coordinate::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int height = 400;

	rect.bottom = rect.top + height;

	IconComboBoxCtrl* pControl = new IconComboBoxCtrl(Icons, IconNames);
	DEBUG_VALID(pControl);

	DWORD dwStyle = WS_CHILD | WS_VSCROLL | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
	if (pControl->Create(dwStyle, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	return pControl;
}



CWnd* Prop::Coordinate::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	CtlCoordEdit* pControl = new CtlCoordEdit();
	DEBUG_VALID(pControl);

	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (pControl->Create(dwStyle, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	pControl->EnableWindow(m_bEnabled);
	pControl->SetOriginalValue((CString)m_varValue);

	bDefaultFormat = FALSE;

	//CWnd* pControl = CBCGPProp::CreateInPlaceEdit(rectEdit, bDefaultFormat);
	//if (pControl != nullptr) {
	//	pControl->ShowWindow(SW_HIDE);
	//}

	return pControl;
}



BOOL Prop::Coordinate::HasButton() const
{
	return TRUE;
}



void Prop::Coordinate::OnClickButton(CPoint point)
{
	bool isLeft = point.x < m_rectButton.CenterPoint().x;

	ClickedButton = (isLeft ? 0 : 1);
	RedrawButton();

	BCGPMessageBox(isLeft ? L"Object snap button" : L"Point picking button");

	ClickedButton = -1;
	RedrawButton();
}



void Prop::Coordinate::OnDPIChanged(UINT nDPIOld, UINT nDPINew)
{
	__super::OnDPIChanged(nDPIOld, nDPINew);

	LoadImages();
}



void Prop::Coordinate::OnDrawButton(CDC* pDC, CRect rectButton)
{
	CSize imageSize = ButtonImages.GetImageSize();

	for (int i = 0; i < 2; i++) {
		CBCGPToolbarButton button;
		CRect rect = rectButton;

		if (i == 0) {
			rect.right = rect.left + rect.Width() / 2;
		}
		else {
			rect.left = rect.right - rect.Width() / 2;
		}

		CBCGPVisualManager::BCGBUTTON_STATE state =
			(ClickedButton == i ? CBCGPVisualManager::ButtonsIsHighlighted : CBCGPVisualManager::ButtonsIsRegular);

		CBCGPVisualManager::GetInstance()->OnFillButtonInterior(pDC, &button, rect, state);
		ButtonImages.DrawEx(pDC, rect, i, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		CBCGPVisualManager::GetInstance()->OnDrawButtonBorder(pDC, &button, rect, state);
	}
}



BOOL Prop::Coordinate::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CtlCoordEdit* pControl = (CtlCoordEdit*)m_pWndInPlace;
	ASSERT_VALID(pControl);

	CString oldValue = m_varValue;
	CString newValue = pControl->UpdateValue();
	m_varValue = (LPCTSTR)newValue;

	if (oldValue != newValue) {
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}



void Prop::Coordinate::LoadImages()
{
	ButtonImages.Clear();
	ButtonImages.SetTransparentColor(RGB(255, 0, 255));

	Ast::GetToolBarImages(ButtonImages, globalUtils.ScaleByDPI(CSize(16, 16)), {
		HOME_2D_POP_ObjectSnap, HOME_2D_CMD_Select_Point
	});
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region ComboButton Class

Prop::ComboButton::ComboButton(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
}



void Prop::ComboButton::AdjustButtonRect()
{
	__super::AdjustButtonRect();

	if (m_dwFlags & PROP_HAS_LIST) {
		m_rectButton.left -= m_rectButton.Width();
	}
}



//LPCTSTR Prop::ComboButton::GetXMLTagName() const
//{
//	return _T("comboButton");
//}



BOOL Prop::ComboButton::HasButton() const
{
	return TRUE;
}



void Prop::ComboButton::OnClickButton(CPoint point)
{
	bool isLeft = point.x < m_rectButton.CenterPoint().x;

	if (isLeft) {
		__super::OnClickButton(point);
	}
	else {
		DEBUG_STOP;
		// TODO - Display your dialog here...
		SetValue(_T("New value"));
	}
}



void Prop::ComboButton::OnDrawButton(CDC* pDC, CRect rectButton)
{
	int center = rectButton.CenterPoint().x;

	for (int i = 0; i < 2; i++) {
		CRect rect = rectButton;

		if (i == 0) {
			if (m_dwFlags & PROP_HAS_LIST) {
				rect.right = center - 1;

				// Draw combobox button at left
				__super::OnDrawButton(pDC, rect);
			}
		}
		else {
			if (m_dwFlags & PROP_HAS_LIST) {
				rect.left = center;
			}

			// Draw push button at right
			if (m_strButtonText.IsEmpty() == false) {
				rect.DeflateRect(2, 2);
				COLORREF clrText = CBCGPVisualManager::GetInstance()->OnDrawPropListPushButton(
					pDC, rect, this, m_pWndList->DrawControlBarColors(), m_bButtonIsFocused, m_bEnabled, m_bButtonIsDown, m_bButtonIsHighlighted);
				COLORREF clrTextOld = pDC->SetTextColor(clrText);

				pDC->DrawText(m_strButtonText, rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				pDC->SetTextColor(clrTextOld);
			}
		}
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region CommandButton Class

Prop::CommandButton::CommandButton(const CString& name, const CString& title, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, 0, (LPCTSTR)title, lpDescr)
	, Title(title)
{
}



BOOL Prop::CommandButton::OnClickValue(UINT uiMsg, CPoint point)
{
	m_pWndList->OnPropertyChanged(this);

	return TRUE;
}



void Prop::CommandButton::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pWndList);

#define TEXT_MARGIN(parent)	globalUtils.ScaleByDPI(4, parent)

	COLORREF oldColor;
	if (Highlighted && m_bEnabled) {
		oldColor = pDC->SetTextColor((COLORREF)Ctl::EColor::DeepSkyBlue);
	}
	else {
		oldColor = pDC->SetTextColor((COLORREF)Ctl::EColor::NavyBlue);
	}

	rect.DeflateRect(TEXT_MARGIN(m_pWndList), 0);
	UINT uiDTFlags = DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER;
	pDC->DrawText(Title, rect, uiDTFlags);

	pDC->SetTextColor(oldColor);

#undef TEXT_MARGIN
}



BOOL Prop::CommandButton::OnEdit(LPPOINT lptClick)
{
	m_pWndList->OnPropertyChanged(this);

	return FALSE;
}



void Prop::CommandButton::OnLeaveMouse()
{
	Highlighted = false;

	Redraw();
}



BOOL Prop::CommandButton::OnSetCursor() const
{
	return FALSE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region CustomColor Class

Prop::CustomColor::CustomColor(const CString& name, const COLORREF& color, LPCTSTR lpDescr)
	: CBCGPColorProp(name, color, NULL, lpDescr)
{
	DEBUG_STOP;
	// WARNING - localization
	EnableOtherButton(_T("Other..."));
	EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
}



//LPCTSTR Prop::CustomColor::GetXMLTagName() const
//{
//	return _T("customColor");
//}



BOOL Prop::CustomColor::OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL /*bIsAutomatic*/, BOOL /*bIsSelected*/, BOOL /*bOnPopupMenu*/)
{
	CBCGPDrawManager dm(*pDC);
	dm.DrawEllipse(rectColor, color, globalData.clrBarDkShadow);

	return TRUE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region CustomDescription Class

Prop::CustomDescription::CustomDescription(const CString& name, const _variant_t& value, LPCTSTR lpDescr)
	: CBCGPProp(name, value, lpDescr)
{
}



//LPCTSTR Prop::CustomDescription::GetXMLTagName() const
//{
//	return _T("customDescription");
//}



void Prop::CustomDescription::OnDrawDescription(CDC* pDC, CRect rect)
{
	DEBUG_STOP;

	CBCGPDrawManager dm(*pDC);
	dm.FillGradient2(rect, RGB(102, 200, 238), RGB(0, 129, 185), 45);

	CBCGPFontSelector fs(*pDC, &globalUtils.GetFontBold(m_pWndList));

	pDC->SetTextColor(RGB(0, 65, 117));
	pDC->DrawText(m_strDescr, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	rect.OffsetRect(-2, -2);
	pDC->SetTextColor(RGB(155, 251, 255));
	pDC->DrawText(m_strDescr, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region CustomDialog Class

Prop::CustomDialog::CustomDialog(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
}



//LPCTSTR Prop::CustomDialog::GetXMLTagName() const
//{
//	return _T("customDialog");
//}



void Prop::CustomDialog::OnClickButton(CPoint point)
{
	DEBUG_STOP;
	// TODO - Display your dialog here...
	SetValue(_T("New value"));
}



BOOL Prop::CustomDialog::HasButton() const
{
	return TRUE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region CustomState Class

Prop::CustomState::CustomState(const CString & name, LPCTSTR lpValue, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, lpValue, lpDescr, data)
{
	m_bAllowEdit = FALSE;
	m_strButtonText = _T("II");

	Images.SetImageSize(CSize(14, 14));
	DEBUG_STOP;
	//m_images.Load(IDB_PROP_LIST_ANIMATION);

	SetState(NULL, L' ', RGB(0, 0, 0), FALSE, 16);
}



void Prop::CustomState::StepAnimation()
{
	if (Stopped) {
		return;
	}

	AnimationStep++;

	if (AnimationStep >= Images.GetCount()) {
		AnimationStep = 0;
	}

	RedrawState();
}



//LPCTSTR Prop::CustomState::GetXMLTagName() const
//{
//	return _T("customState");
//}



BOOL Prop::CustomState::HasButton() const
{
	return TRUE;
}



void Prop::CustomState::OnClickButton(CPoint /*point*/)
{
	Stopped = !Stopped;
	m_strButtonText = (Stopped ? _T(">") : _T("II"));

	Redraw();
}



void Prop::CustomState::OnDrawStateIndicator(CDC* pDC, CRect rect)
{
	if (Stopped == false) {
		Images.DrawEx(pDC, rect, AnimationStep, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Duration Class

Prop::Duration::Duration(const CString& name, const COleDateTimeSpan& duration, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, _variant_t((double)duration), lpDescr, data)
{
}



Prop::Duration::~Duration()
{
	CBCGPPopupWindow::CloseActivePopup();
}



void Prop::Duration::SetDuration(COleDateTimeSpan duration)
{
	ASSERT_VALID(this);
	SetValue(_variant_t((double)duration));
}



COleDateTimeSpan Prop::Duration::GetDuration() const
{
	return (COleDateTimeSpan)(double)m_varValue;
}



void Prop::Duration::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	rectSpin.SetRectEmpty();

	rectEdit = m_Rect;
	rectEdit.left = m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth();
}



CWnd* Prop::Duration::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	DurationCtrl* pControl = new DurationCtrl;
	DEBUG_VALID(pControl);

	pControl->EnableVisualManagerStyle();
	pControl->SetAutoResize(FALSE);
	pControl->SetBackgroundColor(m_pWndList->GetBkColor(), FALSE);
	pControl->SetTextColor(m_bEnabled ? m_pWndList->GetTextColor() : globalData.clrGrayedText, FALSE);

	CRect rectSpin;
	AdjustInPlaceEditRect(rectEdit, rectSpin);
	rectEdit.DeflateRect(1, 1);

	pControl->Create(_T(""), WS_CHILD | WS_VISIBLE, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE);
	pControl->SetFont(GetFont());
	pControl->SetState(CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS, CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
	pControl->SetDuration(GetDuration());
	pControl->EnableWindow(m_bEnabled);

	bDefaultFormat = FALSE;

	return pControl;
}



BOOL Prop::Duration::DoPaste()
{
	if (__super::DoPaste() == FALSE) {
		return FALSE;
	}

	if (DurationWnd.GetSafeHwnd() != nullptr) {
		DurationWnd.SetDuration(GetDuration());
		Redraw();
	}

	return TRUE;
}



//LPCTSTR Prop::Duration::GetXMLTagName() const
//{
//	return _T("duration");
//}



void Prop::Duration::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(m_pWndList);

	rect.DeflateRect(1, 2);
	rect.left = m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth() + TEXT_MARGIN + 1;

	DRAWITEMSTRUCT dis;
	::ZeroMemory(&dis, sizeof(DRAWITEMSTRUCT));

	dis.CtlType = ODT_BUTTON;
	dis.hDC = pDC->GetSafeHdc();
	dis.rcItem = rect;

	DurationWnd.SetTextColor(m_bEnabled ? m_pWndList->GetTextColor() : globalData.clrGrayedText, FALSE);

	COLORREF clrFill = GetAlternateColor();
	if (clrFill == CLR_NONE) {
		clrFill = m_pWndList->GetBkColor();
	}

	DurationWnd.SetBackgroundColor(clrFill, FALSE);
	DurationWnd.SetFont(GetFont());
	DurationWnd.DrawItem(&dis);
}



void Prop::Duration::OnKillSelection(CBCGPProp* pNewSel)
{
	Redraw();
}



void Prop::Duration::OnPosSizeChanged(CRect /*rectOld*/)
{
	ASSERT_VALID(m_pWndList);

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect(rectEdit, rectSpin);

	if (DurationWnd.GetSafeHwnd() == nullptr) {
		DurationWnd.SetAutoResize(FALSE);
		DurationWnd.Create(_T(""), WS_CHILD, rectEdit, m_pWndList, (UINT)-1);

		DurationWnd.SetState(
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS,
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
		DurationWnd.SetFont(GetFont());
		DurationWnd.SetDuration(GetDuration());
	}
	else {
		DurationWnd.SetFont(GetFont());
		DurationWnd.SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	}

	DurationWnd.AdjustControl(rectEdit);
}



void Prop::Duration::OnSetSelection(CBCGPProp* pOldSel)
{
	Redraw();
}



BOOL Prop::Duration::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CBCGPDurationCtrl* pDuration = (CBCGPDurationCtrl*)m_pWndInPlace;
	ASSERT_VALID(pDuration);

	COleDateTimeSpan durationOld = GetDuration();
	COleDateTimeSpan durationNew = pDuration->GetDuration();

	if (durationNew.GetDays() > 2) {
		CBCGPPopupWindow::CloseActivePopup();

		CRect rect;
		m_pWndInPlace->GetWindowRect(rect);

		CBCGPPopupWindow* pPopup = new CBCGPPopupWindow();
		DEBUG_VALID(pPopup);

		pPopup->EnableCloseButton(FALSE);
		pPopup->SetCloseOnOwnerChange();
		pPopup->SetRoundedCorners();
		pPopup->SetShadow();
		pPopup->SetSmallCaptionGripper(FALSE);
		pPopup->SetStemLocation(CBCGPPopupWindow::BCGPPopupWindowStemLocation_TopLeft);

		DEBUG_STOP;
		// TODO - localization
		CBCGPPopupWndParams params;
		params.m_strText = _T("The duration should not exceed 2 days. Please correct the entered duration value.");

		int nOffset = globalUtils.ScaleByDPI(5, m_pWndList);
		pPopup->Create(m_pWndList, params, NULL, CPoint(rect.left + nOffset, rect.bottom + nOffset));

		m_pWndInPlace->SetFocus();

		return FALSE;
	}

	m_varValue = _variant_t((double)durationNew);
	DurationWnd.SetDuration(durationNew);

	if (durationOld != durationNew) {
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}



BOOL Prop::Duration::PushChar(UINT nChar)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);
	ASSERT(m_pWndList->GetCurSel() == this);
	ASSERT_VALID(m_pWndInPlace);

	if (m_bEnabled && m_bAllowEdit) {
		m_pWndInPlace->SendMessage(WM_KEYDOWN, (WPARAM)CString((TCHAR)nChar).MakeUpper()[0]);
		return TRUE;
	}

	OnEndEdit();

	if (::GetCapture() == m_pWndList->GetSafeHwnd()) {
		ReleaseCapture();
	}

	return FALSE;
}



void Prop::Duration::SetValue(const _variant_t& varValue)
{
	ASSERT_VALID(this);

	__super::SetValue(varValue);

	if (DurationWnd.GetSafeHwnd() != nullptr) {
		DurationWnd.SetDuration(GetDuration());
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region FoldersDialog Class

#include "Dlg.Folders.h"

Prop::FoldersDialog::FoldersDialog(const CString& name, const CString value, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (LPCTSTR)value, lpDescr, data)
{
	m_bAllowEdit = FALSE;
}



BOOL Prop::FoldersDialog::HasButton() const
{
	return TRUE;
}



void Prop::FoldersDialog::OnClickButton(CPoint point)
{
	DlgFolders dlg;
	dlg.SetValue(GetValue());
	
	if (dlg.DoModal() == IDOK) {
		SetValue((LPCTSTR)dlg.GetValue());
	}
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region FoldersDialog Class

Prop::FontCombo::FontCombo(const CString& name, const CString value, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (LPCTSTR)value, lpDescr, data)
{
	m_dwFlags = PROP_HAS_LIST;
}



CComboBox* Prop::FontCombo::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int height = 400;

	rect.bottom = rect.top + height;

	CBCGPFontComboBox* pControl = new CBCGPFontComboBox;
	DEBUG_VALID(pControl);
	pControl->m_bVisualManagerStyle = TRUE;

	DWORD dwStyle = WS_CHILD | WS_VSCROLL | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
	if (pControl->Create(dwStyle, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	// WARNING - static var, vary slow
	//CBCGPFontComboBox::m_bDrawUsingFont = TRUE;
	pControl->SelectFont((CString)m_varValue);

	return pControl;
}



CWnd* Prop::FontCombo::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	CWnd* pWnd = __super::CreateInPlaceEdit(rectEdit, bDefaultFormat);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}

	return pWnd;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region HexValue Class

Prop::HexValue::HexValue(const CString& name, long lValue, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, lValue, lpDescr, data, NULL, NULL, _T("01234567890ABCDEFabcdef"))
{
}



CString Prop::HexValue::FormatProperty()
{
	ASSERT_VALID(this);

	CString str;
	str.Format(_T("%X"), (long)m_varValue);

	return str;
}



//LPCTSTR Prop::HexValue::GetXMLTagName() const
//{
//	return _T("hex");
//}



BOOL Prop::HexValue::ParseValue(const CString& str)
{
	// CHECK - local or super?
	return __super::TextToVar(str);
}



BOOL Prop::HexValue::SerializeValue(CString& str)
{
	str = __super::FormatProperty();

	return str != L"*** error ***";
}



BOOL Prop::HexValue::TextToVar(const CString& str)
{
	if (str.IsEmpty()) {
		m_varValue = 0l;
		return TRUE;
	}

	long value = 0;
	::_stscanf_s(str, _T("%X"), &value);
	m_varValue = value;

	return TRUE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region IconCombo Class

Prop::IconCombo::IconCombo(const CString& name, const CString& value, LPCTSTR lpDescr, DWORD_PTR data, CBCGPToolBarImages* pImageList)
	: CBCGPProp(name, (LPCTSTR)value, lpDescr, data)
{
	if (pImageList != nullptr) {
		pImageList->ExportToImageList(m_imageList);
	}
}



bool Prop::IconCombo::AddOption(LPCTSTR lpOption, int nIcon, int nIndent)
{
	if (!__super::AddOption(lpOption)) {
		RETURN_FALSE;
	}

	Icons.push_back(nIcon);
	m_indents.push_back(nIndent);

	return true;
}



CComboBox* Prop::IconCombo::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int height = 400;

	rect.bottom = rect.top + globalUtils.ScaleByDPI(height, pWndParent);

	CComboBoxEx* pControl = new CComboBoxEx;
	DEBUG_VALID(pControl);

	if (pControl->Create(WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	if (m_imageList.GetSafeHandle() != nullptr) {
		pControl->SetImageList(&m_imageList);
	}

	return pControl;
}



//LPCTSTR Prop::IconComboBox::GetXMLTagName() const
//{
//	return _T("comboBoxEx");
//}



BOOL Prop::IconCombo::OnEdit(LPPOINT lpClick)
{
	if (__super::OnEdit(lpClick) == FALSE) {
		return FALSE;
	}

	CComboBoxEx* pCombo = DYNAMIC_DOWNCAST(CComboBoxEx, m_pWndCombo);
	if (pCombo == nullptr) {
		RETURN(FALSE);
	}
	pCombo->ResetContent();

	int index = 0;
	COMBOBOXEXITEM item;
	::ZeroMemory(&item, sizeof(item));
	item.mask = CBEIF_IMAGE | CBEIF_INDENT | CBEIF_SELECTEDIMAGE | CBEIF_TEXT;

	for (POSITION pos = m_lstOptions.GetHeadPosition(); pos != nullptr; index++) {
		CString strItem = m_lstOptions.GetNext(pos);

		item.iItem = index;
		item.iSelectedImage = item.iImage = Icons[index];
		item.iIndent = m_indents[index];
		item.pszText = (LPTSTR)(LPCTSTR)strItem;
		item.cchTextMax = strItem.GetLength();

		pCombo->InsertItem(&item);
	}

	return TRUE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region IconList Class

Prop::IconList::IconList(const CString& name, CBCGPToolBarImages& icons, int nSelected, CStringList* pNames, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (long)nSelected, lpDescr, data)
	, Icons(icons)
{
	if (pNames != nullptr) {
		IconNames.AddTail(pNames);
		ASSERT(IconNames.GetCount() == Icons.GetCount());
	}

	for (int i = 0; i < Icons.GetCount(); i++) {
		CString strItem;
		strItem.Format(_T("%d"), i);

		AddOption(strItem);
	}

	AllowEdit(FALSE);
}



CComboBox* Prop::IconList::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int height = 400;

	rect.bottom = rect.top + height;

	IconComboBoxCtrl* pControl = new IconComboBoxCtrl(Icons, IconNames);
	DEBUG_VALID(pControl);

	DWORD dwStyle = WS_CHILD | WS_VSCROLL | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;
	if (pControl->Create(dwStyle, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	return pControl;
}



CWnd* Prop::IconList::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	CWnd* pWnd = __super::CreateInPlaceEdit(rectEdit, bDefaultFormat);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}

	return pWnd;
}



//LPCTSTR Prop::IconList::GetXMLTagName() const
//{
//	return _T("iconList");
//}



void Prop::IconList::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(pDC);

	CString value = (LPCTSTR)(_bstr_t)m_varValue;
	if (value.IsEmpty() || Icons.GetCount() == 0) {
		return;
	}

	int index = -1;
	::_stscanf_s(value, _T("%d"), &index);
	if (index < 0) {
		return;
	}

	CBCGPFontSelector fs(*pDC, m_pWndList->GetFont());

	CRect rectImage = rect;
	rectImage.left += ICON_PADDING(m_pWndList);
	rectImage.right = rectImage.left + Icons.GetImageSize().cx + ICON_PADDING(m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList);

	Icons.DrawEx(pDC, rectImage, index, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);

	if (IconNames.IsEmpty() == false) {
		CString name = IconNames.GetAt(IconNames.FindIndex(index));
		rect.left = rectImage.right + 2 * ICON_PADDING(m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList);

		pDC->DrawText(name, rect, DT_SINGLELINE | DT_VCENTER);
	}

	m_bValueIsTrancated = FALSE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Password Class

Prop::Password::Password(const CString& name, const CString& password, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (LPCTSTR)password, lpDescr, data)
{
}



CWnd* Prop::Password::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD;
	if (!m_bEnabled || !m_bAllowEdit) {
		dwStyle |= ES_READONLY;
	}

	CEdit* pControl = new CEdit;
	DEBUG_VALID(pControl);

	if (pControl->Create(dwStyle, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	}

	pControl->SetPasswordChar(DefaultChar);
	bDefaultFormat = TRUE;

	return pControl;
}



CString Prop::Password::FormatProperty()
{
	if (m_pWndInPlace != nullptr) {
		return __super::FormatProperty();
	}

	CString value = (LPCTSTR)(_bstr_t)m_varValue;

	for (int i = 0; i < value.GetLength(); i++) {
		value.SetAt(i, DefaultChar);
	}

	return value;
}



//LPCTSTR Prop::Password::GetXMLTagName() const
//{
//	return _T("password");
//}



CString Prop::Password::GetXMLValue() const
{
	return CString();
}



BOOL Prop::Password::IsCopyAvailable() const
{
	return FALSE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region RangeValidation Class

Prop::RangeValidation::RangeValidation(const CString& name, long value, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, value, lpDescr, data)
{
}



void Prop::RangeValidation::SetRange(long minValue, long maxValue)
{
	MinValue = minValue;
	MaxValue = maxValue;

	if (MinValue > MaxValue) {
		std::swap(MinValue, MaxValue);
	}
}



//LPCTSTR Prop::RangeValidation::GetXMLTagName() const
//{
//	return _T("rangeValidation");
//}



BOOL Prop::RangeValidation::OnEndEdit()
{
	CleanState();

	return __super::OnEndEdit();
}



BOOL Prop::RangeValidation::OnUpdateValue()
{
	DEBUG_STOP;

	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	long value = ::_ttol(strText);
	if (value < MinValue || value > MaxValue) {
		// TODO - localization
		SetState(L"Incorrect value: the correct value should be between ??? and ???");
		return FALSE;
	}

	CleanState();

	return __super::OnUpdateValue();
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region Slider Class

Prop::Slider::Slider(const CString& name, long value, LPCTSTR lpDescr, DWORD_PTR data) :
	CBCGPProp(name, 0, value, lpDescr, data)
{
	m_bIsVisible = true;
}



void Prop::Slider::SetRange(int minValue, int maxValue, int step)
{
	ASSERT_VALID(this);

	MinValue = minValue;
	MaxValue = maxValue;
	Step = step;
}



CWnd* Prop::Slider::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(this);
	DEBUG_VALID(m_pWndList);

	CBCGPClientDC dc(m_pWndList);

	CString strLabel(L"000000"); // CHECK
	rectEdit.left += dc.GetTextExtent(strLabel).cx;

	SliderCtrl* pControl = new SliderCtrl(this, m_pWndList->GetBkColor());
	DEBUG_VALID(pControl);

	if (pControl->Create(WS_VISIBLE | WS_CHILD | TBS_NOTICKS, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE) == FALSE) {
		REMOVE_POINTER(pControl);
		RETURN_NULL;
	};

	pControl->SetRange(MinValue, MaxValue, TRUE);
	pControl->SetTicFreq(Step);
	pControl->SetLineSize(Step); // move to cursor
	pControl->SetPageSize(Step); // move to PgUp/PgDn

	pControl->SetPos((long)m_varValue);
	pControl->EnableProgressMode();
	pControl->EnableWindow(m_bEnabled);

	bDefaultFormat = TRUE;

	return pControl;
}



BOOL Prop::Slider::OnSetCursor() const
{
	// Use default
	return FALSE;
}



BOOL Prop::Slider::OnClickValue(UINT uiMsg, CPoint point)
{
	return TRUE;
}



//LPCTSTR Prop::Slider::GetXMLTagName() const
//{
//	return _T("slider");
//}



BOOL Prop::Slider::OnUpdateValue()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	const long currValue = m_varValue;
	m_varValue = (long)((CBCGPSliderCtrl*)m_pWndInPlace)->GetPos();

	if (currValue != (long)m_varValue) {
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}

#pragma endregion // REGION

//**************************************************************************************************

#pragma region TwoButtons Class

Prop::TwoButtons::TwoButtons(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
	LoadImages();
	ClickedButton = -1;
}



void Prop::TwoButtons::AdjustButtonRect()
{
	__super::AdjustButtonRect();

	m_rectButton.left -= m_rectButton.Width();
}



//LPCTSTR Prop::TwoButtons::GetXMLTagName() const
//{
//	return _T("twoButtons");
//}



BOOL Prop::TwoButtons::HasButton() const
{
	return TRUE;
}



void Prop::TwoButtons::OnClickButton(CPoint point)
{
	DEBUG_STOP;

	bool isLeft = point.x < m_rectButton.CenterPoint().x;

	ClickedButton = (isLeft ? 0 : 1);
	RedrawButton();

	// TODO

	ClickedButton = -1;
	RedrawButton();
}



void Prop::TwoButtons::OnDPIChanged(UINT nDPIOld, UINT nDPINew)
{
	__super::OnDPIChanged(nDPIOld, nDPINew);

	LoadImages();
}



void Prop::TwoButtons::OnDrawButton(CDC* pDC, CRect rectButton)
{
	CSize imageSize = Images.GetImageSize();

	for (int i = 0; i < 2; i++) {
		CBCGPToolbarButton button;
		CRect rect = rectButton;

		if (i == 0) {
			rect.right = rect.left + rect.Width() / 2;
		}
		else {
			rect.left = rect.right - rect.Width() / 2;
		}

		CBCGPVisualManager::BCGBUTTON_STATE state =
			(ClickedButton == i ? CBCGPVisualManager::ButtonsIsHighlighted : CBCGPVisualManager::ButtonsIsRegular);

		CBCGPVisualManager::GetInstance()->OnFillButtonInterior(pDC, &button, rect, state);
		Images.DrawEx(pDC, rect, i, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		CBCGPVisualManager::GetInstance()->OnDrawButtonBorder(pDC, &button, rect, state);
	}
}



void Prop::TwoButtons::LoadImages()
{
	DEBUG_STOP;

	Images.Clear();
	Images.SetImageSize(CSize(14, 14));
	Images.SetTransparentColor(RGB(255, 0, 255));
	//m_images.Load(IDB_BUTTONS);

	globalUtils.ScaleByDPI(Images, (m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList));
}

#pragma endregion // REGION

#undef PROP_HAS_LIST
#undef ICON_PADDING
