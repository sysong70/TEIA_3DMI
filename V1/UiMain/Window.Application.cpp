#include "stdafx.h"
#include "resource.h"
#include "Window.Application.h"
#include "Window.ChildFrame.h"
#include "Window.Document.h"
#include "Window.View3d.h"
#include "Window.View2d.h"
#include "Connector.h"
#include "Facility.AppResources.h"
#include "Facility.AppOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

#define PRESET PresetApplication

namespace PresetApplication
{
	CString GetInitPath(HINSTANCE hInstance /*= nullptr*/)
	{
		TCHAR lpszPath[MAX_PATH];
		DWORD nResult = ::GetModuleFileName(hInstance, lpszPath, MAX_PATH);
		ASSERT(nResult > 0);

		CString appPath(lpszPath);
		int nPos = appPath.ReverseFind(L'\\');

		return appPath.Left(nPos);
	}
}

//--------------------------------------------------------------------------------------------------

class VisualManagerCustom : public CBCGPVisualManager2019
{
public:

	DECLARE_DYNCREATE(VisualManagerCustom);

public:

	VisualManagerCustom() : CBCGPVisualManager2019() {}

public:

	BOOL DrawCheckBox(CDC* pDC, CRect rect, BOOL bHighlighted, int nState, BOOL bEnabled, BOOL bPressed) override
	{
		const COLORREF BorderColor = RGB(0xC0, 0xC0, 0xC0);
		//:WANING - replace check box(border) color
		CLocalState<COLORREF> color(globalData.clrBarDkShadow, BorderColor);
		return CBCGPVisualManagerVS2012::DrawCheckBox(pDC, rect, bHighlighted, nState, bEnabled, bPressed);
	}
};

IMPLEMENT_DYNCREATE(VisualManagerCustom, CBCGPVisualManager2019)

//--------------------------------------------------------------------------------------------------

//:REF - C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\src\mfc\doctempl.cpp

class DocTemplate3d : public CMultiDocTemplate
{
public:

	DocTemplate3d()
		: CMultiDocTemplate(IDR_DMITYPE_3D, RUNTIME_CLASS(Window::Document), RUNTIME_CLASS(Window::ChildFrame), RUNTIME_CLASS(Window::View3d)) {}



	Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch) override
	{
		ASSERT(lpszPathName != NULL);
		rpDocMatch = NULL;

		// go through all documents
		POSITION pos = GetFirstDocPosition();
		while (pos != NULL) {
			CDocument* pDoc = GetNextDoc(pos);
			if (pDoc->GetPathName() == lpszPathName) {
				// already open
				rpDocMatch = pDoc;
				return yesAlreadyOpen;
			}
		}

		if (Window::IsAllowed3d(lpszPathName)) {
			return yesAttemptNative; // extension matches, looks like ours
		}

		// otherwise we will guess it may work
		return yesAttemptForeign;
	}
};



class DocTemplate2d : public CMultiDocTemplate
{
public:

	DocTemplate2d()
		: CMultiDocTemplate(IDR_DMITYPE_2D, RUNTIME_CLASS(Window::Document), RUNTIME_CLASS(Window::ChildFrame), RUNTIME_CLASS(Window::View2d)) {}



	Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch) override
	{
		ASSERT(lpszPathName != NULL);
		rpDocMatch = NULL;

		// go through all documents
		POSITION pos = GetFirstDocPosition();
		while (pos != NULL) {
			CDocument* pDoc = GetNextDoc(pos);
			if (pDoc->GetPathName() == lpszPathName) {
				// already open
				rpDocMatch = pDoc;
				return yesAlreadyOpen;
			}
		}

		if (Window::IsAllowed2d(lpszPathName)) {
			return yesAttemptNative; // extension matches, looks like ours
		}

		// otherwise we will guess it may work
		return yesAttemptForeign;
	}
};

//--------------------------------------------------------------------------------------------------

#pragma region About Dialog

class CAboutDlg	: public CBCGPDialog
{
public:

