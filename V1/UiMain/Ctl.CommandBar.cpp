#include "stdafx.h"

#include "Ast.h"
#include "Cls.UserIoManager.h"
#include "Ctl.CommandBar.h"
#include "Wnd.Application.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View2d.h"

#include <Signal2d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery	TheApp.GetMainFrame().ActiveView->GetDelivery2d()->UserIO
#define theParams	UioManager.Params



namespace
{
	COLORREF ForeColor = (COLORREF)Ctl::EColor::White;
	COLORREF BackColor = (COLORREF)Ctl::EColor::Arsenic;
	COLORREF BorderColor = (COLORREF)Ctl::EColor::Gray;
	COLORREF PompptColor = (COLORREF)Ctl::EColor::DarkGray;

	enum class Id
	{
		This = WM_USER,
		Command,
		Prompt,
		Keyword,
		Input,
	};
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlCommandBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CLICK, (int)Id::Keyword, OnKeyword)
END_MESSAGE_MAP()



bool CtlCommandBar::Initialize(CWnd* pParentWnd)
{
	const DWORD dwStyle = WS_CHILD | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, (UINT)Id::This) == FALSE) {
		RETURN_FALSE;
	}

	CFont* pFont = &globalData.fontDefaultGUI;

#define CreateControl(x,id) \
x.m_bVisualManagerStyle = TRUE; \
x.m_clrText = ForeColor; \
x.m_clrBkgnd = BackColor; \
x.Create(L"", WS_CHILD, {}, this, (UINT)id); \
x.SetFont(pFont);

	CreateControl(CommandCtl, Id::Command);
	CreateControl(PromptCtl, Id::Prompt);
	CreateControl(KeywordCtl, Id::Keyword);

#undef CreateControl

	KeywordCtl.m_bUnderlineLinks = FALSE;
	KeywordCtl.m_clrLink = (COLORREF)Ctl::EColor::LightGray;

	CBCGPEditColors colors;
	colors.m_clrText = ForeColor;
	colors.m_clrBackground = BackColor;
	colors.m_clrPrompt = PompptColor;

	InputCtl.m_bVisualManagerStyle = TRUE;
	InputCtl.Create(dwStyle, {}, this, (UINT)Id::Input);
	InputCtl.SetColorTheme(colors);
	InputCtl.SetFont(pFont);

	return true;
}



