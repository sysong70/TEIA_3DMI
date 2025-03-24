#include "stdafx.h"

#include "Dlg.Base.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region DlgBase Class

BEGIN_MESSAGE_MAP(DlgBase, CBCGPDialog)
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()



void DlgBase::DoModaless()
{
	// CHECK
	EnableParent(false);

	AdjustControlsLayout();
	Create(TemplateId);
	ShowWindow(SW_SHOW);
	// WARNING - do not remove (pump message)
	RedrawWindow();
}



CRect DlgBase::GetBodyRect()
{
	CRect rect = GetClientArea();

	rect.top += HeaderHeight;
	rect.bottom -= FooterHeight;

	return rect;
}



CSize DlgBase::GetBodySize()
{
	CRect rect = GetClientArea();

	return { rect.Width(), rect.Height() - HeaderHeight - FooterHeight };
}



bool DlgBase::PumpMessages()
{
	// Must call Create() before using the dialog
	HWND hWnd = GetSafeHwnd();
	ASSERT(hWnd != NULL);

	while (TRUE) {
		if (!::IsWindow(hWnd)) {
			return false;
		}

		MSG msg;
		// Handle dialog messages
		while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				PostThreadMessage(GetCurrentThreadId(), msg.message, msg.wParam, msg.lParam);
				return false;
			}
			if (!::IsWindow(hWnd)) {
				return false;
			}
			if (!IsDialogMessage(&msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!::IsWindow(hWnd)) {
				return false;
			}
		}
	}

	return true;
}



BOOL DlgBase::OnInitDialog()
{
	__super::OnInitDialog();

	EnableVisualManagerStyle(TRUE, TRUE);
	// Allow to drag the dialog box by clicking inside the client area
	EnableDragClientArea();

	return TRUE;
}



void DlgBase::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	if (SizeLimit.ptMinTrackSize.x > 0 && SizeLimit.ptMinTrackSize.y > 0) {
		lpMMI->ptMinTrackSize = SizeLimit.ptMinTrackSize;
	}
	if (SizeLimit.ptMaxTrackSize.x > 0 && SizeLimit.ptMaxTrackSize.y > 0) {
		lpMMI->ptMaxTrackSize = SizeLimit.ptMaxTrackSize;
	}
}



CSize DlgBase::AdjustWindowSize(CSize client)
{
	CRect padding = GetFramePadding();
	CSize win = client;
	win.cx += padding.left + padding.right;
	win.cy += padding.top + padding.bottom;

	SetWindowPos(NULL, 0, 0, win.cx, win.cy, SWP_NOMOVE);

	return win;
}



void DlgBase::EnableParent(bool enable)
{
	if (m_pParentWnd != nullptr && m_pParentWnd->GetSafeHwnd() != nullptr) {
		m_pParentWnd->EnableWindow(enable);
	}
}



CRect DlgBase::GetClientArea()
{
	CRect rect;
	GetClientRect(&rect);

	return rect;
}



CRect DlgBase::GetFramePadding()
{
	CRect windowRect;
	GetWindowRect(windowRect);
	CRect clientArea;
	GetClientRect(clientArea);
	ClientToScreen(&clientArea);

	CRect padding = {
		abs(windowRect.left - clientArea.left),
		abs(windowRect.top - clientArea.top),
		abs(windowRect.right - clientArea.right),
		abs(windowRect.bottom - clientArea.bottom)
	};

	if (padding.IsRectNull()) {
		CSize dp = Dlg::FramePadding();
		padding = { dp.cx, dp.cy, dp.cx ,dp.cy };
	}

	return padding;
}



CSize DlgBase::GetScaled32()
{
	CRect rect;
	GetDlgItem(IDC_DMI_SIZE_CHECKER)->GetClientRect(rect);

	return globalUtils.ScaleByDPI(rect).Size();
}



void DlgBase::SetSizeLimit(bool bMinLimit, bool bMaxLimit)
{
	if (bMinLimit) {
		SizeLimit.ptMinTrackSize.x = WindowSize.cx;
		SizeLimit.ptMinTrackSize.y = WindowSize.cy;
	}

	if (bMaxLimit) {
		SizeLimit.ptMaxTrackSize.x = WindowSize.cx;
		SizeLimit.ptMaxTrackSize.y = WindowSize.cy;
	}
}



void DlgBase::SetSizeLimit(POINT min, POINT max)
{
	SizeLimit.ptMinTrackSize = min;
	SizeLimit.ptMaxTrackSize = max;
}

#pragma endregion //:REGION

//**************************************************************************************************

#pragma region DlgInstances Class

DlgInstances::DlgInstances()
{
}



DlgInstances::~DlgInstances()
{
	for (auto dlg : Buffer) {
		REMOVE_POINTER(dlg);
	}

	Buffer.clear();
}



void DlgInstances::Add(DlgBase* pValue)
{
	DEBUG_VALID(pValue);

	Buffer.push_back(pValue);
}



DlgBase* DlgInstances::Get(int id)
{
	for (auto dlg : Buffer) {
		if (dlg->GetId() == id) {
			return dlg;
		}
	}

	return nullptr;
}



void DlgInstances::Remove(int id)
{
	DlgBase* pValue = nullptr;
	for (auto dlg : Buffer) {
		if (dlg->GetId() == id) {
			pValue = dlg;
			break;
		}
	}

	if (pValue != nullptr) {
		Buffer.remove(pValue);

		pValue->DestroyWindow();
		REMOVE_POINTER(pValue);
	}
}

#pragma endregion //:REGION
