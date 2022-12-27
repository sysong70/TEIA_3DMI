#include "stdafx.h"
#include "Component.PropertyBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Component;

BEGIN_MESSAGE_MAP(PropertyBar, CBCGPDockingControlBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_COMMAND_CLICKED, OnCommandClicked)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_MENU_ITEM_SELECTED, OnMenuItemSelected)
END_MESSAGE_MAP()



Component::PropertyBar::PropertyBar()
{
}



Component::PropertyBar::~PropertyBar()
{
}



LRESULT Component::PropertyBar::OnCommandClicked(WPARAM, LPARAM lp)
{
	int nCommandIndex = (int)lp;

	// TODO: Set your command handler code here

	return 0;
}



int Component::PropertyBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDockingControlBar::OnCreate(lpCreateStruct) == -1) {

		return -1;
	}

	CRect dummyRect;
	dummyRect.SetRectEmpty();

	if (m_wndControl.Create(WS_VISIBLE | WS_CHILD, dummyRect, this, 1) == FALSE) {
		TRACE0("Failed to create Properies Grid \n");
		return -1;
	}

	Initialize();
	AdjustLayout();

	return 0;
}



LRESULT Component::PropertyBar::OnMenuItemSelected(WPARAM wp, LPARAM lp)
{
	int nMenuIndex = (int)wp;

	CBCGPProp* pProp = (CBCGPProp*)lp;
	ASSERT_VALID(pProp);

	// TODO: Set your menu item processing code here

	return 0;
}



void Component::PropertyBar::OnSize(UINT nType, int cx, int cy)
{
	CBCGPDockingControlBar::OnSize(nType, cx, cy);
	AdjustLayout();
}



void Component::PropertyBar::OnSetFocus(CWnd* pOldWnd)
{
	CBCGPDockingControlBar::OnSetFocus(pOldWnd);
	m_wndControl.SetFocus();
}



void Component::PropertyBar::Initialize()
{
	m_wndControl.EnableToolBar();
	m_wndControl.EnableSearchBox();
	m_wndControl.EnableHeaderCtrl(FALSE);
	m_wndControl.EnableDescriptionArea();
	m_wndControl.EnableContextMenu();

	m_wndControl.MarkModifiedProperties();
	m_wndControl.SetVSDotNetLook();
	m_wndControl.SetGroupNameFullWidth();

	m_wndControl.SetRowPadding(1);
}
