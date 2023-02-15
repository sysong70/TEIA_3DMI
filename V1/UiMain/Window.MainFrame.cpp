#include "stdafx.h"
#include "Window.MainFrame.h"
#include "Window.Application.h"
#include "Window.Document.h"
#include "Window.View.h"
#include "Facility.AppResources.h"
#include "Dialog.AppSettings.h"
#include "Dialog.ProgressLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



using namespace Window;

IMPLEMENT_DYNAMIC(MainFrame, CBCGPMDIFrameWnd)

BEGIN_MESSAGE_MAP(MainFrame, CBCGPMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DROPFILES()

	ON_COMMAND(FILE_3D_CMD_Open, OnFileOpen)
	ON_COMMAND(FILE_3D_CMD_Preference, OnFilePreference)
	ON_MESSAGE((UINT)EUserMessage::OnSignal, OnSignal)
	ON_MESSAGE((UINT)EUserMessage::OnNextFileOpen, OnNextFileOpen)
END_MESSAGE_MAP()



Window::MainFrame::MainFrame()
{
}



Window::MainFrame::~MainFrame()
{
}



CRect Window::MainFrame::GetMDIRect()
{
	CRect rect;
	if (m_wndClientArea.GetSafeHwnd() != nullptr) {
		m_wndClientArea.GetWindowRect(rect);
	}

	return rect;
}



Component::PanelBar& Window::MainFrame::GetPanelBar()
{
	return m_panelBar;
}



void Window::MainFrame::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::Target target = (Signal::Target)data.GetInteger(SKW_TARGET);

	switch (target) {
	case Signal::Target::View: {
		int id = data.GetInteger(SKW_VIEWID);
		View* pView = TheAppication.FindView(id);
		if (pView != nullptr) {
			pView->PostMessage((int)EUserMessage::OnSignal, (WPARAM)pData);
		}
	} break;

	case Signal::Target::MainFrame: {
		Signal::MainFrame::Action action = (Signal::MainFrame::Action)data.GetInteger(SKW_ACTION);
		switch (action) {
		case Signal::MainFrame::Action::ShowProgress:
			ShowProgress(true);
			break;

		case Signal::MainFrame::Action::HideProgress:
			ShowProgress(false);
			break;

		default:
			DEBUG_STOP;
			break;
		}

		REMOVE_POINTER(pData);
	} break;

	case Signal::Target::Progress: {
		//DEBUG_VALID(m_pDialog);
		//ASSERT(m_pDialog->GetSignalTargetId() == target);
		if (m_pDialog != nullptr && m_pDialog->GetSignalTargetId() == target) {
			m_pDialog->ReceiveSignal(pData);
		}
		else {
			REMOVE_POINTER(pData);
		}
	} break;

	default:
		DEBUG_STOP;
		REMOVE_POINTER(pData);
	}
}



void Window::MainFrame::ShowPanelBar()
{
	ShowControlBar(&m_panelBar, WS_VISIBLE, FALSE, FALSE);
	RecalcLayout();
}



void Window::MainFrame::ShowProgress(bool bShow)
{
	if (bShow) {
		DEBUG_INVALID(m_pDialog);
		m_pDialog = new Dialog::ProgressLog(this);
		m_pDialog->DoModaless();
	}
	else {
		if (m_pDialog != nullptr) {
			m_pDialog->DestroyWindow();
			REMOVE_POINTER(m_pDialog);
		}
	}
}



void Window::MainFrame::ViewChanged(UINT message, View* pView)
{
	if (message == WM_ACTIVATE) {
		m_panelBar.ViewChanged(&pView->m_tabs);
	}
}



CBCGPMDIChildWnd* Window::MainFrame::CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/)
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



HMENU Window::MainFrame::GetWindowMenuPopup(HMENU hMenuBar)
{
	return nullptr;
}



BOOL Window::MainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (__super::PreCreateWindow(cs) == FALSE) {
		RETURN_FALSE;
	}

	// TODO: modify the Window class or styles here by modifying the CREATESTRUCT cs

	return TRUE;
}

#ifdef _DEBUG

void Window::MainFrame::AssertValid() const
{
	__super::AssertValid();
}



void Window::MainFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG

LRESULT Window::MainFrame::OnSignal(WPARAM wp, LPARAM lp)
{
	ReceiveSignal((Json::Object*)wp);

	return 0;
}



LRESULT Window::MainFrame::OnNextFileOpen(WPARAM wp, LPARAM lp)
{
	if (m_fileNames.size() > 0) {
		//:WARNING - do not use CDocTemplate, or check CMultiDocTemplate
		CString fileName = m_fileNames.front();
		m_fileNames.erase(m_fileNames.begin());
		CDocument* pDoc = AfxGetApp()->OpenDocumentFile(fileName);
	}

	return S_OK;
}



