#include "stdafx.h"
#include "Control.Property.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define ICON_PADDING(parent) globalUtils.ScaleByDPI(3, parent)

using namespace Property;



#pragma region DurationCtrl Class

BEGIN_MESSAGE_MAP(DurationCtrl, CBCGPDurationCtrl)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()



void Property::DurationCtrl::OnKillFocus(CWnd* pNewWnd)
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

#pragma endregion //:REGION

#pragma region IconComboBoxCtrl Class

BEGIN_MESSAGE_MAP(IconComboBoxCtrl, CBCGPComboBox)
	ON_WM_DRAWITEM()
	ON_WM_MEASUREITEM()
END_MESSAGE_MAP()



Property::IconComboBoxCtrl::IconComboBoxCtrl(CBCGPToolBarImages& imageListIcons, CStringList& lstIconNames)
	: m_icons(imageListIcons)
	, m_iconNames(lstIconNames)
{
	m_bVisualManagerStyle = TRUE;
}



Property::IconComboBoxCtrl::~IconComboBoxCtrl()
{
}



void Property::IconComboBoxCtrl::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDIS)
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

	m_icons.DrawEx(pDC, rect, index, CBCGPToolBarImages::ImageAlignHorzLeft, CBCGPToolBarImages::ImageAlignVertCenter);

	if (m_iconNames.IsEmpty() == false) {
		CString name = m_iconNames.GetAt(m_iconNames.FindIndex(index));
		CBCGPFontSelector fs(*pDC, &globalUtils.GetFontRegular(this), clrText, TRUE);
		CRect rectText = rect;
		rectText.left += m_icons.GetImageSize().cx + ICON_PADDING(this);

		pDC->DrawText(name, rectText, DT_SINGLELINE | DT_VCENTER);
	}
}



void Property::IconComboBoxCtrl::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	int nTextWidth = 0;
	int nTextHeight = 0;

	if (m_iconNames.IsEmpty() == false) {
		nTextHeight = globalUtils.GetTextHeight(this);

		CBCGPClientDC dc(this);
		CBCGPFontSelector fs(dc, &globalUtils.GetFontRegular(this));

		for (POSITION pos = m_iconNames.GetHeadPosition(); pos != nullptr;) {
			CString name = m_iconNames.GetNext(pos);
			nTextWidth = max(nTextWidth, dc.GetTextExtent(name).cx + ICON_PADDING(this));
		}
	}

	lpMeasureItemStruct->itemWidth = m_icons.GetImageSize().cx + nTextWidth + 2 * ICON_PADDING(this);
	lpMeasureItemStruct->itemHeight = max(nTextHeight, m_icons.GetImageSize().cy + 2 * ICON_PADDING(this));
}

#pragma endregion //:REGION

#pragma region SliderCtrl Class

BEGIN_MESSAGE_MAP(SliderCtrl, CBCGPSliderCtrl)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_HSCROLL_REFLECT()
END_MESSAGE_MAP()



Property::SliderCtrl::SliderCtrl(Slider* pProp, COLORREF clrBack)
{
	m_bVisualManagerStyle = TRUE;
	m_bDrawFocus = FALSE;

	m_pProp = pProp;
	m_clrBack = clrBack;
	m_brBackground.CreateSolidBrush(m_clrBack);
}



Property::SliderCtrl::~SliderCtrl()
{
}



HBRUSH Property::SliderCtrl::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	pDC->SetBkColor(m_clrBack);

	return m_brBackground;
}



void Property::SliderCtrl::HScroll(UINT /*nSBCode*/, UINT /*nPos*/)
{
	ASSERT_VALID(m_pProp);

	m_pProp->OnUpdateValue();
	m_pProp->Redraw();
}

#pragma endregion //:REGION

#pragma region Color Class

Property::Color::Color(const CString& name, UINT id, const COLORREF& color, LPCTSTR lpszDescr, DWORD_PTR dwData)
	: CBCGPColorProp(name, id, color, NULL, lpszDescr)
{
}



