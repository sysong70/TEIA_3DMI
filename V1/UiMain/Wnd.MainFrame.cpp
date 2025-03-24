#include "stdafx.h"

#include "Ast.AppResources.h"
#include "Cnt.h"
#include "Dlg.AppOptions.h"
#include "Dlg.ProgressBar.h"
#include "Wnd.Application.h"
#include "Wnd.Document.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	void RemoveMenuItems(CMenu* pMenu)
	{
		while (pMenu->GetMenuItemCount() > 0) {
			UINT id = pMenu->GetMenuItemID(0);
			if (id == (UINT)-1) {
				CMenu* pSubMenu = pMenu->GetSubMenu(0);
				RemoveMenuItems(pSubMenu);
				pMenu->DeleteMenu(0, MF_BYPOSITION);
			}
			else {
				pMenu->RemoveMenu(0, MF_BYPOSITION);
			}
		}
	}
}

//**************************************************************************************************

IMPLEMENT_DYNAMIC(WndMainFrame, CBCGPMDIFrameWnd)

BEGIN_MESSAGE_MAP(WndMainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()
	ON_WM_SIZE()

	ON_COMMAND(FILE_3D_CMD_Open, OnFileOpen)
	ON_COMMAND(FILE_3D_CMD_Options, OnAppOptions)
	ON_MESSAGE((UINT)Wnd::EMessage::OnSignal2d, OnSignal2d)
	ON_MESSAGE((UINT)Wnd::EMessage::OnSignal3d, OnSignal3d)
	ON_MESSAGE((UINT)Wnd::EMessage::OnNextFileOpen, OnNextFileOpen)
END_MESSAGE_MAP()



WndMainFrame::WndMainFrame()
{
	ContextMenu.CreatePopupMenu();
}



DlgDebugTracer& WndMainFrame::GetDebugTracer()
{
	DlgBase* pDialog = Dialogs.Get((int)Dlg::EIndex::DebugTracer);
	if (pDialog == nullptr) {
		pDialog = new DlgDebugTracer;
		pDialog->Create(IDD_DMI_DEBUG_TRACER);

		Dialogs.Add(pDialog);
	}

	pDialog->ShowWindow(SW_SHOW);

	return *(DlgDebugTracer*)pDialog;
}



CMenu& WndMainFrame::GetContextMenu(bool clearFirst)
{
	if (clearFirst) {
		RemoveMenuItems(&ContextMenu);
	}

	return ContextMenu;
}



CRect WndMainFrame::GetMDIRect()
{
	CRect rect;
	if (m_wndClientArea.GetSafeHwnd() != nullptr) {
		m_wndClientArea.GetWindowRect(rect);
	}

	return rect;
}



void WndMainFrame::ShowPanelBar()
{
	ShowControlBar(&PanelBarCtl, WS_VISIBLE, FALSE, FALSE);
	RecalcLayout();
}



void WndMainFrame::ShowProgress(bool bShow)
{
	if (bShow) {
		BeginWaitCursor();
		UpdateWindow();

		DlgBase* pDialog = Dialogs.Get((int)Signal::Target::Progress);
		if (pDialog == nullptr) {
			pDialog = new DlgProgressBar(this);
			Dialogs.Add(pDialog);
		}

		pDialog->DoModaless();
	}
	else {
		Dialogs.Remove((int)Signal::Target::Progress);

		EndWaitCursor();
		UpdateWindow();
	}
}



void WndMainFrame::ShowTaskBar(bool show)
{
	if (show) {
		TaskBarCtl.ShowWindow(SW_SHOW);
		TaskBarCtl.AdjustLayout();
	}
	else {
		TaskBarCtl.ShowWindow(SW_HIDE);
	}
}



void WndMainFrame::ViewChanged(UINT message, WndView* pView)
{
	if (message == WM_ACTIVATE) {
		PanelBarCtl.ViewChanged(pView->GetPanelManager());

		if (ActiveView == nullptr ||
			ActiveView->ViewType != pView->ViewType) {
			RibbonBarCtl.ChangeByDocType(pView->GetDocument()->GetDocType());
			UioManager.ViewChanged(pView);
		}

		if (ActiveView != nullptr &&
			ActiveView->ViewId != -1 &&
			ActiveView->ViewId != pView->ViewId) {
			ActiveView->CancelCommand();
		}
	}
	else if (message == WM_CREATE) {
	}
	else if (message == WM_DESTROY) {
		if (ActiveView == pView) {
			ActiveView = nullptr;
		}

		return;
	}
	else {
		DEBUG_STOP;
	}

	ActiveView = pView;
}



bool WndMainFrame::HasCommandHandeler(UINT id)
{
	switch (id) {
		case FILE_3D_CMD_New:
		case FILE_3D_CMD_Open:
		case FILE_3D_CMD_Options:
		case HOME_3D_CMD_Window_Cascade:
		case HOME_2D_CMD_Window_Cascade:
		case HOME_3D_CMD_Window_TileHorizontal:
		case HOME_2D_CMD_Window_TileHorizontal:
		case HOME_3D_CMD_Window_TileVertical:
		case HOME_2D_CMD_Window_TileVertical:
			return true;

		default:
			return false;
	}
}



CBCGPMDIChildWnd* WndMainFrame::CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/)
{
	if (lpcszDocName != nullptr && lpcszDocName[0] != '\0') {
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(lpcszDocName);
		if (pDoc != nullptr) {
			POSITION pos = pDoc->GetFirstViewPosition();

			if (pos != nullptr) {
				CView* pView = pDoc->GetNextView(pos);
				if (pView == nullptr) {
					return nullptr;
				}

				return DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pView->GetParent());
			}
		}
	}

	return nullptr;
}