void Window::MainFrame::OnClose()
{
	SaveMDIState(TheAppication.GetRegSectionPath());
	__super::OnClose();
}



void Window::MainFrame::OnCommand(UINT id)
{
	switch (id) {
	case FILE_3D_CMD_New:                   TheAppication.OnFileNew();                            return;
	case FILE_3D_CMD_Open:                  OnFileOpen();                                         return;
	case FILE_3D_CMD_Preference:            OnFilePreference();                                   return;
	case HOME_3D_CMD_Window_Cascade:        SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_CASCADE);   return;
	case HOME_3D_CMD_Window_TileHorizontal: SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_TILE_HORZ); return;
	case HOME_3D_CMD_Window_TileVertical:   SendMessage(WM_COMMAND, (WPARAM)ID_WINDOW_TILE_VERT); return;

	default:
		DEBUG_STOP;
		break;
	}
}



int Window::MainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	EnableMDITabs(TRUE, FALSE, CBCGPTabWnd::LOCATION_TOP, TRUE, CBCGPTabWnd::STYLE_UNDERLINE);
	GetMDITabs().EnableTabDocumentsMenu(TRUE, CBCGPTabWnd::TAB_DOCUMENTS_MENU_HAMBURGER);
	GetMDITabs().SetActiveTabBoldFont(FALSE);
	GetMDITabs().SetCaptionFont(TRUE);
	GetMDITabs().SetFlatFrame(TRUE);
	GetMDITabs().SetScrollButtonFullSize(TRUE);
	GetMDITabs().SetTabBorderSize(0);
	GetMDITabs().SetTabCloseButtonMode(CBCGPTabWnd::TAB_CLOSE_BUTTON_ACTIVE);

	m_ribbonBar.Initialize(this);
	m_panelBar.Initialize(this);
	m_statusBar.Initialize(this);

	DockControlBar(&m_panelBar);

	return 0;
}



void Window::MainFrame::OnDropFiles(HDROP hDropInfo)
{
	TCHAR szFileName[_MAX_PATH];
	UINT nFiles = ::DragQueryFile(hDropInfo, (UINT)-1, NULL, 0);

	for (UINT i = 0; i < nFiles; i++) {
		::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);

		if (IsAllowedFile(szFileName)) {
			m_fileNames.push_back(CString(szFileName));
		}
	}

	::DragFinish(hDropInfo);

	if (m_fileNames.size() > 0) {
		PostMessage((UINT)EUserMessage::OnNextFileOpen);
	}
	else {
		BCGPMessageBox(L"No files are allowed. Please select a different file.");
	}

	__super::OnDropFiles(hDropInfo);
}



void Window::MainFrame::OnFileOpen()
{
	const DWORD SHOW_OPTION = WM_USER;

	Json::Object& dialog = TheAppResources.GetDialog("FileOpen");
	Json::Array& arr = dialog.GetArray("filter");
	CString filter;
	for (Json::Value* pValue : arr.GetBuffer()) {
		filter += pValue->AsString();
	}

//#define USE_OPTION_DLG

#ifdef USE_OPTION_DLG
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter, this);
	//:WARNING
	dlg.AddCheckButton(SHOW_OPTION, Facility::Local(L"Show import option|파일 옵션 보기"), TRUE);
	dlg.MakeProminent(SHOW_OPTION); // align to buttons

	if (dlg.DoModal() == IDOK) {
		m_fileNames.push_back(dlg.GetFileName());

		BOOL bShow = FALSE;
		dlg.GetCheckButtonState(WM_USER, bShow);
		if (bShow) {
			//:TODO - show option dialog
		}
		else {
			//:WARNING
			m_importOption.Clean();
		}

		PostMessage((UINT)EUserMessage::OnNextFileOpen);
	}
#else
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, filter, this);

	if (dlg.DoModal() == IDOK) {
		POSITION pos = dlg.GetStartPosition();
		while (pos != nullptr) {
			CString path = dlg.GetNextPathName(pos);
			if (IsAllowedFile(path)) {
				m_fileNames.push_back(path);
			}
		}

		if (m_fileNames.size() > 0) {
			PostMessage((UINT)EUserMessage::OnNextFileOpen);
		}
		else {
			BCGPMessageBox(L"No files are allowed. Please select a different file.");
		}
	}
#endif

#undef USE_OPTION_DLG
}



void Window::MainFrame::OnFilePreference()
{
	Dialog::AppSettings dlg;
	dlg.DoModal();
}



bool Window::MainFrame::HasNextFile()
{
	return m_fileNames.size() > 0;
}



void Window::MainFrame::OpenNextFile()
{
}