BOOL Property::Color::IsDroppedDown() const
{
	return m_pPopup != NULL;
}



void Property::Color::OnCloseCombo()
{
	DEBUG_VALID(m_pPopup);

	m_pPopup->ShowWindow(SW_HIDE);
	m_pPopup = NULL;

	__super::OnCloseCombo();
}

#pragma endregion: //:REGION

#pragma region ComboButton Class

#define PROP_HAS_LIST 0x0001

Property::ComboButton::ComboButton(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
}



void Property::ComboButton::AdjustButtonRect()
{
	__super::AdjustButtonRect();

	if (m_dwFlags & PROP_HAS_LIST) {
		m_rectButton.left -= m_rectButton.Width();
	}
}



//LPCTSTR Property::ComboButton::GetXMLTagName() const
//{
//	return _T("comboButton");
//}



BOOL Property::ComboButton::HasButton() const
{
	return TRUE;
}



void Property::ComboButton::OnClickButton(CPoint point)
{
	bool isLeft = point.x < m_rectButton.CenterPoint().x;

	if (isLeft) {
		__super::OnClickButton(point);
	}
	else {
		DEBUG_STOP;
		//:TODO - Display your dialog here...
		SetValue(_T("New value"));
	}
}



void Property::ComboButton::OnDrawButton(CDC* pDC, CRect rectButton)
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

#undef PROP_HAS_LIST

#pragma endregion //:REGION

#pragma region CommandButton Class

Property::CommandButton::CommandButton(const CString& name, const CString& title, UINT id, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, id, (LPCTSTR)title, lpDescr)
	, m_title(title)
{
}



BOOL Property::CommandButton::OnClickValue(UINT uiMsg, CPoint point)
{
	m_pWndList->OnPropertyChanged(this);

	return TRUE;
}



void Property::CommandButton::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT_VALID(m_pWndList);

#define TEXT_MARGIN(parent)	globalUtils.ScaleByDPI(4, parent)

	COLORREF oldColor;
	if (m_bHighlighted && m_bEnabled) {
		oldColor = pDC->SetTextColor((COLORREF)Control::EColor::DeepSkyBlue);
	}
	else {
		oldColor = pDC->SetTextColor((COLORREF)Control::EColor::NavyBlue);
	}

	rect.DeflateRect(TEXT_MARGIN(m_pWndList), 0);
	UINT uiDTFlags = DT_LEFT | DT_NOPREFIX | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER;
	pDC->DrawText(m_title, rect, uiDTFlags);

	pDC->SetTextColor(oldColor);

#undef TEXT_MARGIN
}



BOOL Property::CommandButton::OnEdit(LPPOINT lptClick)
{
	m_pWndList->OnPropertyChanged(this);

	return FALSE;
}



void Property::CommandButton::OnLeaveMouse()
{
	m_bHighlighted = false;

	Redraw();
}



BOOL Property::CommandButton::OnSetCursor() const
{
	return FALSE;
}

#pragma endregion //:REGION

#pragma region CustomColor Class

Property::CustomColor::CustomColor(const CString& name, const COLORREF& color, LPCTSTR lpDescr)
	: CBCGPColorProp(name, color, NULL, lpDescr)
{
	DEBUG_STOP;
	//:WARNING - localization
	EnableOtherButton(_T("Other..."));
	EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
}



//LPCTSTR Property::CustomColor::GetXMLTagName() const
//{
//	return _T("customColor");
//}



BOOL Property::CustomColor::OnDrawPaletteColorBox(CDC* pDC, const CRect rectColor, COLORREF color, BOOL /*bIsAutomatic*/, BOOL /*bIsSelected*/, BOOL /*bOnPopupMenu*/)
{
	CBCGPDrawManager dm(*pDC);
	dm.DrawEllipse(rectColor, color, globalData.clrBarDkShadow);

	return TRUE;
}

#pragma endregion //:REGION

#pragma region CustomDescription Class

Property::CustomDescription::CustomDescription(const CString& name, const _variant_t& value, LPCTSTR lpDescr)
	: CBCGPProp(name, value, lpDescr)
{
}



