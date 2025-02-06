#include "stdafx.h"
#include "Component.CommandBar.h"
#include "Window.MainFrame.h"
#include "Window.View2d.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define theDelivery (*m_pView).GetDelivery().userIO

//**************************************************************************************************

namespace
{
	COLORREF ForeColor = (COLORREF)Control::EColor::White;
	COLORREF BackColor = (COLORREF)Control::EColor::Arsenic;
	COLORREF BorderColor = (COLORREF)Control::EColor::Gray;
	COLORREF PompptColor = (COLORREF)Control::EColor::DarkGray;

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

void Component::CommandBar::Data::Clear()
{
	Command.Empty();
	Prompt.Empty();
	Keyword.Empty();
	Keywords.clear();
}



bool Component::CommandBar::Data::FindKeyword(const CString& value)
{
	for (auto item : Keywords) {
		if (item.CompareNoCase(value) == 0) {
			return true;
		}
	}

	return false;
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(Component::CommandBar, CWnd)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(NM_CLICK, (int)Id::Keyword, OnKeyword)
END_MESSAGE_MAP()



Component::CommandBar::CommandBar()
{
}



Component::CommandBar::~CommandBar()
{
}



bool Component::CommandBar::Initialize(CWnd* pParentWnd)
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


	CreateControl(m_wndCommand, Id::Command);
	CreateControl(m_wndPrompt, Id::Prompt);
	CreateControl(m_wndKeyword, Id::Keyword);

#undef CreateControl

	m_wndKeyword.m_bUnderlineLinks = FALSE;
	m_wndKeyword.m_clrLink = (COLORREF)Control::EColor::LightGray;

	CBCGPEditColors colors;
	colors.m_clrText = ForeColor;
	colors.m_clrBackground = BackColor;
	colors.m_clrPrompt = PompptColor;

	m_wndInput.m_bVisualManagerStyle = TRUE;
	m_wndInput.Create(dwStyle, {}, this, (UINT)Id::Input);
	m_wndInput.SetColorTheme(colors);
	m_wndInput.SetFont(pFont);

	return true;
}



void Component::CommandBar::ReceiveSignal(Json::Object* pData)
{
#define OnAction(x) Signal::UserIO::Action::Put##x: Put##x(data); break

	Json::Object& data = *pData;
	Signal::UserIO::Action action = (Signal::UserIO::Action)data.GetInteger(SKW_ACTION, -1);

	if (action == Signal::UserIO::Action::StandbyCommand) {
		StandbyCommand(data);
		AdjustLayout();
	}
	else {
		switch (action) {
		case OnAction(Command);
		case OnAction(Prompt);
		case OnAction(Error);
		case OnAction(Echo);

		default:
			DEBUG_STOP;
			break;
		}
	}

	REMOVE_POINTER(pData);

#undef OnAction
}



void Component::CommandBar::ViewChanged(Window::View* pView)
{
	m_pView = pView;
	ShowWindow(SW_SHOW);
}



CSize Component::CommandBar::AdjustLayout()
{
	const int THIS_WIDTH = globalUtils.ScaleByDPI(800);
	const int THIS_HEIGHT = globalUtils.ScaleByDPI(28);
	const int XMARGIN = globalUtils.ScaleByDPI(8);
	const int INPUT_HEIGHT = globalUtils.ScaleByDPI(20);
	const int INPUT_TOP = globalUtils.ScaleByDPI(6);

	if (GetSafeHwnd() == nullptr || IsWindowVisible() == false ||
		GetParent() == nullptr || GetParent()->GetSafeHwnd() == nullptr) {
		return {};
	}

	CRect parentArea;
	GetParent()->GetClientRect(parentArea);

	CRect thisArea;
	thisArea.left = 0;
	thisArea.top = parentArea.bottom - THIS_HEIGHT;
	thisArea.right = THIS_WIDTH;
	thisArea.bottom = parentArea.bottom;
	// move and resize this
	SetWindowPos(NULL, thisArea.left, thisArea.top, thisArea.Width(), thisArea.Height(), SWP_NONE);

	CRect frame;
	GetClientRect(frame);
	int margin = XMARGIN;

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
	frame.top = INPUT_TOP;
	frame.bottom = frame.top + INPUT_HEIGHT;

	m_wndInput.SetWindowPos(NULL, frame.left, frame.top, frame.Width(), frame.Height(), SWP_NONE);

	return { thisArea.Width(), thisArea.Height() };
}

//--------------------------------------------------------------------------------------------------

#include "Window.Application.h"

BOOL Component::CommandBar::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR) {
		if (pMsg->hwnd != m_wndInput.GetSafeHwnd()) {
			// WARNING - not SendMessage
			m_wndInput.PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
			return TRUE;
		}

		// repeat - pMsg->lParam
		UINT chr = pMsg->wParam;
		CString input;
		m_wndInput.GetWindowText(input);

		if (chr == VK_ESCAPE) {
			m_wndInput.SetWindowText(nullptr);
			if (m_data.Command.IsEmpty() == false) {
				theDelivery.OnInput(KEY_CANCEL);
			}
		}
		else if (chr == VK_SPACE) {
			if (input.IsEmpty() == true) {
				// TODO - run last command, change input
			}

			if (m_data.FindKeyword(input)) {
				input.Insert(0, PRE_KEYWORD);
			}

			m_wndInput.SetWindowText(nullptr);
			theDelivery.OnInput(input);
		}
		else if (chr == VK_RETURN) {
			if (m_data.FindKeyword(input)) {
				input.Insert(0, PRE_KEYWORD);
			}

			if (input.IsEmpty() == false) {
				m_wndInput.SetWindowText(nullptr);
				theDelivery.OnInput(input);
			}
		}
		else {
			return __super::PreTranslateMessage(pMsg);
		}

		//:CHECK - Why does the cursor disappear?
		SetCursor(TheApplication.LoadStandardCursor(IDC_ARROW));
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}



