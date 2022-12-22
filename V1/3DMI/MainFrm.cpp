// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "DmiApp.h"

#include "MainFrm.h"
#include "BackStagePageInfo.h"

#include "DmiDoc.h"

#include "../Common/Json.h"

#include "../Common/DmiCommandDefine.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CBCGPMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_WINDOW_MANAGER, OnWindowManager)
	ON_COMMAND(ID_MDI_MOVE_TO_NEXT_GROUP, OnMdiMoveToNextGroup)
	ON_COMMAND(ID_MDI_MOVE_TO_PREV_GROUP, OnMdiMoveToPrevGroup)
	ON_COMMAND(ID_MDI_NEW_HORZ_TAB_GROUP, OnMdiNewHorzTabGroup)
	ON_COMMAND(ID_MDI_NEW_VERT_GROUP, OnMdiNewVertGroup)
	ON_COMMAND(ID_MDI_CANCEL, OnMdiCancel)
	ON_COMMAND(ID_VIEW_WORKSPACE, OnViewWorkspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateViewWorkspace)
	ON_COMMAND(ID_VIEW_WORKSPACE2, OnViewWorkspace2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE2, OnUpdateViewWorkspace2)
	ON_COMMAND(ID_VIEW_OUTPUT, OnViewOutput)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateViewOutput)
	ON_COMMAND(ID_VIEW_PROPERTIES, OnViewPropGrid)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIES, OnUpdateViewPropGrid)
	ON_COMMAND(ID_TOOLS_OPTIONS, OnToolsOptions)
	ON_COMMAND(ID_TOOLS_MANAGER, OnToolsManager)
	ON_COMMAND_RANGE(ID_RIBBON_AUTOHIDE, ID_RIBBON_SHOW_TABS_COMMANDS, OnRibbonDisplayOptions)
	ON_UPDATE_COMMAND_UI_RANGE(ID_RIBBON_AUTOHIDE, ID_RIBBON_SHOW_TABS_COMMANDS, OnUpdateRibbonDisplayOptions)

	ON_REGISTERED_MESSAGE(BCGM_ON_BEFORE_RIBBON_BACKSTAGE_VIEW, OnBeforeShowRibbonBackstageView)
	ON_REGISTERED_MESSAGE(BCGM_ON_RIBBON_CUSTOMIZE, OnRibbonCustomize)

	ON_MESSAGE(MSG_FILE_IMPORT_INFORMATION, OnMsgFileImportInformation)
	ON_MESSAGE(MSG_FILE_IMPORT_EXCHANGE_INFORMATION, OnMsgFileImportExchangeInformation)
	ON_MESSAGE(MSG_SELECTED_ITEM_INFORMATION, OnMsgSelectedItemInformation)

	ON_WM_ACTIVATE()
	ON_WM_ACTIVATEAPP()
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CBCGPMDITabParams mdiTabParams;

	mdiTabParams.m_bTabIcons = TRUE;
	mdiTabParams.m_tabLocation = CBCGPTabWnd::LOCATION_TOP;
	mdiTabParams.m_bDocumentMenu = TRUE;
	mdiTabParams.m_style = CBCGPTabWnd::STYLE_3D_SCROLLED;
	mdiTabParams.m_closeButtonMode = CBCGPTabWnd::TAB_CLOSE_BUTTON_HIGHLIGHTED_COMPACT;

	EnableMDITabbedGroups(TRUE, mdiTabParams);

	EnableTearOffMDIChildren();

	if (!CreateRibbonBar())
	{
		TRACE0("Failed to create ribbon bar\n");
		return -1;      // fail to create
	}


	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.LoadFromXML(_T("IDR_BCGP_RIBBON_XML")))
	{
		TRACE0("Failed to load status bar\n");
		return -1;      // fail to create
	}

	// Load control bar icons:
	CBCGPToolBarImages imagesWorkspace;
	imagesWorkspace.SetImageSize(CSize(16, 16));
	imagesWorkspace.Load(IDB_WORKSPACE);
	globalUtils.ScaleByDPI(imagesWorkspace);

	const int nPaneSize = globalUtils.ScaleByDPI(200);

	if (!m_wndModelBrowser.Create(_T("Model Browser"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE, ID_VIEW_WORKSPACE,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar\n");
		return -1;      // fail to create
	}

	m_wndModelBrowser.SetIcon(imagesWorkspace.ExtractIcon(0), FALSE);

	if (!m_wndWorkSpace2.Create(_T("View 2"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE, ID_VIEW_WORKSPACE2,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_LEFT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Workspace bar 2\n");
		return -1;      // fail to create
	}

	m_wndWorkSpace2.SetIcon(imagesWorkspace.ExtractIcon(1), FALSE);

	const int nOutputPaneSize = globalUtils.ScaleByDPI(150);

	if (!m_wndOutput.Create(_T("Output"), this, CSize(nOutputPaneSize, nOutputPaneSize),
		TRUE /* Has gripper */, ID_VIEW_OUTPUT,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM))
	{
		TRACE0("Failed to create output bar\n");
		return -1;      // fail to create
	}
	m_wndOutput.SetIcon(imagesWorkspace.ExtractIcon(2), FALSE);

	if (!m_wndPropGrid.Create(_T("Properties"), this, CRect(0, 0, nPaneSize, nPaneSize),
		TRUE,
		ID_VIEW_PROPERTIES,
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBRS_RIGHT | CBRS_FLOAT_MULTI))
	{
		TRACE0("Failed to create Properties Bar\n");
		return FALSE;		// fail to create
	}

	m_wndPropGrid.SetIcon(imagesWorkspace.ExtractIcon(3), FALSE);

	m_wndModelBrowser.EnableDocking(CBRS_ALIGN_ANY);
	m_wndWorkSpace2.EnableDocking(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDocking(CBRS_ALIGN_ANY);
	m_wndPropGrid.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	EnableAutoHideBars(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndModelBrowser);
	m_wndWorkSpace2.AttachToTabWnd(&m_wndModelBrowser, BCGP_DM_STANDARD, FALSE, NULL);
	DockControlBar(&m_wndOutput);
	DockControlBar(&m_wndPropGrid);

	// Enable windows manager:
	EnableWindowsDialog(ID_WINDOW_MANAGER, IDS_WINDOWS_MANAGER, TRUE, FALSE, TRUE);

	// Enable windows navigator (activated by Ctrl+Tab/Ctrl+Shift+Tab):
	EnableWindowsNavigator();

	DragAcceptFiles(TRUE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGPMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CBCGPMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CBCGPMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

// CMainFrame message handlers


void CMainFrame::OnWindowManager()
{
	ShowWindowsDialog();
}

BOOL CMainFrame::OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop)
{
	CMenu menu;
	VERIFY(menu.LoadMenu(bDrop ? IDR_POPUP_DROP_MDITABS : IDR_POPUP_MDITABS));

	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);

	if ((dwAllowedItems & BCGP_MDI_CREATE_HORZ_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_HORZ_TAB_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CREATE_VERT_GROUP) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_NEW_VERT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_NEXT) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_NEXT_GROUP, MF_BYCOMMAND);
	}

	if ((dwAllowedItems & BCGP_MDI_CAN_MOVE_PREV) == 0)
	{
		pPopup->DeleteMenu(ID_MDI_MOVE_TO_PREV_GROUP, MF_BYCOMMAND);
	}

	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;
	pPopupMenu->SetAutoDestroy(FALSE);

	CWnd* pMenuOwner = GetActiveTearOffFrame() == NULL ? this : GetActiveTearOffFrame();
	pPopupMenu->Create(pMenuOwner, point.x, point.y, pPopup->GetSafeHmenu ());

	return TRUE;
}

void CMainFrame::OnMdiMoveToNextGroup()
{
	MDITabMoveToNextGroup();
}

void CMainFrame::OnMdiMoveToPrevGroup()
{
	MDITabMoveToNextGroup(FALSE);
}

void CMainFrame::OnMdiNewHorzTabGroup()
{
	MDITabNewGroup(FALSE);
}

void CMainFrame::OnMdiNewVertGroup()
{
	MDITabNewGroup();
}

void CMainFrame::OnMdiCancel()
{
	// TODO: add your command handler code here

}

CBCGPMDIChildWnd* CMainFrame::CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != NULL && lpcszDocName [0] != '\0')
	{
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(lpcszDocName);
		if (pDoc != NULL)
		{
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != NULL)
			{
				CView* pView = pDoc->GetNextView(pos);
				if (pView == NULL)
				{
					return NULL;
				}

				return DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pView->GetParent());
			}
		}
	}

	return NULL;
}