//LPCTSTR Property::CustomDescription::GetXMLTagName() const
//{
//	return _T("customDescription");
//}



void Property::CustomDescription::OnDrawDescription(CDC* pDC, CRect rect)
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

#pragma endregion //:REGION

#pragma region CustomDialog Class

Property::CustomDialog::CustomDialog(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
}



//LPCTSTR Property::CustomDialog::GetXMLTagName() const
//{
//	return _T("customDialog");
//}



void Property::CustomDialog::OnClickButton(CPoint point)
{
	DEBUG_STOP;
	//:TODO - Display your dialog here...
	SetValue(_T("New value"));
}



BOOL Property::CustomDialog::HasButton() const
{
	return TRUE;
}

#pragma endregion //:REGION

#pragma region CustomState Class

Property::CustomState::CustomState(const CString & name, LPCTSTR lpValue, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, lpValue, lpDescr, data)
{
	m_bAllowEdit = FALSE;
	m_strButtonText = _T("II");

	m_images.SetImageSize(CSize(14, 14));
	DEBUG_STOP;
	//m_images.Load(IDB_PROP_LIST_ANIMATION);

	SetState(NULL, L' ', RGB(0, 0, 0), FALSE, 16);
}



void Property::CustomState::AnimationStep()
{
	if (m_bStopped) {
		return;
	}

	m_nAnimationStep++;

	if (m_nAnimationStep >= m_images.GetCount()) {
		m_nAnimationStep = 0;
	}

	RedrawState();
}



//LPCTSTR Property::CustomState::GetXMLTagName() const
//{
//	return _T("customState");
//}



BOOL Property::CustomState::HasButton() const
{
	return TRUE;
}



void Property::CustomState::OnClickButton(CPoint /*point*/)
{
	m_bStopped = !m_bStopped;
	m_strButtonText = (m_bStopped ? _T(">") : _T("II"));

	Redraw();
}



void Property::CustomState::OnDrawStateIndicator(CDC* pDC, CRect rect)
{
	if (m_bStopped == false) {
		m_images.DrawEx(pDC, rect, m_nAnimationStep, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
	}
}

#pragma endregion //:REGION

#pragma region Duration Class

Property::Duration::Duration(const CString& name, const COleDateTimeSpan& duration, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, _variant_t((double)duration), lpDescr, data)
{
}



Property::Duration::~Duration()
{
	CBCGPPopupWindow::CloseActivePopup();
}



void Property::Duration::SetDuration(COleDateTimeSpan duration)
{
	ASSERT_VALID(this);
	SetValue(_variant_t((double)duration));
}



COleDateTimeSpan Property::Duration::GetDuration() const
{
	return (COleDateTimeSpan)(double)m_varValue;
}



void Property::Duration::AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)
{
	rectSpin.SetRectEmpty();

	rectEdit = m_Rect;
	rectEdit.left = m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth();
}



CWnd* Property::Duration::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DEBUG_VALID(m_pWndList);

	DurationCtrl* pDuration = new DurationCtrl;
	DEBUG_VALID(pDuration);

	pDuration->EnableVisualManagerStyle();
	pDuration->SetAutoResize(FALSE);
	pDuration->SetBackgroundColor(m_pWndList->GetBkColor(), FALSE);
	pDuration->SetTextColor(m_bEnabled ? m_pWndList->GetTextColor() : globalData.clrGrayedText, FALSE);

	CRect rectSpin;
	AdjustInPlaceEditRect(rectEdit, rectSpin);
	rectEdit.DeflateRect(1, 1);

	pDuration->Create(_T(""), WS_CHILD | WS_VISIBLE, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE);
	pDuration->SetFont(GetFont());
	pDuration->SetState(CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS, CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
	pDuration->SetDuration(GetDuration());
	pDuration->EnableWindow(m_bEnabled);

	bDefaultFormat = FALSE;

	return pDuration;
}