BOOL Component::CommandBar::OnEraseBkgnd(CDC* pDC)
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



void Component::CommandBar::OnKeyword(NMHDR* pNmhdr, LRESULT* pResult)
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

bool Component::CommandBar::PutCommand(Json::Object& data)
{
	m_data.Command = data.GetString(SKW_COMMAND);

	if (m_data.Command.IsEmpty() == false) {
		m_wndCommand.SetWindowText(m_data.Command);
		m_wndCommand.ShowWindow(SW_SHOW);

		m_wndInput.SetPrompt(nullptr);
		m_wndInput.SetErrorMessage(nullptr);
		m_wndInput.SetWindowText(nullptr);
		m_wndInput.ShowWindow(SW_SHOW);
	}
	else {
		m_wndCommand.ShowWindow(SW_HIDE);
	}

	AdjustLayout();

	return true;
}



bool Component::CommandBar::PutPrompt(Json::Object& data)
{
	static const CString PREFIX = CString(PRE_KEYWORD);

	ASSERT(m_data.Command.IsEmpty() == false);

	m_data.Prompt = Facility::Local(data.GetString(SKW_PROMPT));
	m_data.Keyword = Facility::Local(data.GetString(SKW_KEYWORD));
	m_data.Keywords.clear();

	if (m_data.Prompt.IsEmpty()) {
		m_wndPrompt.ShowWindow(SW_HIDE);
	}
	else {
		m_wndPrompt.SetWindowText(m_data.Prompt);
		m_wndPrompt.ShowWindow(SW_SHOW);
	}

	if (m_data.Keyword.IsEmpty()) {
		m_wndKeyword.ShowWindow(SW_HIDE);
	}
	else {
		WStringArray keywords;
		WStr::Split(m_data.Keyword, L'/', keywords);

		CString multiLink = L"[ ";
		for (auto item : keywords) {
			CString code = WStr::Front(item, L'(', false, false);
			CString key = WStr::Back(item, L'(', L')', false, false);
			m_data.Keywords.push_back(code);
			m_data.Keywords.push_back(key);

			multiLink += L"<A HREF=\"" + PREFIX + key + L"\">" + item + L"</A> ";
		}
		multiLink += L"]";

		m_wndKeyword.SetWindowText(multiLink);
		m_wndKeyword.ShowWindow(SW_SHOW);
	}

	m_wndInput.SetPrompt(nullptr);
	m_wndInput.SetErrorMessage(nullptr);
	m_wndInput.SetWindowText(nullptr);

	AdjustLayout();

	m_wndPrompt.RedrawWindow();
	m_wndKeyword.RedrawWindow();

	return true;
}



bool Component::CommandBar::PutEcho(Json::Object& data)
{
	CString value = Facility::Local(data.GetString(SKW_VALUE));
	m_wndInput.SetPrompt(value);
	return true;
}



bool Component::CommandBar::PutError(Json::Object& data)
{
	CString value = Facility::Local(data.GetString(SKW_VALUE));
	m_wndInput.SetErrorMessage(value, (COLORREF)Control::EColor::IndianRed);
	return true;
}



bool Component::CommandBar::StandbyCommand(Json::Object& data)
{
	m_data.Clear();

	m_wndCommand.ShowWindow(SW_HIDE);
	m_wndPrompt.ShowWindow(SW_HIDE);
	m_wndKeyword.ShowWindow(SW_HIDE);

	CString prompt = Facility::Local(data.GetString(SKW_PROMPT));

	m_wndInput.SetWindowText(L"");
	m_wndInput.SetPrompt(prompt);
	m_wndInput.ShowWindow(SW_SHOW);

	return true;
}

#undef theDelivery