	CAboutDlg() : CBCGPDialog(IDD_ABOUTBOX)
	{
		EnableVisualManagerStyle(TRUE, TRUE);
	}

	CBCGPURLLinkButton m_btnURL;

protected:

	void DoDataExchange(CDataExchange* pDX) override
	{
		__super::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
	}

protected:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

Window::Application TheApplication;

using namespace Window;

BEGIN_MESSAGE_MAP(Application, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(FILE_3D_CMD_New, CBCGPWinApp::OnFileNew)
END_MESSAGE_MAP()



Window::Application::Application()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	//:WARNING - use SetCustomVisualManger()
	//SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2019_BLACK);
}



Window::MainFrame& Window::Application::GetMainFrame()
{
	return *(Window::MainFrame*)m_pMainWnd;
}



Window::View* Window::Application::FindView(int id)
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != nullptr) {
		CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
		if (pDocTemplate == nullptr) {
			continue;
		}

		POSITION docPos = pDocTemplate->GetFirstDocPosition();
		while (docPos != nullptr) {
			Document* pDocument = DYNAMIC_DOWNCAST(Document, pDocTemplate->GetNextDoc(docPos));
			if (pDocument != nullptr) {
				//View* pView = DYNAMIC_DOWNCAST(View, pDocument->GetView());
				View* pView = (View*)pDocument->GetView();
				if (pView != nullptr && pView->GetId() == id) {
					return pView;
				}
			}
		}
	}

	RETURN_NULL;
}



CString Window::Application::GetAppPath(bool bLastBackslash)
{
	CString sPath = PRESET::GetInitPath(m_hInstance);
	if (bLastBackslash) {
		Path::AddBackslash(sPath);
	}

	return sPath;
}



int Window::Application::ExitInstance()
{
	Connector3d::GetInstance().application.OnExitInstance();
	//Connector2d::GetInstance().application.OnExitInstance();

	return __super::ExitInstance();
}



BOOL Window::Application::InitInstance()
{
	//Dumping objects ->
	//{1359} normal block at 0x0139D7D8, 332 bytes long.
	//Data: < > 00 00 00 00 CD CD CD CD 00 00 00 00 00 00 00 00

	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(962454); // {1359}
	//_CrtDumpMemoryLeaks();
	//_CrtMemDumpAllObjectsSince(0);

#pragma region Initialize Settings

	Facility::SetLanguage(Facility::ELanguage::English);

	TheAppResources.Load();
	TheAppOptions.SetFolderPath(GetAppPath() + L"Settings\\");
	TheAppOptions.Load();

	if (Connector3d::Initialize()) {
		Signal::Application& app = Connector3d::GetInstance().application;

		app.OnInitInstance();
		app.OnUpdatePreference(TheAppOptions.GetPreferences());

		Json::Array& tree = TheAppResources.GetDialog("FileOptions").GetArray("tree");
		Json::Object reference;
		reference.SetObject("Import", new Json::Object(tree.GetObject(0)));
		reference.SetObject("Export", new Json::Object(tree.GetObject(1)));

		app.OnFileOptionReference(reference);
		app.OnUpdateFileOption(TheAppOptions.GetFileOptions());
	}
	else {
		RETURN_FALSE;
	}

/*
	if (Connector2d::Initialize()) {
		Signal::Application& app = Connector2d::GetInstance().application;
		app.OnInitInstance();
		app.OnUpdatePreference(TheAppOptions.GetPreferences());
		app.OnUpdateFileOption(TheAppOptions.GetFileOptions());
	}
	else {
		RETURN_FALSE;
	}
*/

	double dpi = globalUtils.GetDpiForWindow(nullptr) / 96.0;
	Connector3d::GetInstance().application.OnDpiAware(dpi);

#pragma endregion //:REGION

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;

	InitCommonControlsEx(&InitCtrls);

	__super::InitInstance();
	SetCustomVisualManager();

	// Initialize OLE libraries
	if (AfxOleInit() == FALSE) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	SetRegistryKey(_T("Dataface"));
	//:WARNING - recend file counts
	LoadStdProfileSettings(10);
	SetRegistryBase(_T("Settings"));

	CMultiDocTemplate* pDocTemplate;

	if ((pDocTemplate = new DocTemplate3d()) == nullptr) {
		RETURN_FALSE;
	}
	AddDocTemplate(pDocTemplate);

	if ((pDocTemplate = new DocTemplate2d()) == nullptr) {
		RETURN_FALSE;
	}
	AddDocTemplate(pDocTemplate);

	MainFrame* pMainFrame = new MainFrame;
	if (pMainFrame->LoadFrame(IDR_MAINFRAME) == FALSE) {
		RETURN_FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->DragAcceptFiles();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//:CHECK - disable new and last document
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) {
		if (pMainFrame->LoadMDIState(GetRegSectionPath()) == FALSE ||
			DYNAMIC_DOWNCAST(CMDIChildWnd, pMainFrame->GetActiveFrame()) == nullptr) {
			if (ProcessShellCommand(cmdInfo) == FALSE) {
				RETURN_FALSE;
			}
		}
	}
	else {
		// Dispatch commands specified on the command line
		if (ProcessShellCommand(cmdInfo) == FALSE) {
			RETURN_FALSE;
		}
	}

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}