BOOL Property::Duration::DoPaste()
{
	if (__super::DoPaste() == FALSE) {
		return FALSE;
	}

	if (m_wndDuration.GetSafeHwnd() != nullptr) {
		m_wndDuration.SetDuration(GetDuration());
		Redraw();
	}

	return TRUE;
}



//LPCTSTR Property::Duration::GetXMLTagName() const
//{
//	return _T("duration");
//}



void Property::Duration::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(m_pWndList);

	rect.DeflateRect(1, 2);
	rect.left = m_pWndList->GetListRect().left + m_pWndList->GetPropertyColumnWidth() + TEXT_MARGIN + 1;

	DRAWITEMSTRUCT dis;
	::ZeroMemory(&dis, sizeof(DRAWITEMSTRUCT));

	dis.CtlType = ODT_BUTTON;
	dis.hDC = pDC->GetSafeHdc();
	dis.rcItem = rect;

	m_wndDuration.SetTextColor(m_bEnabled ? m_pWndList->GetTextColor() : globalData.clrGrayedText, FALSE);

	COLORREF clrFill = GetAlternateColor();
	if (clrFill == CLR_NONE) {
		clrFill = m_pWndList->GetBkColor();
	}

	m_wndDuration.SetBackgroundColor(clrFill, FALSE);
	m_wndDuration.SetFont(GetFont());
	m_wndDuration.DrawItem(&dis);
}



void Property::Duration::OnKillSelection(CBCGPProp* pNewSel)
{
	Redraw();
}



void Property::Duration::OnPosSizeChanged(CRect /*rectOld*/)
{
	ASSERT_VALID(m_pWndList);

	CRect rectEdit;
	CRect rectSpin;

	AdjustInPlaceEditRect(rectEdit, rectSpin);

	if (m_wndDuration.GetSafeHwnd() == nullptr) {
		m_wndDuration.SetAutoResize(FALSE);
		m_wndDuration.Create(_T(""), WS_CHILD, rectEdit, m_pWndList, (UINT)-1);

		m_wndDuration.SetState(
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS,
			CBCGPDurationCtrl::DRTN_DAYS | CBCGPDurationCtrl::DRTN_HOURS_MINS);
		m_wndDuration.SetFont(GetFont());
		m_wndDuration.SetDuration(GetDuration());
	}
	else {
		m_wndDuration.SetFont(GetFont());
		m_wndDuration.SetWindowPos(NULL, rectEdit.left, rectEdit.top, rectEdit.Width(), rectEdit.Height(), SWP_NOZORDER | SWP_NOACTIVATE);
	}

	m_wndDuration.AdjustControl(rectEdit);
}



void Property::Duration::OnSetSelection(CBCGPProp* pOldSel)
{
	Redraw();
}



BOOL Property::Duration::OnUpdateValue()
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

		pPopup->EnableCloseButton(FALSE);
		pPopup->SetCloseOnOwnerChange();
		pPopup->SetRoundedCorners();
		pPopup->SetShadow();
		pPopup->SetSmallCaptionGripper(FALSE);
		pPopup->SetStemLocation(CBCGPPopupWindow::BCGPPopupWindowStemLocation_TopLeft);

		DEBUG_STOP;
		//:TODO - localization
		CBCGPPopupWndParams params;
		params.m_strText = _T("The duration should not exceed 2 days. Please correct the entered duration value.");

		int nOffset = globalUtils.ScaleByDPI(5, m_pWndList);
		pPopup->Create(m_pWndList, params, NULL, CPoint(rect.left + nOffset, rect.bottom + nOffset));

		m_pWndInPlace->SetFocus();

		return FALSE;
	}

	m_varValue = _variant_t((double)durationNew);
	m_wndDuration.SetDuration(durationNew);

	if (durationOld != durationNew) {
		m_pWndList->OnPropertyChanged(this);
	}

	return TRUE;
}



BOOL Property::Duration::PushChar(UINT nChar)
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



void Property::Duration::SetValue(const _variant_t& varValue)
{
	ASSERT_VALID(this);

	__super::SetValue(varValue);

	if (m_wndDuration.GetSafeHwnd() != nullptr) {
		m_wndDuration.SetDuration(GetDuration());
	}
}

