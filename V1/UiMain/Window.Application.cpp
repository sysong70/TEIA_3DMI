#include "stdafx.h"
#include "resource.h"
#include "Window.Application.h"
#include "Window.ChildFrame.h"
#include "Window.Document.h"
#include "Window.View.h"
#include "Connector.h"
#include "Facility.AppResources.h"
#include "Path.h"

#ifdef _DEBUG
#define new DEBUG_NEW
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
END_MESSAGE_MAP()



Window::Application::Application()
{
	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;

	//AddVisualTheme(BCGP_VISUAL_THEME_OFFICE_2019_COLORFUL, CMD_THEME_LIGHT);
	//AddVisualTheme(BCGP_VISUAL_THEME_OFFICE_2019_BLACK, CMD_THEME_DARK);
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
				View* pView = DYNAMIC_DOWNCAST(View, pDocument->GetView());
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

	return __super::ExitInstance();
}



BOOL Window::Application::InitInstance()
{
#pragma region Initialize Settings

	Facility::SetLanguage(Facility::UiLanguage::English);

	if (Connector3d::Initialize()) {
		Connector3d::GetInstance().application.OnInitInstance();
	}

	//Connector2d::Initialize();

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

	LoadResource();

	SetRegistryKey(_T("Dataface"));
	LoadStdProfileSettings(4);
	SetRegistryBase(_T("Settings"));

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_DMITYPE,
		RUNTIME_CLASS(Document),
		RUNTIME_CLASS(ChildFrame), // custom MDI child frame
		RUNTIME_CLASS(View));
	if (pDocTemplate == nullptr) {
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
	params.m_clrFill = (COLORREF)Component::EColor::Arsenic;
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



void Window::Application::LoadResource()
{
	TheAppResources.Load();
}

#undef PRESET
