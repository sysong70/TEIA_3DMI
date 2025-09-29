#include "stdafx.h"

#include "Ast.AppOptions.h"
#include "Ast.AppResources.h"
#include "Cnt.h"
#include "Wnd.Application.h"
#include "Wnd.ChildFrame.h"
#include "Wnd.Document.h"
#include "Wnd.MainFrame.h"
#include "Wnd.View2d.h"
#include "Wnd.View3d.h"

#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

class GdiPlusManager
{
public:

	Gdiplus::GdiplusStartupInput StartupInput;
	ULONG_PTR Token;

public:

	GdiPlusManager()
	{
		Gdiplus::GdiplusStartup(&Token, &StartupInput, nullptr);
	}

	~GdiPlusManager()
	{
		Gdiplus::GdiplusShutdown(Token);
	}
};

//**************************************************************************************************

class VisualManagerCustom : public CBCGPVisualManager2019
{
public:

	VisualManagerCustom()
		: CBCGPVisualManager2019()
	{
		// KEN - 20250123, change context meuu height
		// CHECK - different with CBCGPToolBar::SetSizes()
		CBCGPToolBar::SetMenuSizes(
			globalUtils.ScaleByDPI(CSize(24, 24)),	// button size (menu height)
			globalUtils.ScaleByDPI(CSize(16, 16))	// inner image size of button
		);
	}

public:

	BOOL DrawCheckBox(CDC* pDC, CRect rect, BOOL bHighlighted, int nState, BOOL bEnabled, BOOL bPressed) override
	{
		const COLORREF borderColor = RGB(0xC0, 0xC0, 0xC0);
		// WANING - replace check box(border) color
		CLocalState<COLORREF> color(globalData.clrBarDkShadow, borderColor);
		return CBCGPVisualManagerVS2012::DrawCheckBox(pDC, rect, bHighlighted, nState, bEnabled, bPressed);
	}

	DECLARE_DYNCREATE(VisualManagerCustom);
};



IMPLEMENT_DYNCREATE(VisualManagerCustom, CBCGPVisualManager2019)

//**************************************************************************************************

namespace
{
	GdiPlusManager GdiMananger;

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

//**************************************************************************************************

//:REF - C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\atlmfc\src\mfc\doctempl.cpp

class DocTemplate3d : public CMultiDocTemplate
{
public:

	DocTemplate3d()
		: CMultiDocTemplate(IDR_DMITYPE_3D, RUNTIME_CLASS(WndDocument), RUNTIME_CLASS(WndChildFrame), RUNTIME_CLASS(WndView3d)) {}



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

		if (Wnd::IsAllowed3d(lpszPathName)) {
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
		: CMultiDocTemplate(IDR_DMITYPE_2D, RUNTIME_CLASS(WndDocument), RUNTIME_CLASS(WndChildFrame), RUNTIME_CLASS(WndView2d)) {}



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

		if (Wnd::IsAllowed2d(lpszPathName)) {
			return yesAttemptNative; // extension matches, looks like ours
		}

		// otherwise we will guess it may work
		return yesAttemptForeign;
	}
};

//**************************************************************************************************

#pragma region About Dialog

class CAboutDlg	: public CBCGPDialog
{
public:

	CAboutDlg() : CBCGPDialog(IDD_ABOUTBOX)
	{
		EnableVisualManagerStyle(TRUE, TRUE);
	}

	CBCGPURLLinkButton m_btnURL;

public:

	void DoDataExchange(CDataExchange* pDX) override
	{
		__super::DoDataExchange(pDX);

		DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
	}

public:

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

#pragma endregion // REGION

//**************************************************************************************************

WndApplication TheApp;

BEGIN_MESSAGE_MAP(WndApplication, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(FILE_3D_CMD_New, CBCGPWinApp::OnFileNew)
END_MESSAGE_MAP()



WndApplication::WndApplication()
{
#ifdef _DEBUG
/*
	Dumping objects ->
	{1359} normal block at 0x0139D7D8, 332 bytes long.
	Data: < > 00 00 00 00 CD CD CD CD 00 00 00 00 00 00 00 00

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetBreakAlloc(962454); // {1359}
	_CrtDumpMemoryLeaks();
	_CrtMemDumpAllObjectsSince(0);
*/
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(154829);
	//_CrtDumpMemoryLeaks();
	//_CrtMemDumpAllObjectsSince(0);
#endif

	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	// WARNING - use SetCustomVisualManger()
	//SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2019_BLACK);
}



CDocument* WndApplication::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CString fileName(lpszFileName);
	fileName.MakeUpper();

	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != nullptr) {
		CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
		if (pDocTemplate == nullptr) {
			continue;
		}

		POSITION docPos = pDocTemplate->GetFirstDocPosition();
		while (docPos != nullptr) {
			WndDocument* pDocument = DYNAMIC_DOWNCAST(WndDocument, pDocTemplate->GetNextDoc(docPos));
			if (pDocument != nullptr) {
				CString path = pDocument->GetPathName();
				path.MakeUpper();
				// WARNING - has same document?
				if (path == fileName) {
					return nullptr;
				}
			}
		}
	}