void Window::Application::OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd)
{
	__super::OnBeforeChangeVisualTheme(appOptions, pMainWnd);
	appOptions.m_strScenicRibbonLabel = _T("File");
}



void Window::Application::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}



BOOL Window::Application::SaveAllModified()
{
	if (__super::SaveAllModified() == FALSE) {
		return FALSE;
	}

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CBCGPMDIFrameWnd, m_pMainWnd);
	if (pMainFrame != nullptr) {
		pMainFrame->SaveMDIState(GetRegSectionPath());
	}

	return TRUE;
}



void Window::Application::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



void Window::Application::SetCustomVisualManager()
{
#pragma region Create visual manager - based BCGP_VISUAL_THEME_OFFICE_2019_BLACK;
	m_ActiveTheme = BCGP_VISUAL_THEME_CUSTOM;

	//:WARNING - copy from CBCGPWinApp::SetVisualTheme(), BCGP_VISUAL_THEME_OFFICE_2019_BLACK
	CBCGPVisualManager2019::SetStyle(CBCGPVisualManager2019::Office2016_Black);
	CBCGPVisualManager::SetDefaultManager(RUNTIME_CLASS(VisualManagerCustom));

	m_AppOptions.m_bMDIActiveTabBold = FALSE;
	m_AppOptions.m_MDITabsCloseButtonMode = CBCGPTabWnd::TAB_CLOSE_BUTTON_ACTIVE;
	m_AppOptions.m_bMDITabsLargeFont = TRUE;

	m_AppOptions.m_strScenicRibbonLabel = _T("File");
	m_AppOptions.m_bScenicRibbon = TRUE;
	m_AppOptions.m_bRibbonMinimizeButton = TRUE;

	CBCGPTabbedControlBar::ResetTabs();
	CBCGPDockManager::SetDockMode(BCGP_DT_SMART);
	CBCGPThemeSelectorComboBox::SelectActiveThemeInAllControls(this);

#pragma endregion //:REGION

	/// Colors

	//globalData.clrActiveBorder;
	//globalData.clrActiveCaption;
	//globalData.clrActiveCaptionGradient;
	//globalData.clrBarDkShadow;				// for CBCGPTreeCtrlEx check box, expand button, switch, ...
	//globalData.clrBarFace;
	//globalData.clrBarHilite;
	//globalData.clrBarLight;
	//globalData.clrBarShadow;
	//globalData.clrBarText;
	//globalData.clrBarWindow;
	//globalData.clrBtnDkShadow;
	//globalData.clrBtnFace;
	//globalData.clrBtnHilite;
	//globalData.clrBtnLight;
	//globalData.clrBtnShadow;
	//globalData.clrBtnText;
	//globalData.clrCaptionText;
	//globalData.clrGrayedText;
	//globalData.clrHilite;
	//globalData.clrHotLinkText;
	//globalData.clrHotText;
	//globalData.clrInactiveBorder;
	//globalData.clrInactiveCaption;
	//globalData.clrInactiveCaptionGradient;
	//globalData.clrInactiveCaptionText;
	//globalData.clrMenuText;
	//globalData.clrPrompt;
	//globalData.clrTextHilite;
	//globalData.clrWindow;
	//globalData.clrWindowFrame;
	//globalData.clrWindowText;

#pragma region // Visual manager-based tooltip

	globalData.m_bIsDlgWsCaptionStyle = TRUE;
	globalData.m_bUseDlgFontInControls = TRUE;
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

	CBCGPToolTipParams params;
	params.m_bBoldLabel = FALSE;
	params.m_bDrawDescription = FALSE;
	params.m_bDrawIcon = FALSE;
	params.m_clrFill = (COLORREF)Control::EColor::Arsenic;
	params.m_clrText = globalData.clrBarFace;
	params.m_clrBorder = globalData.clrWindowFrame;

	GetTooltipManager()->SetTooltipParams(BCGP_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CBCGPToolTipCtrl), &params);