#pragma endregion //:REGION

#pragma region HexValue Class

Property::HexValue::HexValue(const CString& name, long lValue, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, lValue, lpDescr, data, NULL, NULL, _T("01234567890ABCDEFabcdef"))
{
}



CString Property::HexValue::FormatProperty()
{
	ASSERT_VALID(this);

	CString str;
	str.Format(_T("%X"), (long)m_varValue);

	return str;
}



//LPCTSTR Property::HexValue::GetXMLTagName() const
//{
//	return _T("hex");
//}



BOOL Property::HexValue::ParseValue(const CString& str)
{
	return __super::TextToVar(str);
}



BOOL Property::HexValue::SerializeValue(CString& str)
{
	str = __super::FormatProperty();

	return str != L"*** error ***";
}



BOOL Property::HexValue::TextToVar(const CString& str)
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

#pragma endregion //:REGION

#pragma region IconComboBox Class

Property::IconComboBox::IconComboBox(const CString& name, const CString& value, LPCTSTR lpDescr, DWORD_PTR data, CBCGPToolBarImages* pImageList)
	: CBCGPProp(name, (LPCTSTR)value, lpDescr, data)
{
	if (pImageList != nullptr) {
		pImageList->ExportToImageList(m_imageList);
	}
}



bool Property::IconComboBox::AddOption(LPCTSTR lpOption, int nIcon, int nIndent)
{
	if (!__super::AddOption(lpOption)) {
		RETURN_FALSE;
	}

	m_icons.push_back(nIcon);
	m_indents.push_back(nIndent);

	return true;
}



CComboBox* Property::IconComboBox::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int HEIGHT = 400;

	rect.bottom = rect.top + globalUtils.ScaleByDPI(HEIGHT, pWndParent);

	CComboBoxEx* pCombo = new CComboBoxEx;
	if (pCombo->Create(WS_CHILD | WS_VSCROLL | CBS_DROPDOWNLIST, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pCombo);
		RETURN_NULL;
	}

	if (m_imageList.GetSafeHandle() != nullptr) {
		pCombo->SetImageList(&m_imageList);
	}

	return pCombo;
}



//LPCTSTR Property::IconComboBox::GetXMLTagName() const
//{
//	return _T("comboBoxEx");
//}



BOOL Property::IconComboBox::OnEdit(LPPOINT lpClick)
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
		item.iSelectedImage = item.iImage = m_icons[index];
		item.iIndent = m_indents[index];
		item.pszText = (LPTSTR)(LPCTSTR)strItem;
		item.cchTextMax = strItem.GetLength();

		pCombo->InsertItem(&item);
	}

	return TRUE;
}

#pragma endregion //:REGION

#pragma region IconList Class

Property::IconList::IconList(const CString& name, CBCGPToolBarImages& icons, int nSelected, CStringList* pNames, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (long)nSelected, lpDescr, data)
	, m_icons(icons)
{
	if (pNames != nullptr) {
		m_iconNames.AddTail(pNames);
		ASSERT(m_iconNames.GetCount() == m_icons.GetCount());
	}

	for (int i = 0; i < m_icons.GetCount(); i++) {
		CString strItem;
		strItem.Format(_T("%d"), i);

		AddOption(strItem);
	}

	AllowEdit(FALSE);
}



CComboBox* Property::IconList::CreateCombo(CWnd* pWndParent, CRect rect)
{
	const int HEIGHT = 400;

	rect.bottom = rect.top + HEIGHT;

	IconComboBoxCtrl* pCombo = new IconComboBoxCtrl(m_icons, m_iconNames);
	DWORD dwStyle = WS_CHILD | WS_VSCROLL | CBS_NOINTEGRALHEIGHT | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;

	if (pCombo->Create(dwStyle, rect, pWndParent, BCGPROPLIST_ID_INPLACE_COMBO) == FALSE) {
		REMOVE_POINTER(pCombo);
		RETURN_NULL;
	}

	return pCombo;
}