void CMainFrame::OnClose()
{
	SaveMDIState(theApp.GetRegSectionPath());
	CBCGPMDIFrameWnd::OnClose();
}


BOOL CMainFrame::CreateRibbonBar()
{
	m_wndRibbonBar.EnableCustomization();

	if (!m_wndRibbonBar.Create(this))
	{
		return FALSE;
	}

	if (!m_wndRibbonBar.LoadFromXML(_T("IDR_BCGP_RIBBON_XML")))
	{
		return FALSE;
	}

	CBCGPRibbonCategory* pBackstageCategory = m_wndRibbonBar.GetBackstageCategory();

	if (pBackstageCategory != NULL)
	{
		CBCGPRibbonBackstageViewPanel* pBackstagePanel = DYNAMIC_DOWNCAST(CBCGPRibbonBackstageViewPanel, pBackstageCategory->GetPanel(0));

		if (pBackstagePanel != NULL)
		{
			pBackstagePanel->AttachViewToItem(5, new CBCGPRibbonBackstageViewItemForm(IDD_FORM_INFO, RUNTIME_CLASS(CBackStagePageInfo)), FALSE);
			pBackstagePanel->AttachRecentViewToItem(6, BCGP_SHOW_FOLDERS_LIST | BCGP_SHOW_PINS, FALSE);
			pBackstagePanel->AttachPrintPreviewToItem(7, FALSE);
		}
	}

	// Add "Ribbon Display Options" button to the left of system buttons:
	m_RibbonDisplayOptionsIcons.SetImageSize(CSize(32, 32));
	m_RibbonDisplayOptionsIcons.Load(IDB_RIBBON_DISPLAY_OPTIONS);
	globalUtils.ScaleByDPI(m_RibbonDisplayOptionsIcons);

	HICON icon = (HICON) ::LoadImage(
		AfxGetResourceHandle(),
		MAKEINTRESOURCE(IDI_RIBBON_DISPLAY_OPTIONS),
		IMAGE_ICON,
		globalUtils.ScaleByDPI(16),
		globalUtils.ScaleByDPI(16),
		LR_SHARED);

	CBCGPRibbonCaptionCustomButton* pCaptionButton = new CBCGPRibbonCaptionCustomButton((UINT)-1, icon, BCGP_RIBBON_CAPTION_CUSTOM_BUTTON_DISPLAY_IN_NORMAL_MODE);
	pCaptionButton->SetDefaultCommand(FALSE);
	pCaptionButton->SetToolTipText(_T("Ribbon Display Options"));

	pCaptionButton->AddSubItem(new CBCGPRibbonButton(ID_RIBBON_AUTOHIDE, _T("Auto-hide Ribbon"), m_RibbonDisplayOptionsIcons.ExtractIcon(0), TRUE, NULL, TRUE, TRUE));
	pCaptionButton->AddSubItem(new CBCGPRibbonButton(ID_RIBBON_SHOW_TABS, _T("Show Tabs"), m_RibbonDisplayOptionsIcons.ExtractIcon(1), TRUE, NULL, TRUE, TRUE));
	pCaptionButton->AddSubItem(new CBCGPRibbonButton(ID_RIBBON_SHOW_TABS_COMMANDS, _T("Show Tabs and Commands"), m_RibbonDisplayOptionsIcons.ExtractIcon(2), TRUE, NULL, TRUE, TRUE));

	m_wndRibbonBar.AddCaptionCustomButton(pCaptionButton);

	return TRUE;
}

