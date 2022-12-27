#include "stdafx.h"
#include "Dialog.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Dialog;

BEGIN_MESSAGE_MAP(Base, CBCGPDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()



Dialog::Base::Base(UINT nIDTemplate, CWnd* pParent)
	: CBCGPDialog(nIDTemplate, pParent)
	, m_nTemplateId(nIDTemplate)
{
}



Dialog::Base::~Base()
{
}



void Dialog::Base::DoModaless()
{
	EnableParent(false);

	AdjustControlsLayout();
	Create(m_nTemplateId);
	ShowWindow(SW_SHOW);
}

// return dialog result

Signal::Target Dialog::Base::GetSignalTargetId()
{
	return Signal::Target::Unknown;
}



Facility::Base* Dialog::Base::GetResultData(bool bNewInstance)
{
	RETURN_NULL;
}

// delete after using this data

Json::Object* Dialog::Base::GetResult()
{
	RETURN_NULL;
}



void Dialog::Base::ReceiveSignal(Json::Object* pData)
{
	DEBUG_STOP;
	REMOVE_POINTER(pData);
}



CRect Dialog::Base::ConstructHeader(const CRect& boundary)
{
	return {};
}

CRect Dialog::Base::ConstructBody(const CRect& boundary)
{
	return {};
}

CRect Dialog::Base::ConstructFooter(const CRect& boundary)
{
	return {};
}



CRect Dialog::Base::GetBodyRect()
{
	CRect rect = GetClientArea();

	rect.top += m_nHeaderHeight;
	rect.bottom -= m_nFooterHeight;

	return rect;
}



CSize Dialog::Base::GetBodySize()
{
	CRect rect = GetClientArea();

	return { rect.Width(), rect.Height() - m_nHeaderHeight - m_nFooterHeight };
}



BOOL Dialog::Base::OnInitDialog()
{
	__super::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);
	// Allow to drag the dialog box by clicking inside the client area
	EnableDragClientArea();

	return TRUE;
}



void Dialog::Base::PostNcDestroy()
{
	__super::PostNcDestroy();
}



void Dialog::Base::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (m_sizeLimit.ptMinTrackSize.x > 0 && m_sizeLimit.ptMinTrackSize.y > 0) {
		lpMMI->ptMinTrackSize = m_sizeLimit.ptMinTrackSize;
	}
	if (m_sizeLimit.ptMaxTrackSize.x > 0 && m_sizeLimit.ptMaxTrackSize.y > 0) {
		lpMMI->ptMaxTrackSize = m_sizeLimit.ptMaxTrackSize;
	}
}



CSize Dialog::Base::AdjustWindowSize(CSize client)
{
	CRect padding = GetPadding();
	CSize win = client;
	win.cx += padding.left + padding.right;
	win.cy += padding.top + padding.bottom;

	SetWindowPos(nullptr, 0, 0, win.cx, win.cy, SWP_NOMOVE);

	return win;
}



void Dialog::Base::EnableParent(bool enable)
{
	if (m_pParentWnd != nullptr && m_pParentWnd->GetSafeHwnd() != nullptr) {
		m_pParentWnd->EnableWindow(enable);
	}
}



inline CRect Dialog::Base::GetClientArea()
{
	CRect rect;
	GetClientRect(&rect);

	return rect;
}



CFont* Dialog::Base::GetDefaultFont()
{
	//:WAIT
	return GetFont();
}



CRect Dialog::Base::GetPadding()
{
	CRect windowRect;
	GetWindowRect(windowRect);
	CRect clientArea;
	GetClientRect(clientArea);
	ClientToScreen(&clientArea);

	return {
		abs(windowRect.left - clientArea.left),
		abs(windowRect.top - clientArea.top),
		abs(windowRect.right - clientArea.right),
		abs(windowRect.bottom - clientArea.bottom)
	};
}



CSize Dialog::Base::GetFrameThickness()
{
	return GetPadding().BottomRight();
}



CSize Dialog::Base::GetScaled32()
{
	CRect rect;
	GetDlgItem(IDC_DMI_SIZE_CHECKER)->GetClientRect(rect);

	return globalUtils.ScaleByDPI(rect).Size();
}



void Dialog::Base::SetSizeLimit(bool bMinLimit, bool bMaxLimit)
{
	if (bMinLimit) {
		m_sizeLimit.ptMinTrackSize.x = m_windowSize.cx;
		m_sizeLimit.ptMinTrackSize.y = m_windowSize.cy;
	}

	if (bMaxLimit) {
		m_sizeLimit.ptMaxTrackSize.x = m_windowSize.cx;
		m_sizeLimit.ptMaxTrackSize.y = m_windowSize.cy;
	}
}



void Dialog::Base::SetSizeLimit(POINT min, POINT max)
{
	m_sizeLimit.ptMinTrackSize = min;
	m_sizeLimit.ptMaxTrackSize = max;
}