void CtlCommandBar::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return;
	}

	const int thisWidth = globalUtils.ScaleByDPI(800);
	const int thisHeight = globalUtils.ScaleByDPI(28);
	const int xMargin = globalUtils.ScaleByDPI(8);
	const int inputHeight = globalUtils.ScaleByDPI(20);
	const int inputTop = globalUtils.ScaleByDPI(6);

	CRect parentArea;
	GetParent()->GetClientRect(parentArea);

	CRect thisArea;
	thisArea.left = 0;
	thisArea.top = parentArea.bottom - thisHeight;
	thisArea.right = thisWidth;
	thisArea.bottom = parentArea.bottom;
	// move and resize this
	SetWindowPos(NULL, thisArea.left, thisArea.top, thisArea.Width(), thisArea.Height(), SWP_NONE);

	CRect frame;
	GetClientRect(frame);
	int margin = xMargin;

	if (CommandCtl.IsWindowVisible()) {
		frame.left += margin;
		CommandCtl.SizeToContent();
		CRect rect = Ctl::AdjustLayout(&CommandCtl, frame, {}, Ctl::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	if (PromptCtl.IsWindowVisible()) {
		frame.left += margin;
		PromptCtl.SizeToContent();
		CRect rect = Ctl::AdjustLayout(&PromptCtl, frame, {}, Ctl::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	if (KeywordCtl.IsWindowVisible()) {
		frame.left += margin;
		KeywordCtl.SizeToContent();
		CRect rect = Ctl::AdjustLayout(&KeywordCtl, frame, {}, Ctl::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	// CEdit margin
	margin /= 2;

	frame.left += margin;
	frame.right -= margin;
	frame.top = inputTop;
	frame.bottom = frame.top + inputHeight;

	InputCtl.SetWindowPos(NULL, frame.left, frame.top, frame.Width(), frame.Height(), SWP_NONE);
}

//--------------------------------------------------------------------------------------------------

bool CtlCommandBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (InputCtl.GetSafeHwnd() != nullptr && InputCtl.IsWindowVisible()) {
		PostMessage(WM_CHAR, (WPARAM)nChar, (LPARAM)nRepCnt);
		return true;
	}

	return true;
}



bool CtlCommandBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (InputCtl.GetSafeHwnd() != nullptr && InputCtl.IsWindowVisible()) {
		PostMessage(WM_KEYDOWN, (WPARAM)nChar, (LPARAM)nRepCnt);
		return true;
	}

	return true;
}



BOOL CtlCommandBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR) {
		if (pMsg->hwnd != InputCtl.GetSafeHwnd()) {
			// WARNING - not SendMessage
			InputCtl.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}

		// repeat - pMsg->lParam
		UINT chr = pMsg->wParam;
		CString input;
		InputCtl.GetWindowText(input);

		if (chr == VK_ESCAPE) {
			InputCtl.SetWindowText(nullptr);
			if (theParams.Command.IsEmpty() == false) {
				theDelivery.OnInput(KEY_CANCEL);
			}
		}
		else if (chr == VK_SPACE) {
			if (input.IsEmpty() == true) {
				// TODO - run last command, change input
			}

			if (theParams.FindKeyword(input)) {
				input.Insert(0, PRE_KEYWORD);
			}

			InputCtl.SetWindowText(nullptr);
			theDelivery.OnInput(input);
		}
		else if (chr == VK_RETURN) {
			if (theParams.FindKeyword(input)) {
				input.Insert(0, PRE_KEYWORD);
			}

			if (input.IsEmpty() == false) {
				InputCtl.SetWindowText(nullptr);
				theDelivery.OnInput(input);
			}
		}
		else {
			return __super::PreTranslateMessage(pMsg);
		}

		//:CHECK - Why does the cursor disappear?
		SetCursor(TheApp.LoadStandardCursor(IDC_ARROW));
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}



BOOL CtlCommandBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound(BackColor);
	const CBrush border(BorderColor);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&border);
	rect.DeflateRect(1, 1, 1, 1);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void CtlCommandBar::OnKeyword(NMHDR* pNmhdr, LRESULT* pResult)
{
	*pResult = 0;

	BCGPNMLINK* pLink = (BCGPNMLINK*)pNmhdr;
	if (pLink != nullptr) {
		theDelivery.OnInput(pLink->item.szUrl);
	}
	else {
		DEBUG_STOP;
	}
}

//--------------------------------------------------------------------------------------------------

bool CtlCommandBar::SetCommand()
{
	CString value = theParams.Command;

	if (value.IsEmpty() == false) {
		CommandCtl.SetWindowText(value);
		CommandCtl.ShowWindow(SW_SHOW);

		InputCtl.SetPrompt(nullptr);
		InputCtl.SetErrorMessage(nullptr);
		InputCtl.SetWindowText(nullptr);
		InputCtl.ShowWindow(SW_SHOW);
	}
	else {
		CommandCtl.ShowWindow(SW_HIDE);
	}

	AdjustLayout();

	return true;
}



bool CtlCommandBar::SetPrompt()
{
	static const CString prefix = CString(PRE_KEYWORD);

	if (theParams.Prompt.IsEmpty()) {
		PromptCtl.ShowWindow(SW_HIDE);
	}
	else {
		PromptCtl.SetWindowText(theParams.Prompt);
		PromptCtl.ShowWindow(SW_SHOW);
	}

	if (theParams.Keyword.IsEmpty()) {
		KeywordCtl.ShowWindow(SW_HIDE);
	}
	else {
		WStringArray keywords;
		WStr::Split(theParams.Keyword, L'/', keywords);

		CString multiLink = L"[ ";
		for (auto keyword : keywords) {
			CString code = WStr::Front(keyword, L'(', false, false);
			CString key = WStr::Back(keyword, L'(', L')', false, false);

			multiLink += L"<A HREF=\"" + prefix + key + L"\">" + keyword + L"</A> ";
		}
		multiLink += L"]";

		KeywordCtl.SetWindowText(multiLink);
		KeywordCtl.ShowWindow(SW_SHOW);
	}

	InputCtl.SetPrompt(nullptr);
	InputCtl.SetErrorMessage(nullptr);
	InputCtl.SetWindowText(nullptr);

	AdjustLayout();

	PromptCtl.RedrawWindow();
	KeywordCtl.RedrawWindow();

	return true;
}



bool CtlCommandBar::SetEcho(const CString& value)
{
	InputCtl.SetPrompt(value);

	return true;
}



bool CtlCommandBar::SetError(const CString& value)
{
	InputCtl.SetErrorMessage(value, (COLORREF)Ctl::EColor::IndianRed);

	return true;
}



bool CtlCommandBar::StandbyCommand(const CString& prompt)
{
	CommandCtl.ShowWindow(SW_HIDE);
	PromptCtl.ShowWindow(SW_HIDE);
	KeywordCtl.ShowWindow(SW_HIDE);

	AdjustLayout();

	InputCtl.SetWindowText(L"");
	InputCtl.SetPrompt(prompt);
	InputCtl.ShowWindow(SW_SHOW);

	return true;
}

#undef theParams
#undef theDelivery