#pragma endregion //:REGION

#pragma region // Replace all fonts

	const wchar_t DEFAULT_FONTNAME[] = L"Segoe UI";

	enum EFontIndex
	{
		fontRegular = 0,
		fontCaption,
		fontTitle,
		fontHeader,
		fontTooltip,
		fontBold,
		fontUnderline,
		fontDefaultGUI,
		fontDefaultGUIBold,
		fontDefaultGUIUnderline,
		fontVert,
		fontVertCaption,
		fontSmall,
		fontGroup,
		fontGroupBold,
		fontAppCaption,
	};

	struct fontType
	{
		CString Varialbe;
		EFontIndex Index;
		CFont& Font;
		int Height = 0;
	};

	std::vector<fontType> fontTypes = {
		{ L"fontRegular", fontRegular, globalData.fontRegular },
		{ L"fontCaption", fontCaption, globalData.fontCaption },
		{ L"fontTitle", fontTitle, globalData.fontTitle },
		{ L"fontHeader", fontHeader, globalData.fontHeader },
		{ L"fontTooltip", fontTooltip, globalData.fontTooltip },
		{ L"fontBold", fontBold, globalData.fontBold },
		{ L"fontUnderline", fontUnderline, globalData.fontUnderline },
		{ L"fontDefaultGUI", fontDefaultGUI, globalData.fontDefaultGUI },
		{ L"fontDefaultGUIBold", fontDefaultGUIBold, globalData.fontDefaultGUIBold },
		{ L"fontDefaultGUIUnderline", fontDefaultGUIUnderline, globalData.fontDefaultGUIUnderline },
		{ L"fontVert", fontVert, globalData.fontVert },
		{ L"fontVertCaption", fontVertCaption, globalData.fontVertCaption },
		{ L"fontSmall", fontSmall, globalData.fontSmall },
		{ L"fontGroup", fontGroup, globalData.fontGroup },
		{ L"fontGroupBold", fontGroupBold, globalData.fontGroupBold },
		{ L"fontAppCaption", fontAppCaption, globalData.fontAppCaption },
	};

	LOGFONT appCaption;
	globalData.fontAppCaption.GetLogFont(&appCaption);

	for (auto& type : fontTypes) {
		LOGFONT lf;
		type.Font.GetLogFont(&lf);

		::lstrcpy(lf.lfFaceName, DEFAULT_FONTNAME);
		lf.lfHeight = appCaption.lfHeight;

		type.Font.DeleteObject();
		type.Font.CreateFontIndirect(&lf);
	}

#pragma endregion //:REGION
}

#undef PRESET