LRESULT CMainFrame::OnRibbonCustomize(WPARAM wp, LPARAM /*lp*/)
{
	ShowOptions(wp == 0 ? 1 : 0);
	return 1;
}

void CMainFrame::OnToolsOptions()
{
	ShowOptions(0);
}

void CMainFrame::OnToolsManager()
{
	ShowOptions(2);
}

void CMainFrame::ShowOptions(int nPage)
{
	// Create custom categories:

	// "Popular" items:

	CList<UINT, UINT> lstPopular;

	lstPopular.AddTail(ID_FILE_NEW);
	lstPopular.AddTail(ID_FILE_OPEN);
	lstPopular.AddTail(ID_FILE_SAVE);
	lstPopular.AddTail(ID_FILE_PRINT_PREVIEW);
	lstPopular.AddTail(ID_FILE_PRINT_DIRECT);
	lstPopular.AddTail(ID_EDIT_UNDO);

	// Hidden commands:
	CList<UINT,UINT> lstHidden;
	m_wndRibbonBar.GetItemIDsList(lstHidden, TRUE);

	// All commands:
	CList<UINT,UINT> lstAll;
	m_wndRibbonBar.GetItemIDsList(lstAll, FALSE, FALSE);

	// Create "Customize QAT" page:
	CBCGPRibbonCustomizeQATPage pageCustomizeQAT(&m_wndRibbonBar);

	pageCustomizeQAT.AddCustomCategory(_T("Popular Commands"), lstPopular);
	pageCustomizeQAT.AddCustomCategory(_T("Commands not in the Ribbon"), lstHidden);
	pageCustomizeQAT.AddCustomCategory(_T("All Commands"), lstAll);

	// Create "Customize Ribbon" page:
	CBCGPRibbonCustomizeRibbonPage pageCustomizeRibbon(&m_wndRibbonBar);

	pageCustomizeRibbon.AddCustomCategory(_T("Popular Commands"), lstPopular);
	pageCustomizeRibbon.AddCustomCategory(_T("Commands not in the Ribbon"), lstHidden);
	pageCustomizeRibbon.AddCustomCategory(_T("All Commands"), lstAll);

	// Create "Customize Context Menu" page:
	CBCGPRibbonCustomizeContextMenuPage pageCustomizeContextMenu(&m_wndRibbonBar);
	pageCustomizeContextMenu.AddCustomCategory(_T("Popular Commands"), lstPopular);
	pageCustomizeContextMenu.AddCustomCategory(_T("Commands not in the Ribbon"), lstHidden);
	pageCustomizeContextMenu.AddCustomCategory(_T("All Commands"), lstAll);

	// Create property sheet:
	CBCGPPropertySheet propSheet(_T("Options"), this, nPage);
	propSheet.m_psh.dwFlags |= PSH_NOAPPLYNOW;
	propSheet.EnableLayout(TRUE, NULL, TRUE);
	propSheet.EnableVisualManagerStyle(TRUE, TRUE);

	propSheet.SetLook(CBCGPPropertySheet::PropSheetLook_List, -1 /* Default list width */);

	propSheet.AddPage(&pageCustomizeRibbon);

	propSheet.AddPage(&pageCustomizeQAT);

	propSheet.AddPage(&pageCustomizeContextMenu);


	// Create "Tools" page:
	CBCGPRibbonToolsPage pageTools(&m_wndRibbonBar);
	propSheet.AddPage(&pageTools);

	// TODO: add your option pages here:
	// COptionsPage1 pageOptions1;
	// propSheet.AddPage(&pageOptions1);
	//
	// COptionsPage1 pageOptions2;
	// propSheet.AddPage(&pageOptions2);

	if (propSheet.DoModal() != IDOK)
	{
		return;
	}
}