HMENU WndMainFrame::GetWindowMenuPopup(HMENU hMenuBar)
{
	return nullptr;
}


/*
BOOL WndMainFrame::OnDrawMenuImage(CDC* pDC, const CBCGPToolbarMenuButton* pMenuButton, const CRect& rectImage)
{
// TODO - 2D menu is not registered at this time. Will fix it later.
#define OnMenuId(x) case CONTEXT_2D_CMD_ObjectSnap_##x: id = HOME_3D_CMD_ObjectSnap_##x; break

	UINT id = 0;

	switch (pMenuButton->m_nID) {
		OnMenuId(Point);
		OnMenuId(End);
		OnMenuId(Mid);
		OnMenuId(Intersection);
		OnMenuId(Perpendicular);
		OnMenuId(Center);
		OnMenuId(Quadrant);
		OnMenuId(Near);

		case CONTEXT_2D_POP_ObjectSnap_Overrides:
			id = HOME_3D_POP_ObjectSnap;
			break;

		default:
			return __super::OnDrawMenuImage(pDC, pMenuButton, rectImage);
	}

	CSize size = globalUtils.ScaleByDPI(CSize(16, 16));
	int offsetX = (rectImage.Width() - size.cx) / 2;
	int offsetY = (rectImage.Height() - size.cy) / 2;

	CRect rect = rectImage;
	rect.DeflateRect(offsetX, offsetY);

	CBCGPBaseRibbonElement* pElem = m_ribbonBar.FindByID(id, FALSE, TRUE);
	if (pElem != nullptr) {
		pElem->OnDrawMenuImage(pDC, rect);
	}
	else {
		DEBUG_STOP;
	}

#undef OnMenuId
}
*/


BOOL WndMainFrame::OnEraseMDIClientBackground(CDC* pDC)
{
	HBITMAP hBitmap = TheAppResources.BkHandle;
	CBitmap* pBitmap = CBitmap::FromHandle(hBitmap);
	BITMAP bitmap;
	pBitmap->GetBitmap(&bitmap);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	memDC.SelectObject(pBitmap);

	CRect rect;
	m_wndClientArea.GetClientRect(rect);

	pDC->FillRect(rect, &CBrush(RGB(0x36, 0x36, 0x36)));
	pDC->BitBlt(rect.left, rect.bottom - bitmap.bmHeight, bitmap.bmWidth, bitmap.bmHeight, &memDC, 0, 0, SRCCOPY);

	return TRUE;
}



void WndMainFrame::OnSizeMDIClient(const CRect& rectOld, const CRect& rectNew)
{
	m_wndClientArea.RedrawWindow();
}



