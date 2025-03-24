#include "stdafx.h"

#include "Ctl.TaskIoBar.h"
#include "Ast.AppResources.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View2d.h"

#include "../Library/Signal/Signal2d.h"
#include "../Library/OdaInterface/Uio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery	((WndView2d*)GetParent())->GetDelivery2d()->UserIO



namespace
{
	CRect BarMargin()
	{
		return globalUtils.ScaleByDPI(CRect(4, 38, 0, 38));
	}

	CSize CloseHandleSize()
	{
		return globalUtils.ScaleByDPI(CSize(16, 16));
	}
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlTaskIoBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_COMMAND_CLICKED, OnCommandClicked)
END_MESSAGE_MAP()



bool CtlTaskIoBar::Initialize(CWnd* pParentWnd)
{
	const DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	TitleCtl.Create(L"TITLE", WS_CHILD | WS_VISIBLE, {}, this);
	TitleCtl.m_bVisualManagerStyle = TRUE;

	return PropListCtl.Initialize(this, WM_USER, {});
}



CSize CtlTaskIoBar::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	int padding = Ctl::BarPadding();
	CRect margin = BarMargin();

	CRect titleRect;
	CRect propRect;

	TitleCtl.GetWindowRect(titleRect);
	PropListCtl.GetWindowRect(propRect);

	CSize barSize = CSize(0, titleRect.Height()) + propRect.Size() + CSize(padding * 2, padding * 3);

	TitleCtl.SetWindowPos(NULL, padding, padding, 0, 0, SWP_NOSIZE);
	PropListCtl.SetWindowPos(NULL, padding, titleRect.Height() + padding * 2, propRect.Width(), propRect.Height(), SWP_SHOWWINDOW);
	SetWindowPos(NULL, margin.left, margin.top, barSize.cx, barSize.cy, SWP_SHOWWINDOW);

	return {};
}



void CtlTaskIoBar::Show(WndView* pTargetView)
{
	if (pTargetView == nullptr) {
		ShowWindow(SW_HIDE);
		return;
	}

	SetParent((CWnd*)pTargetView);
	ShowWindow(SW_SHOW);
	AdjustLayout();

	Initialized = true;
}



bool CtlTaskIoBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case VK_ESCAPE:
		theDelivery.OnInput(KEY_CANCEL);
		Show(nullptr);
		break;

	case '@':
		PropListCtl.Select(L"LimitLength");
		break;

	case '<':
		PropListCtl.Select(L"LimitAngle");
		break;

	case 'x':
	case 'X':
		PropListCtl.Select(L"FilterX");
		break;

	case 'y':
	case 'Y':
		PropListCtl.Select(L"FilterY");
		break;

	case '_':
		PropListCtl.Select(L"OSnapOverrides");
		break;

	default:
		PropListCtl.Select(L"Point", nChar);
		break;
	}

	return true;
}



bool CtlTaskIoBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return false;
}



BOOL CtlTaskIoBar::PreTranslateMessage(MSG* pMsg)
{
	BOOL process = __super::PreTranslateMessage(pMsg);
	return process;
}



LRESULT CtlTaskIoBar::OnCommandClicked(WPARAM wp, LPARAM lp)
{
	int index = (int)lp * 2 + 1;

	CString value(PRE_KEYWORD);
	value += UioManager.Params.Keywords[index];
	theDelivery.OnInput(value);

	return S_OK;
}



LRESULT CtlTaskIoBar::OnDPIChangedAfterParent(WPARAM, LPARAM)
{
	LRESULT result = Default();

	//:TODO

	return result;
}



BOOL CtlTaskIoBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush border((COLORREF)Ctl::EColor::Gray);
	const CBrush backgound((COLORREF)Ctl::EColor::DialogBack);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&border);
	rect.DeflateRect(CRect(1, 1, 1, 1));
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



LRESULT CtlTaskIoBar::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	if (Initialized == false) {
		return S_OK;
	}

	CBCGPProp* pProp = (CBCGPProp*)lp;
	CString name = pProp->GetXMLTagName();
	CString result;

	if (name == L"Point") {
		result = (CString)pProp->GetValue();
	}
	else if (name == L"LimitLength") {
		result = L"@" + (CString)pProp->GetValue();
	}
	else if (name == L"LimitAngle") {
		result = L"<" + (CString)pProp->GetValue();
	}
	else if (name == L"FilterX") {
		result = L".x" + (CString)pProp->GetValue();
	}
	else if (name == L"FilterY") {
		result = L".y" + (CString)pProp->GetValue();
	}
	else if (name == L"OSnapOverrides") {
		int index = pProp->GetSelectedOption();

		switch (index) {
		case 0: result = L"_none";		break;
		case 1: result = L"_point";		break;
		case 2: result = L"_end";		break;
		case 3: result = L"_mid";		break;
		case 4: result = L"_int";		break;
		case 5: result = L"_perp";		break;
		case 6: result = L"_center";	break;
		case 7: result = L"_quad";		break;
		case 8: result = L"_near";		break;
		default:
			break;
		}
	}
	else {
		DEBUG_STOP;
	}

	if (result.IsEmpty() == false) {
		theDelivery.OnInput(result);
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------------------

bool CtlTaskIoBar::SetPrompt()
{
	WndView* pView = TheApp.GetMainFrame().ActiveView;
	if (pView == nullptr) {
		return false;
	}

	int options = UioManager.Params.Options;

	if (GetBit(options, Uio::eFirstPoint) || GetBit(options, Uio::eOtherPoint)) {
		Json::Object& design = TheAppResources.GetTask("IoPoint");
		Json::Object& header = design.GetAt("header");
		Json::Object& properties = design.GetAt("properties");

		CString title = Ast::GetTitle(header);
		TitleCtl.SetWindowText(title);
		TitleCtl.SizeToContent();

		CSize size = globalUtils.ScaleByDPI(Ast::GetSize(header.GetAt("size")));
		PropListCtl.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE);
		PropListCtl.InitializeDesign(properties);

		PropListCtl.Enable(L"LimitLength", GetBit(options, Uio::eUseLengthFilter));
		PropListCtl.Enable(L"LimitAngle", GetBit(options, Uio::eUseAngleFilter));
	}
	else {
		DEBUG_STOP;
	}

	CString keyword = UioManager.Params.Keyword;

	if (keyword.IsEmpty() == false) {
		WStringArray keywords;
		WStr::Split(keyword, L'/', keywords);

		CStringList commands;
		for (auto key : keywords) {
			commands.AddTail(key);
		}

		PropListCtl.SetCommands(commands);
		PropListCtl.SetCommandsVisible(TRUE);
	}
	else {
		PropListCtl.SetCommandsVisible(FALSE);
	}

	Show(pView);

	return true;
}



bool CtlTaskIoBar::StandbyCommand(const CString& prompt)
{
	Show(nullptr);
	return true;
}
