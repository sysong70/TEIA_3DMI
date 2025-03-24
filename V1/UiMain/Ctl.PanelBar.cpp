#include "stdafx.h"

#include "Ast.h"
#include "Cls.PanelManager.h"
#include "Ctl.PanelBar.h"
#include "Ctl.TabWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlPanelBar, CBCGPDockingControlBar)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



bool CtlPanelBar::Initialize(CWnd* pMainFrame)
{
	if (Create(Ast::Local(L"PANELS|패널"), pMainFrame, CSize(600, 100), TRUE, WM_USER,
		WS_CHILD | WS_VISIBLE | WS_DISABLED | CBRS_LEFT) == FALSE) {
		RETURN_FALSE;
	}

	const CSize barSize = globalUtils.ScaleByDPI(CSize(270, 600));

	EnableDocking(CBRS_ALIGN_LEFT);
	SetBCGStyle(GetBCGStyle() & ~(CBRS_BCGP_AUTOHIDE | CBRS_BCGP_FLOAT));
	SetMinSize(barSize);
	// WARNING - clear all and close button only
	RemoveCaptionButtons();
	m_arrButtons.Add(new CBCGPCaptionButton(HTCLOSE_BCG, FALSE, this));
	// WARNING - set child style
	//SendMessageToDescendants(BCGM_ONSETCONTROLVMMODE, TRUE, 0, TRUE, FALSE);

	return true;
}



void CtlPanelBar::ViewChanged(ClsPanelManager* pManager)
{
	DEBUG_VALID(pManager);

	CRect rect;
	GetClientRect(rect);

	ActivePanelManager = pManager->Activate(this, { rect.Width(), rect.Height() });

	EnableWindow(TRUE);
}



BOOL CtlPanelBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Ctl::EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void CtlPanelBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (ActivePanelManager != nullptr) {
		ActivePanelManager->OnSize(nType, cx, cy);
	}
}