BOOL WndMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (__super::PreCreateWindow(cs) == FALSE) {
		RETURN_FALSE;
	}

	// TODO: modify the Window class or styles here by modifying the CREATESTRUCT cs

	return TRUE;
}

#ifdef _DEBUG

void WndMainFrame::AssertValid() const
{
	__super::AssertValid();
}



void WndMainFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

LRESULT WndMainFrame::OnSignal2d(WPARAM wp, LPARAM lp)
{
	Json::Object* pData = (Json::Object*)wp;
	Json::Object& data = *pData;
	Sgn::ETarget target = (Sgn::ETarget)data.GetInteger(SKW_TARGET);

	switch (target) {
#ifdef _DEBUG
	case Sgn::ETarget::DebugTracer: {
		DlgBase& dlg = GetDebugTracer();
		dlg.ShowWindow(SW_SHOW);
		dlg.ReceiveSignal2d(pData);

		REMOVE_POINTER(pData);
	} break;
#endif

	case Sgn::ETarget::Command: {
		int id = data.GetInteger(SKW_VIEWID, -1);

		if (id == -1) {
			//:TODO - active command
			REMOVE_POINTER(pData);
		}
		else {
			WndView* pView = TheApp.FindView(id);
			if (pView != nullptr) {
				pView->PostMessage((int)Wnd::EMessage::OnSignal2d, (WPARAM)pData);
			}
			else {
				DEBUG_STOP;
				REMOVE_POINTER(pData);
			}
		}
	} break;

	case Sgn::ETarget::MainFrame: {
		SgnMainFrame::Action action = (SgnMainFrame::Action)data.GetInteger(SKW_ACTION, -1);

		switch (action) {
		case SgnMainFrame::Action::ShowProgressBar: {
			BeginWaitCursor();
			UpdateWindow();

			DlgBase* pDialog = Dialogs.Get((int)Dlg::EIndex::ProgressBar);
			if (pDialog == nullptr) {
				pDialog = new DlgProgressBar(this);
				Dialogs.Add(pDialog);
			}

			pDialog->DoModaless();
		} break;

		case SgnMainFrame::Action::HideProgressBar: {
			Dialogs.Remove((int)Dlg::EIndex::ProgressBar);

			EndWaitCursor();
			UpdateWindow();
		} break;

		default:
			DEBUG_STOP;
			break;
		}

		REMOVE_POINTER(pData);
	} break;

	case Sgn::ETarget::ProgressBar: {
		if (auto pDialog = Dialogs.Get((int)Dlg::EIndex::ProgressBar)) {
			pDialog->ReceiveSignal2d(pData);
		}
		else {
			//:WARNING - not found dialog, ignore message
			REMOVE_POINTER(pData);
		}
	} break;

	case Sgn::ETarget::StatusBar: {
		StatusBarCtl.ReceiveSignal(pData);
	} break;

	case Sgn::ETarget::UserIO: {
		UioManager.ReceiveSignal(pData);
	} break;

	case Sgn::ETarget::View: {
		int id = data.GetInteger(SKW_VIEWID, -1);
		WndView* pView = TheApp.FindView(id);

		if (pView != nullptr) {
			pView->PostMessage((int)Wnd::EMessage::OnSignal2d, (WPARAM)pData);
		}
		else {
			DEBUG_STOP;
			REMOVE_POINTER(pData);
		}
	} break;

	default:
		DEBUG_STOP;
		break;
	}

	return 0;
}