	return __super::OpenDocumentFile(lpszFileName);
}



CDocument* WndApplication::OpenDocumentFile(LPCTSTR lpszFileName, BOOL bAddToMRU)
{
	RETURN_NULL;
}



WndView* WndApplication::FindView(int id)
{
	POSITION pos = GetFirstDocTemplatePosition();
	while (pos != nullptr) {
		CDocTemplate* pDocTemplate = GetNextDocTemplate(pos);
		if (pDocTemplate == nullptr) {
			continue;
		}

		POSITION docPos = pDocTemplate->GetFirstDocPosition();
		while (docPos != nullptr) {
			WndDocument* pDocument = DYNAMIC_DOWNCAST(WndDocument, pDocTemplate->GetNextDoc(docPos));
			if (pDocument != nullptr) {
				//WndView* pView = DYNAMIC_DOWNCAST(WndView, pDocument->GetView());
				WndView* pView = (WndView*)pDocument->GetView();
				if (pView != nullptr && pView->ViewId == id) {
					return pView;
				}
			}
		}
	}

	RETURN_NULL;
}



CString WndApplication::GetAppPath(bool bLastBackslash)
{
	CString sPath = GetInitPath(m_hInstance);
	if (bLastBackslash) {
		Path::AddBackslash(sPath);
	}

	return sPath;
}



int WndApplication::ExitInstance()
{
	Cnt3d::GetInstance().application.OnExitInstance();
	Cnt2d::GetInstance().Application.OnExitInstance();

	return __super::ExitInstance();
}



BOOL WndApplication::InitInstance()
{
#pragma region Initialize Settings

	TheAppResources.Load();
	TheAppOptions.SetFolderPath(GetAppPath() + L"Settings\\");
	TheAppOptions.Load();

	// CHECK - using registry or TheAppOptions
	Ast::ELanguage language = Ast::ELanguage::English;
	Ast::SetLanguage(language);

	double dpi = globalUtils.GetDpiForWindow(nullptr) / 96.0;

	if (Cnt3d::Initialize()) {
		Signal::Application& app = Cnt3d::GetInstance().application;

		app.OnInitInstance();
		app.OnDpiAware(dpi);
		app.OnUpdatePreference(TheAppOptions.Preferences);

		Json::Array& tree = TheAppResources.GetDialog("FileOptions").GetArray("tree");
		Json::Object reference;
		reference.SetObject("Import", new Json::Object(tree.GetObject(0)));
		reference.SetObject("Export", new Json::Object(tree.GetObject(1)));

		app.OnFileOptionReference(reference);
		app.OnUpdateFileOption(TheAppOptions.FileOptions);
	}
	else {
		RETURN_FALSE;
	}
	// #2D_ODA
/* 
	if (Cnt2d::Initialize()) {
		//:Ken -20250128
		Cnt2d::SetLanguage((int)language);

		SgnApplication& app = Cnt2d::GetInstance().Application;

		app.OnInitInstance();
		app.OnDpiAware(dpi);
		app.OnUpdatePreference(TheAppOptions.Preferences);
		// CHECK
		app.OnUpdateFileOption(TheAppOptions.FileOptions);
	}
	else {
		RETURN_FALSE;
	}
*/

#pragma endregion // REGION

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
	// WARNING - recend file counts
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

	WndMainFrame* pMainFrame = new WndMainFrame;
	if (pMainFrame->LoadFrame(IDR_MAINFRAME) == FALSE) {
		RETURN_FALSE;
	}
	m_pMainWnd = pMainFrame;
	m_pMainWnd->DragAcceptFiles();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// CHECK - disable new and last document
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



void WndApplication::OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd)
{
	__super::OnBeforeChangeVisualTheme(appOptions, pMainWnd);
	appOptions.m_strScenicRibbonLabel = _T("File");
}



void WndApplication::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}



BOOL WndApplication::SaveAllModified()
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



void WndApplication::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}



void WndApplication::SetCustomVisualManager()
{
#pragma region Create visual manager - based BCGP_VISUAL_THEME_OFFICE_2019_BLACK;

	m_ActiveTheme = BCGP_VISUAL_THEME_CUSTOM;

	// WARNING - copy from CBCGPWinApp::SetVisualTheme(), BCGP_VISUAL_THEME_OFFICE_2019_BLACK
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

#pragma endregion // REGION

#pragma region // Visual manager-based tooltip

	/// REF - Colors

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

	globalData.m_bIsDlgWsCaptionStyle = TRUE;
	globalData.m_bUseDlgFontInControls = TRUE;
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

	CBCGPToolTipParams params;
	params.m_bBoldLabel = FALSE;
	params.m_bDrawDescription = FALSE;
	params.m_bDrawIcon = FALSE;
	params.m_clrFill = (COLORREF)Ctl::EColor::Arsenic;
	params.m_clrText = globalData.clrBarFace;
	params.m_clrBorder = globalData.clrWindowFrame;

	GetTooltipManager()->SetTooltipParams(BCGP_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CBCGPToolTipCtrl), &params);

#pragma endregion // REGION

#pragma region // Replace all fonts

	const wchar_t defaultFontName[] = L"Segoe UI";

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

		::lstrcpy(lf.lfFaceName, defaultFontName);
		lf.lfHeight = appCaption.lfHeight;

		type.Font.DeleteObject();
		type.Font.CreateFontIndirect(&lf);
	}

#pragma endregion // REGION
}
