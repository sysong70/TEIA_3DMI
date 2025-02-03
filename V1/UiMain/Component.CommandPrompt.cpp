#include "stdafx.h"
#include "Component.CommandPrompt.h"
#include "Window.MainFrame.h"
#include "Window.View2d.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define Delivery (*(Window::View*)GetParent()).GetDelivery()

//**************************************************************************************************

namespace
{
	COLORREF ForeColor = (COLORREF)Control::EColor::White;
	COLORREF BackColor = (COLORREF)Control::EColor::Arsenic;
	COLORREF BorderColor = (COLORREF)Control::EColor::Gray;
	COLORREF PompptColor = (COLORREF)Control::EColor::DarkGray;



	CString DefaultPrompt()
	{
		return Facility::Local(L"Type a command|명령 입력");
	};
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(Component::CommandPrompt, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()



Component::CommandPrompt::CommandPrompt()
{
}



Component::CommandPrompt::~CommandPrompt()
{
}



bool Component::CommandPrompt::Initialize(CWnd* pParentWnd)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	CFont* pFont = &globalData.fontDefaultGUI;

#define CreateControl(x) \
x.m_bVisualManagerStyle = TRUE; \
x.m_clrText = ForeColor; \
x.m_clrBkgnd = BackColor; \
x.Create(L"", WS_CHILD, {}, this); \
x.SetFont(pFont);

	CreateControl(m_wndCommand);
	CreateControl(m_wndPrompt);
	CreateControl(m_wndKeyword);

#undef CreateControl

	CBCGPEditColors colors;
	colors.m_clrText = ForeColor;
	colors.m_clrBackground = BackColor;
	colors.m_clrPrompt = PompptColor;

	m_wndInput.m_bVisualManagerStyle = TRUE;
	m_wndInput.Create(WS_CHILD, {}, this, WM_USER);
	m_wndInput.SetColorTheme(colors);
	m_wndInput.SetFont(pFont);
	m_wndInput.SetPrompt(DefaultPrompt());

	return true;
}



CSize Component::CommandPrompt::AdjustLayout()
{
	const int WindowWidth = 800;
	const int WindowHeight = 28;
	const int WindowXPadding = 12;
	const int ControlXMargin = 8;
	const int InputHeight = 20;
	const int InputTop = 6;

	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	CRect parentArea;
	GetParent()->GetClientRect(parentArea);
/*
	int padding = globalUtils.ScaleByDPI(WindowXPadding);
	int height = globalUtils.ScaleByDPI(WindowHeight);
	int width = globalUtils.ScaleByDPI(WindowWidth) + padding * 2;
	if (width > parentArea.Width()) {
		width = parentArea.Width() - padding * 2;
	}

	CRect thisArea;
	thisArea.left = (parentArea.Width() - width) / 2;
	thisArea.top = parentArea.bottom - height;
	thisArea.right = thisArea.left + width;
	thisArea.bottom = parentArea.bottom;
	// move and resize this
	SetWindowPos(NULL, thisArea.left, thisArea.top, thisArea.Width(), thisArea.Height(), SWP_NONE);
*/
	int padding = globalUtils.ScaleByDPI(WindowXPadding);
	int height = globalUtils.ScaleByDPI(WindowHeight);
	int width = globalUtils.ScaleByDPI(WindowWidth);

	CRect thisArea;
	thisArea.left = 0;
	thisArea.top = parentArea.bottom - height;
	thisArea.right = width;
	thisArea.bottom = parentArea.bottom;
	// move and resize this
	SetWindowPos(NULL, thisArea.left, thisArea.top, thisArea.Width(), thisArea.Height(), SWP_NONE);


	CRect frame;
	GetClientRect(frame);
	int margin = globalUtils.ScaleByDPI(ControlXMargin);

	if (m_wndCommand.IsWindowVisible()) {
		frame.left += margin;
		m_wndCommand.SizeToContent();
		CRect rect = Control::AdjustLayout(&m_wndCommand, frame, {}, Control::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	if (m_wndPrompt.IsWindowVisible()) {
		frame.left += margin;
		m_wndPrompt.SizeToContent();
		CRect rect = Control::AdjustLayout(&m_wndPrompt, frame, {}, Control::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	if (m_wndKeyword.IsWindowVisible()) {
		frame.left += margin;
		m_wndKeyword.SizeToContent();
		CRect rect = Control::AdjustLayout(&m_wndKeyword, frame, {}, Control::EPivot::MiddleLeft);
		frame.left = rect.right;
	}

	// CEdit margin
	margin /= 2;

	frame.left += margin;
	frame.right -= margin;
	frame.top = globalUtils.ScaleByDPI(InputTop);
	frame.bottom = frame.top + globalUtils.ScaleByDPI(InputHeight);

	m_wndInput.SetWindowPos(NULL, frame.left, frame.top, frame.Width(), frame.Height(), SWP_NONE);

	return { thisArea.Width(), thisArea.Height() };
}

//--------------------------------------------------------------------------------------------------

void Component::CommandPrompt::Activate(CString command, CString prompt, CString keyword)
{
	SetCommand(command);
	SetPrompt(prompt);
	SetKeyword(keyword);
	ClearInput();

	AdjustLayout();
	ShowWindow(SW_SHOW);
}



void Component::CommandPrompt::Activate(CString prompt, CString keyword)
{
	ASSERT(m_command.IsEmpty() == false);

	SetCommand(m_command);
	SetPrompt(prompt);
	SetKeyword(keyword);
	ClearInput();

	AdjustLayout();
	ShowWindow(SW_SHOW);
}



void Component::CommandPrompt::Cancel()
{
}



bool Component::CommandPrompt::Parse()
{
	return false;
}



void Component::CommandPrompt::Wait()
{
	m_wndCommand.ShowWindow(SW_HIDE);
	m_wndPrompt.ShowWindow(SW_HIDE);
	m_wndKeyword.ShowWindow(SW_HIDE);

	ClearInput(true);

	AdjustLayout();
	ShowWindow(SW_SHOW);
}

//--------------------------------------------------------------------------------------------------

BOOL Component::CommandPrompt::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound(BackColor);
	const CBrush border(BorderColor);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&border);
	rect.DeflateRect(CRect(1, 1, 1, 1));
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Component::CommandPrompt::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

//--------------------------------------------------------------------------------------------------

void Component::CommandPrompt::ClearInput(bool wait)
{
	m_wndInput.SetWindowText(L"");
	m_wndInput.SetPrompt(wait ? DefaultPrompt() : L"");
	m_wndInput.ShowWindow(SW_SHOW);
}



bool Component::CommandPrompt::SetCommand(const CString& value)
{
	m_command = value;

	if (value.IsEmpty() == false) {
		m_wndCommand.SetWindowText(value);
		m_wndCommand.ShowWindow(SW_SHOW);
		return true;
	}
	else {
		m_wndCommand.ShowWindow(SW_HIDE);
		return false;
	}
}



bool Component::CommandPrompt::SetPrompt(const CString& value)
{
	m_prompt = value;

	if (value.IsEmpty() == false) {
		m_wndPrompt.SetWindowText(value);
		m_wndPrompt.ShowWindow(SW_SHOW);
		return true;
	}
	else {
		m_wndPrompt.ShowWindow(SW_HIDE);
		return false;
	}
}



bool Component::CommandPrompt::SetKeyword(const CString& value)
{
	m_keyword = value;

	WStringArray keywords;
	WStr::Split(value, L' ', keywords);

	for (auto keyword : keywords) {
		CString title = Facility::Local(keyword);
		// TODO - add button or ...
	}

	if (keywords.size() > 0) {
		m_wndKeyword.ShowWindow(SW_SHOW);
		return true;
	}
	else {
		m_wndKeyword.ShowWindow(SW_HIDE);
		return false;
	}
}



bool Component::CommandPrompt::SetMacro(const CString& value)
{
	RETURN_FALSE;
}

#undef Delivery
