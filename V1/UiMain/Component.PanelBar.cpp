#include "stdafx.h"
#include "resource.h"
#include "Component.PanelBar.h"
#include "Component.TabWnd.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetPanelBar

namespace PresetPanelBar
{
	const UINT Id = WM_USER;

	CSize MinBarSize()
	{
		return globalUtils.ScaleByDPI(CSize(270, 600));
	}
}



using namespace Component;

BEGIN_MESSAGE_MAP(PanelBar, CBCGPDockingControlBar)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()



Component::PanelBar::PanelBar()
{
}



Component::PanelBar::~PanelBar()
{
}



bool Component::PanelBar::Initialize(CWnd* pMainFrame)
{
	if (Create(Facility::Local(L"PANELS|패널"), pMainFrame, CSize(600, 100), TRUE,
		PRESET::Id, WS_CHILD | WS_VISIBLE /* | WS_DISABLED */ | CBRS_LEFT) == FALSE) {
		RETURN_FALSE;
	}

	EnableDocking(CBRS_ALIGN_LEFT);
	SetBCGStyle(GetBCGStyle() & ~(CBRS_BCGP_AUTOHIDE | CBRS_BCGP_FLOAT));
	SetMinSize(PRESET::MinBarSize());
	//:WARNING - clear all and close button only
	RemoveCaptionButtons();
	m_arrButtons.Add(new CBCGPCaptionButton(HTCLOSE_BCG, FALSE, this));
	//:WARNING - set child style
	SendMessageToDescendants(BCGM_ONSETCONTROLVMMODE, TRUE, 0, TRUE, FALSE);

	EnableWindow(FALSE);

	return true;
}



void Component::PanelBar::ViewChanged(TabWnd* pTabs)
{
	DEBUG_VALID(pTabs);

	CRect rect;
	GetClientRect(rect);

	m_pActiveTabs = pTabs;
	m_pActiveTabs->ShowWindow(SW_HIDE);
	m_pActiveTabs->SetParent(this);
	m_pActiveTabs->SetWindowPos(nullptr, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE);
	m_pActiveTabs->ShowWindow(SW_SHOW);

	EnableWindow(TRUE);
}



BOOL Component::PanelBar::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)EColor::DarkBack);

	CRect rect;
	GetClientRect(rect);
	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



void Component::PanelBar::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (m_pActiveTabs != nullptr) {
		m_pActiveTabs->SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

#undef PRESET