LRESULT WndMainFrame::OnSignal3d(WPARAM wp, LPARAM lp)
{
	Json::Object* pData = (Json::Object*)wp;
	Json::Object& data = *pData;
	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET);

	if (target == Signal::Target::View || target == Signal::Target::ModelPanel || target == Signal::Target::TaskBar) {
		int id = data.GetInteger(SKW_VIEWID, -1);
		WndView* pView = TheApp.FindView(id);

		if (pView != nullptr) {
			pView->PostMessage((int)Wnd::EMessage::OnSignal3d, (WPARAM)pData);
		}
		else {
			DEBUG_STOP;
			REMOVE_POINTER(pData);
		}

		return 0;
	}

	if (target == Signal::Target::StatusBar) {
		StatusBarCtl.ReceiveSignal(pData);
		return 0;
	}

	if (target == Signal::Target::Command) {
		int id = data.GetInteger(SKW_VIEWID, -1);

		if (id == -1) {
			// TODO - active command
			REMOVE_POINTER(pData);
		}
		else {
			WndView* pView = TheApp.FindView(id);
			if (pView != nullptr) {
				pView->PostMessage((int)Wnd::EMessage::OnSignal3d, (WPARAM)pData);
			}
			else {
				DEBUG_STOP;
				REMOVE_POINTER(pData);
			}
		}

		return 0;
	}

	if (target == Signal::Target::MainFrame) {
		Signal::MainFrame::Action action = (Signal::MainFrame::Action)data.GetInteger(SKW_ACTION, -1);

		switch (action) {
		case Signal::MainFrame::Action::ShowProgress:
		{
			BeginWaitCursor();
			UpdateWindow();

			DlgBase* pDialog = Dialogs.Get((int)Dlg::EIndex::ProgressBar);
			if (pDialog == nullptr) {
				pDialog = new DlgProgressBar(this);
				Dialogs.Add(pDialog);
			}

			pDialog->DoModaless();
		}
		break;

		case Signal::MainFrame::Action::HideProgress:
		{
			Dialogs.Remove((int)Dlg::EIndex::ProgressBar);

			EndWaitCursor();
			UpdateWindow();
		}
		break;

		default:
			DEBUG_STOP;
			break;
		}

		REMOVE_POINTER(pData);
		return 0;
	}

	if (target == Signal::Target::Progress) {
		if (auto pDialog = Dialogs.Get((int)Dlg::EIndex::ProgressBar)) {
			pDialog->ReceiveSignal3d(pData);
		}
		else {
			DEBUG_STOP;
			REMOVE_POINTER(pData);
		}

		return 0;
	}

	DEBUG_STOP;
	return 0;
}



LRESULT WndMainFrame::OnNextFileOpen(WPARAM wp, LPARAM lp)
{
	if (WaitingFileNames.size() > 0) {
		// WARNING - do not use CDocTemplate, or check CMultiDocTemplate
		CString fileName = WaitingFileNames.front();
		WaitingFileNames.erase(WaitingFileNames.begin());
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(fileName);
	}

	return S_OK;
}



void WndMainFrame::OnClose()
{
	SaveMDIState(TheApp.GetRegSectionPath());
	__super::OnClose();
}



void WndMainFrame::OnCommand(UINT id)
{
	switch (id) {
	case FILE_3D_CMD_New:
		TheApp.OnFileNew();
		return;

	case FILE_3D_CMD_Open:
		OnFileOpen();
		return;

	case FILE_3D_CMD_Options:
		OnAppOptions();
		return;

	case HOME_3D_CMD_Window_Cascade:
	case HOME_2D_CMD_Window_Cascade:
		SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_CASCADE);
		return;

	case HOME_3D_CMD_Window_TileHorizontal:
	case HOME_2D_CMD_Window_TileHorizontal:
		SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_TILE_HORZ);
		return;

	case HOME_3D_CMD_Window_TileVertical:
	case HOME_2D_CMD_Window_TileVertical:
		SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_TILE_VERT);
		return;

	default:
		DEBUG_STOP;
		break;
	}
}



int WndMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	EnableMDITabs(TRUE, FALSE, CBCGPTabWnd::LOCATION_TOP, TRUE, CBCGPTabWnd::STYLE_UNDERLINE);

	CBCGPTabWnd& mdi = GetMDITabs();
	mdi.EnableTabDocumentsMenu(TRUE, CBCGPTabWnd::TAB_DOCUMENTS_MENU_HAMBURGER);
	mdi.SetActiveTabBoldFont(FALSE);
	mdi.SetCaptionFont(TRUE);
	mdi.SetFlatFrame(TRUE);
	mdi.SetScrollButtonFullSize(TRUE);
	mdi.SetTabBorderSize(0);
	mdi.SetTabCloseButtonMode(CBCGPTabWnd::TAB_CLOSE_BUTTON_ACTIVE);

	RibbonBarCtl.Initialize(this);
	PanelBarCtl.Initialize(this);
	StatusBarCtl.Initialize(this);
	TaskBarCtl.Initialize(this);

	DockControlBar(&PanelBarCtl);

	UioManager.Initialize(this);

	return 0;
}