CWnd* Property::IconList::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	CWnd* pWnd = __super::CreateInPlaceEdit(rectEdit, bDefaultFormat);
	if (pWnd != nullptr) {
		pWnd->ShowWindow(SW_HIDE);
	}

	return pWnd;
}



//LPCTSTR Property::IconList::GetXMLTagName() const
//{
//	return _T("iconList");
//}



void Property::IconList::OnDrawValue(CDC* pDC, CRect rect)
{
	ASSERT_VALID(m_pWndList);
	ASSERT_VALID(pDC);

	CString value = (LPCTSTR)(_bstr_t)m_varValue;
	if (value.IsEmpty() || m_icons.GetCount() == 0) {
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
	rectImage.right = rectImage.left + m_icons.GetImageSize().cx + ICON_PADDING(m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList);

	m_icons.DrawEx(pDC, rectImage, index, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);

	if (m_iconNames.IsEmpty() == false) {
		CString name = m_iconNames.GetAt(m_iconNames.FindIndex(index));
		rect.left = rectImage.right + 2 * ICON_PADDING(m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList);

		pDC->DrawText(name, rect, DT_SINGLELINE | DT_VCENTER);
	}

	m_bValueIsTrancated = FALSE;
}

#pragma endregion //:REGION

#pragma region Password Class

Property::Password::Password(const CString& name, const CString& password, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, (LPCTSTR)password, lpDescr, data)
{
}



CWnd* Property::Password::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	DWORD dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_PASSWORD;
	if (!m_bEnabled || !m_bAllowEdit) {
		dwStyle |= ES_READONLY;
	}

	CEdit* pEdit = new CEdit;
	if (pEdit->Create(dwStyle, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE) == FALSE) {
		REMOVE_POINTER(pEdit);
		RETURN_NULL;
	}

	pEdit->SetPasswordChar(DefaultChar);
	bDefaultFormat = TRUE;

	return pEdit;
}



CString Property::Password::FormatProperty()
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



//LPCTSTR Property::Password::GetXMLTagName() const
//{
//	return _T("password");
//}



CString Property::Password::GetXMLValue() const
{
	return CString();
}



BOOL Property::Password::IsCopyAvailable() const
{
	return FALSE;
}

#pragma endregion //:REGION

#pragma region RangeValidation Class

Property::RangeValidation::RangeValidation(const CString& name, long value, LPCTSTR lpDescr, DWORD_PTR data)
	: CBCGPProp(name, value, lpDescr, data)
{
}



void Property::RangeValidation::SetRange(long minValue, long maxValue)
{
	m_minValue = minValue;
	m_maxValue = maxValue;

	if (m_minValue > m_maxValue) {
		std::swap(m_minValue, m_maxValue);
	}
}



//LPCTSTR Property::RangeValidation::GetXMLTagName() const
//{
//	return _T("rangeValidation");
//}



BOOL Property::RangeValidation::OnEndEdit()
{
	CleanState();

	return __super::OnEndEdit();
}



BOOL Property::RangeValidation::OnUpdateValue()
{
	DEBUG_STOP;

	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndInPlace);
	ASSERT_VALID(m_pWndList);
	ASSERT(::IsWindow(m_pWndInPlace->GetSafeHwnd()));

	CString strText;
	m_pWndInPlace->GetWindowText(strText);

	long value = ::_ttol(strText);
	if (value < m_minValue || value > m_maxValue) {
		//:TODO - localization
		SetState(L"Incorrect value: the correct value should be between ??? and ???");
		return FALSE;
	}

	CleanState();

	return __super::OnUpdateValue();
}

#pragma endregion //:REGION

#pragma region Slider Class

Property::Slider::Slider(const CString& name, UINT id, long value, LPCTSTR lpDescr, DWORD_PTR data) :
	CBCGPProp(name, id, value, lpDescr, data)
{
	m_bIsVisible = true;
}