void CMainFrame::OnViewWorkspace()
{
	ShowControlBar(&m_wndModelBrowser, !(m_wndModelBrowser.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewWorkspace(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndModelBrowser.IsVisible());
	pCmdUI->Enable(!GetDockManager()->IsPrintPreviewValid());
}

void CMainFrame::OnViewWorkspace2()
{
	ShowControlBar(&m_wndWorkSpace2, !(m_wndWorkSpace2.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewWorkspace2(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndWorkSpace2.IsVisible());
	pCmdUI->Enable(!GetDockManager()->IsPrintPreviewValid());
}

void CMainFrame::OnViewOutput()
{
	ShowControlBar(&m_wndOutput, !(m_wndOutput.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewOutput(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndOutput.IsVisible());
	pCmdUI->Enable(!GetDockManager()->IsPrintPreviewValid());
}

void CMainFrame::OnViewPropGrid()
{
	ShowControlBar(&m_wndPropGrid, !(m_wndPropGrid.IsVisible()), FALSE, TRUE);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewPropGrid(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropGrid.IsVisible());
	pCmdUI->Enable(!GetDockManager()->IsPrintPreviewValid());
}



void CMainFrame::OnRibbonDisplayOptions(UINT nCmd)
{
	switch (nCmd)
	{
	case ID_RIBBON_AUTOHIDE:
		m_wndRibbonBar.SetAutoHideMode();
		break;

	case ID_RIBBON_SHOW_TABS:
		m_wndRibbonBar.SetAutoHideMode(FALSE);

		if (!m_wndRibbonBar.IsMinimized())
		{
			m_wndRibbonBar.ToggleMimimizeState();
		}
		break;

	case ID_RIBBON_SHOW_TABS_COMMANDS:
		m_wndRibbonBar.SetAutoHideMode(FALSE);

		if (m_wndRibbonBar.IsMinimized())
		{
			m_wndRibbonBar.ToggleMimimizeState();
		}
		break;
	}
}

void CMainFrame::OnUpdateRibbonDisplayOptions(CCmdUI* pCmdUI)
{
	switch (pCmdUI->m_nID)
	{
	case ID_RIBBON_AUTOHIDE:
		pCmdUI->Enable(m_wndRibbonBar.IsAutoHideModeAvailable());
		pCmdUI->SetRadio(m_wndRibbonBar.IsInAutoHideMode());
		break;

	case ID_RIBBON_SHOW_TABS:
		pCmdUI->SetRadio(!m_wndRibbonBar.IsInAutoHideMode() && m_wndRibbonBar.IsMinimized());
		break;

	case ID_RIBBON_SHOW_TABS_COMMANDS:
		pCmdUI->SetRadio(!m_wndRibbonBar.IsInAutoHideMode() && !m_wndRibbonBar.IsMinimized());
		break;
	}
}

LRESULT CMainFrame::OnBeforeShowRibbonBackstageView(WPARAM, LPARAM)
{
	theApp.CreateScreenshot(m_bmpAppPreview);
	return 0;
}



void CMainFrame::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CBCGPMDIFrameWnd::OnActivate(nState, pWndOther, bMinimized);

	//AfxMessageBox(L"Test OnActivate"); 
}


void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID)
{
	CBCGPMDIFrameWnd::OnActivateApp(bActive, dwThreadID);

	//AfxMessageBox(L"Test OnActivateApp"); 
}

void CMainFrame::OnDropFiles(HDROP hDropInfo)
{
	UINT uDragCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	TCHAR szSourcePath[MAX_PATH];

	for(UINT i = 0; i < uDragCount; i++)
	{
		DragQueryFile(hDropInfo, i, szSourcePath, MAX_PATH);
		CString strFilePath(szSourcePath); // strPath 변수에 드래그앤드랍한 파일경로가 저장된다.

		POSITION pcPosition = theApp.GetFirstDocTemplatePosition();
		CDocTemplate * pcDocTemplate = theApp.GetNextDocTemplate(pcPosition);
		CDocument * pcNewDoc = pcDocTemplate->OpenDocumentFile(strFilePath);
	}

	CBCGPMDIFrameWnd::OnDropFiles(hDropInfo);
}

//== DLL에서 전달 받은 Message를 처리하는 함수 ==========================================================
LRESULT CMainFrame::OnMsgFileImportInformation(WPARAM wParam, LPARAM lParam)
{
	if(NULL == wParam) {
		return 0;
	}

	Json::Object * pcRoot = (Json::Object *)wParam;
	DWORD_PTR nId = pcRoot->GetDwordPtr("Id");
	LONGLONG nTimeTick1 = pcRoot->GetLongLong("TimeTick1");
	LONGLONG nTimeTick2 = pcRoot->GetLongLong("TimeTick2");
	LONGLONG nTimeTick3 = pcRoot->GetLongLong("TimeTick3");

	DmiDoc * pcDoc = GetDoc(nId);
	if(nullptr == pcDoc) {
		return 0;
	}

	pcDoc->SetTimeTick(0, nTimeTick1);
	pcDoc->SetTimeTick(1, nTimeTick2);
	pcDoc->SetTimeTick(2, nTimeTick3);

	// 전달된 Error Msg 저장
	CStringArray astrHpsErrMsgArray;

	Json::Array & cHpsError = pcRoot->GetArray("HpsError");

	for(int nIndex = 0; nIndex < cHpsError.GetSize(); nIndex++) {
		CString strErrMsg = cHpsError.GetAt(nIndex)->AsString();
		astrHpsErrMsgArray.Add(strErrMsg);
	}

	pcDoc->SetHpsErrMsgArray(astrHpsErrMsgArray);

	pcDoc->UpdatePropertyBarInformation();

	return 0;
}

LRESULT CMainFrame::OnMsgFileImportExchangeInformation(WPARAM wParam, LPARAM lParam)
{
	if(NULL == wParam) {
		return 0;
	}

	Json::Object * pcRoot = (Json::Object *) wParam;
	DWORD_PTR nId = pcRoot->GetDwordPtr("Id");
	LONGLONG nTimeTick1 = pcRoot->GetLongLong("TimeTick1");
	LONGLONG nTimeTick2 = pcRoot->GetLongLong("TimeTick2");
	LONGLONG nTimeTick3 = pcRoot->GetLongLong("TimeTick3");
	LONGLONG nTimeTick4 = pcRoot->GetLongLong("TimeTick4");

	DmiDoc * pcDoc = GetDoc(nId);
	if(nullptr == pcDoc) {
		return 0;
	}

	pcDoc->SetTimeTick(0, nTimeTick1);
	pcDoc->SetTimeTick(1, nTimeTick2);
	pcDoc->SetTimeTick(2, nTimeTick3);
	pcDoc->SetTimeTick(3, nTimeTick4);

/*
	// 전달된 Error Msg 저장
	CStringArray astrHpsErrMsgArray;

	Json::Array & cHpsError = pcRoot->GetArray("HpsError");

	for(int nIndex = 0; nIndex < cHpsError.GetSize(); nIndex++) {
		CString strErrMsg = cHpsError.GetAt(nIndex)->AsString();
		astrHpsErrMsgArray.Add(strErrMsg);
	}

	pcDoc->SetHpsErrMsgArray(astrHpsErrMsgArray);
*/

	pcDoc->UpdateImportExchangePropertyBarInformation();

	return 0;
}

LRESULT CMainFrame::OnMsgSelectedItemInformation(WPARAM wParam, LPARAM lParam)
{
	if(NULL == wParam) {
		return 0;
	}

	// Json 정보 처리
	Json::Object * pcRoot = (Json::Object *) wParam;
	DWORD_PTR nId = pcRoot->GetDwordPtr("Id");
	CString strItemInfo = pcRoot->GetString("ItemInfo");

	DmiDoc * pcDoc = GetDoc(nId);
	if(nullptr == pcDoc) {
		return 0;
	}

	pcDoc->SetSelectedItemInformation(strItemInfo);

	pcDoc->UpdatePropertyBarInformation();

	return 0;
}

DmiDoc * CMainFrame::GetDoc(DWORD_PTR nId)
{
	POSITION pcTemplatePosition = theApp.GetFirstDocTemplatePosition();
	while(nullptr != pcTemplatePosition) {
		CDocTemplate * pcDocTemplate = theApp.GetNextDocTemplate(pcTemplatePosition);

		POSITION pcDocPosition = pcDocTemplate->GetFirstDocPosition();
		while(nullptr != pcDocPosition) {
			DmiDoc * pcDoc = (DmiDoc *) pcDocTemplate->GetNextDoc(pcDocPosition);
			if(nId == pcDoc->GetId()) {
				return pcDoc;
			}
		}
	}

	return nullptr;
}