void WndMainFrame::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFileName[_MAX_PATH];
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	for (UINT i = 0; i < nFiles; i++) {
		::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);

		if (Wnd::IsAllowedFile(szFileName)) {
			WaitingFileNames.push_back(CString(szFileName));
		}
	}

	::DragFinish(hDropInfo);

	if (WaitingFileNames.size() > 0) {
		PostMessage((UINT)Wnd::EMessage::OnNextFileOpen);
	}
	else {
		BCGPMessageBox(L"No files are allowed. Please select a different file.");
	}

	__super::OnDropFiles(hDropInfo);
}



void WndMainFrame::OnFileOpen()
{
	Json::Object& dialog = TheAppResources.GetDialog("FileOptions");
	Json::Array& arr = dialog.GetArray("tree").GetObject(0).GetArray("items");

	CString filters = L"All types (*.*)|*.*|";

	for (Json::Value* pValue : arr.GetBuffer()) {
		Json::Object& object = pValue->AsObject();
		if (object.GetBoolean("visible", true) == false) {
			continue;
		}

		// "ACIS "
		filters += object.GetString("title") + L" ";
		// "*.SAT, *.SAB, "
		Json::Array& ext = object.GetArray("ext");
		CString extensions;
		for (Json::Value* pExt : ext.GetBuffer()) {
			extensions += L"*." + pExt->AsString() + L", ";
		}
		// "(*.sat, *.sab)"
		extensions.TrimRight(L", ");
		// "ACIS (*.sat, *.sab)
		filters += L"(" + extensions.MakeLower() + L")";
		// |*.sat;*.sab|
		extensions.Replace(L", ", L";");
		// "ACIS (*.sat, *.sab)|*.sat;*.sab|
		filters += L"|" + extensions + L"|";
	}

	// add end mark, "All types (*.*)|*.*|ACIS (*.sat, *.sab)|*.sat;*.sab||"
	filters += L"|";

//#define USE_OPTION_DLG

#ifdef USE_OPTION_DLG
	const DWORD SHOW_OPTION = WM_USER;

	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter, this);
	// WARNING
	dlg.AddCheckButton(SHOW_OPTION, Ast::Local(L"Show import option|파일 옵션 보기"), TRUE);
	dlg.MakeProminent(SHOW_OPTION); // align to buttons

	if (dlg.DoModal() == IDOK) {
		WaitingFileNames.push_back(dlg.GetFileName());

		BOOL bShow = FALSE;
		dlg.GetCheckButtonState(WM_USER, bShow);
		if (bShow) {
			// TODO - show option dialog
		}
		else {
			// WARNING
			m_importOption.Clean();
		}

		PostMessage((UINT)EMessage::OnNextFileOpen);
	}
#else
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, filters, this);

	if (dlg.DoModal() == IDOK) {
		POSITION pos = dlg.GetStartPosition();
		while (pos != nullptr) {
			CString path = dlg.GetNextPathName(pos);
			if (Wnd::IsAllowedFile(path)) {
				WaitingFileNames.push_back(path);
			}
		}

		if (WaitingFileNames.size() > 0) {
			PostMessage((UINT)Wnd::EMessage::OnNextFileOpen);
		}
		else {
			BCGPMessageBox(L"No files are allowed. Please select a different file.");
		}
	}
#endif

#undef USE_OPTION_DLG
}



void WndMainFrame::OnAppOptions()
{
	DlgAppOptions dlg;
	dlg.DoModal();
}



void WndMainFrame::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		TaskBarCtl.AdjustLayout();
		UioManager.AdjustLayout();
	}
}



bool WndMainFrame::HasNextFile()
{
	return WaitingFileNames.size() > 0;
}



void WndMainFrame::OpenNextFile()
{
}