void Property::Slider::SetRange(int minValue, int maxValue, int step)
{
	ASSERT_VALID(this);

	m_minValue = minValue;
	m_maxValue = maxValue;
	m_step = step;
}



CWnd* Property::Slider::CreateInPlaceEdit(CRect rectEdit, BOOL& bDefaultFormat)
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pWndList);

	CBCGPClientDC dc(m_pWndList);

	CString strLabel(L"0000");
	rectEdit.left += dc.GetTextExtent(strLabel).cx;

	SliderCtrl* pSlider = new SliderCtrl(this, m_pWndList->GetBkColor());
	if (pSlider->Create(WS_VISIBLE | WS_CHILD | TBS_NOTICKS, rectEdit, m_pWndList, BCGPROPLIST_ID_INPLACE) == FALSE) {
		REMOVE_POINTER(pSlider);
		RETURN_NULL;
	};

	pSlider->SetRange(m_minValue, m_maxValue, TRUE);
	pSlider->SetTicFreq(m_step);
	pSlider->SetLineSize(m_step); // move to cursor
	pSlider->SetPageSize(m_step); // move to PgUp/PgDn

	pSlider->SetPos((long)m_varValue);
	pSlider->EnableProgressMode();
	pSlider->EnableWindow(m_bEnabled);

	bDefaultFormat = TRUE;

	return pSlider;
}



BOOL Property::Slider::OnSetCursor() const
{
	// Use default
	return FALSE;
}



BOOL Property::Slider::OnClickValue(UINT uiMsg, CPoint point)
{
	return TRUE;
}



//LPCTSTR Property::Slider::GetXMLTagName() const
//{
//	return _T("slider");
//}



BOOL Property::Slider::OnUpdateValue()
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

#pragma endregion //:REGION

#pragma region TwoButton Class

Property::TwoButtons::TwoButtons(const CString& name, const _variant_t& value) :
	CBCGPProp(name, value)
{
	LoadImages();
	m_nClickedButton = -1;
}



void Property::TwoButtons::AdjustButtonRect()
{
	__super::AdjustButtonRect();

	m_rectButton.left -= m_rectButton.Width();
}



//LPCTSTR Property::TwoButtons::GetXMLTagName() const
//{
//	return _T("twoButtons");
//}



BOOL Property::TwoButtons::HasButton() const
{
	return TRUE;
}



void Property::TwoButtons::OnClickButton(CPoint point)
{
	DEBUG_STOP;

	bool isLeft = point.x < m_rectButton.CenterPoint().x;

	m_nClickedButton = (isLeft ? 0 : 1);
	RedrawButton();

	//:TODO

	m_nClickedButton = -1;
	RedrawButton();
}



void Property::TwoButtons::OnDPIChanged(UINT nDPIOld, UINT nDPINew)
{
	__super::OnDPIChanged(nDPIOld, nDPINew);

	LoadImages();
}



void Property::TwoButtons::OnDrawButton(CDC* pDC, CRect rectButton)
{
	CSize imageSize = m_images.GetImageSize();

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
			(m_nClickedButton == i ? CBCGPVisualManager::ButtonsIsHighlighted : CBCGPVisualManager::ButtonsIsRegular);

		CBCGPVisualManager::GetInstance()->OnFillButtonInterior(pDC, &button, rect, state);
		m_images.DrawEx(pDC, rect, i, CBCGPToolBarImages::ImageAlignHorzCenter, CBCGPToolBarImages::ImageAlignVertCenter);
		CBCGPVisualManager::GetInstance()->OnDrawButtonBorder(pDC, &button, rect, state);
	}
}



void Property::TwoButtons::LoadImages()
{
	DEBUG_STOP;

	m_images.Clear();
	m_images.SetImageSize(CSize(14, 14));
	m_images.SetTransparentColor(RGB(255, 0, 255));
	//m_images.Load(IDB_BUTTONS);

	globalUtils.ScaleByDPI(m_images, (m_pWndList->GetSafeHwnd() == nullptr ? AfxGetMainWnd() : m_pWndList));
}

#pragma endregion //:REGION
