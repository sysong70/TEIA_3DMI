#include "stdafx.h"
#include "resource.h"
#include "Window.Application.h"
#include "Window.ChildFrame.h"
#include "Window.Document.h"
#include "Window.View3d.h"
#include "Window.View2d.h"
#include "Connector.h"
#include "Facility.AppResources.h"
#include "Facility.AppSettings.h"
#include <Path.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



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
}



#pragma region About Dialog

class CAboutDlg
	: public CBCGPDialog
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

Window::Application TheAppication;

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
	SetVisualTheme(BCGP_VISUAL_THEME_OFFICE_2019_BLACK);
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



CString Window::Application::Path(bool bLastBackslash)
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
	//_CrtSetBreakAlloc(1359); // {1359}
	//_CrtDumpMemoryLeaks();
	//_CrtMemDumpAllObjectsSince(0);

#pragma region Initialize Settings

	Facility::SetLanguage(Facility::ELanguage::English);

	TheAppResources.Load();
	TheAppSettings.SetFolderPath(Path());
	TheAppSettings.Load();

	if (Connector3d::Initialize()) {
		Connector3d::GetInstance().application.OnInitInstance();
	}
	if (Connector2d::Initialize()) {
		Connector2d::GetInstance().application.OnInitInstance();
	}

	double dpi = globalUtils.GetDpiForWindow(nullptr) / 96.0;
	Connector3d::GetInstance().application.OnDpiAware(dpi);

#pragma endregion //:REGION

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;

	InitCommonControlsEx(&InitCtrls);

	__super::InitInstance();
	InitializeBcg();

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

	if ((pDocTemplate = new PRESET::DocTemplate3d()) == nullptr) {
		RETURN_FALSE;
	}
	AddDocTemplate(pDocTemplate);

	if ((pDocTemplate = new PRESET::DocTemplate2d()) == nullptr) {
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



void Window::Application::InitializeBcg()
{
	//COLORREF clrActiveBorder = globalData.clrActiveBorder;
	//COLORREF clrActiveCaption = globalData.clrActiveCaption;
	//COLORREF clrActiveCaptionGradient = globalData.clrActiveCaptionGradient;
	//COLORREF clrBarDkShadow = globalData.clrBarDkShadow;
	//COLORREF clrBarFace = globalData.clrBarFace;
	//COLORREF clrBarHilite = globalData.clrBarHilite;
	//COLORREF clrBarLight = globalData.clrBarLight;
	//COLORREF clrBarShadow = globalData.clrBarShadow;
	//COLORREF clrBarText = globalData.clrBarText;
	//COLORREF clrBarWindow = globalData.clrBarWindow;
	//COLORREF clrBtnDkShadow = globalData.clrBtnDkShadow;
	//COLORREF clrBtnFace = globalData.clrBtnFace;
	//COLORREF clrBtnHilite = globalData.clrBtnHilite;
	//COLORREF clrBtnLight = globalData.clrBtnLight;
	//COLORREF clrBtnShadow = globalData.clrBtnShadow;
	//COLORREF clrBtnText = globalData.clrBtnText;
	//COLORREF clrCaptionText = globalData.clrCaptionText;
	//COLORREF clrGrayedText = globalData.clrGrayedText;
	//COLORREF clrHilite = globalData.clrHilite;
	//COLORREF clrHotLinkText = globalData.clrHotLinkText;
	//COLORREF clrHotText = globalData.clrHotText;
	//COLORREF clrInactiveBorder = globalData.clrInactiveBorder;
	//COLORREF clrInactiveCaption = globalData.clrInactiveCaption;
	//COLORREF clrInactiveCaptionGradient = globalData.clrInactiveCaptionGradient;
	//COLORREF clrInactiveCaptionText = globalData.clrInactiveCaptionText;
	//COLORREF clrMenuText = globalData.clrMenuText;
	//COLORREF clrPrompt = globalData.clrPrompt;
	//COLORREF clrTextHilite = globalData.clrTextHilite;
	//COLORREF clrWindow = globalData.clrWindow;
	//COLORREF clrWindowFrame = globalData.clrWindowFrame;
	//COLORREF clrWindowText = globalData.clrWindowText;

	globalData.m_bIsDlgWsCaptionStyle = TRUE;
	globalData.m_bUseDlgFontInControls = TRUE;
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

	// Visual manager-based tooltip

	CBCGPToolTipParams params;
	params.m_bBoldLabel = FALSE;
	params.m_bDrawDescription = FALSE;
	params.m_bDrawIcon = FALSE;
	params.m_clrFill = (COLORREF)Control::EColor::Arsenic;
	params.m_clrText = globalData.clrBarFace;
	params.m_clrBorder = globalData.clrWindowFrame;

	GetTooltipManager()->SetTooltipParams(BCGP_TOOLTIP_TYPE_ALL, RUNTIME_CLASS(CBCGPToolTipCtrl), &params);

	// Replace fonts

	enum FontIndex
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
		FontIndex Index;
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

		//TRACE(L"%s, %s, %d\n", (LPCTSTR)type.Varialbe, lf.lfFaceName, lf.lfHeight);
		::lstrcpy(lf.lfFaceName, L"Segoe UI");
		lf.lfHeight = appCaption.lfHeight;

		type.Font.DeleteObject();
		type.Font.CreateFontIndirect(&lf);
	}
}

#undef PRESET
