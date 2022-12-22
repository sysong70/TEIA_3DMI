// RCT3D.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "DmiApp.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "DmiDoc.h"
#include "DmiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DLL/DmiCommandAnalyzerDll.h"

#include "../Common/DmiCommandDefine.h"

// CAboutDlg dialog used for App About

class CAboutDlg : public CBCGPDialog
{
public:
	CAboutDlg();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
	CBCGPURLLinkButton m_btnURL;

protected:
	virtual void DoDataExchange(CDataExchange * pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CBCGPDialog(IDD_ABOUTBOX)
{
	EnableVisualManagerStyle(TRUE, TRUE);
}

void CAboutDlg::DoDataExchange(CDataExchange * pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMPANY_URL, m_btnURL);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CBCGPDialog)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(DmiApp, CBCGPWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CBCGPWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CBCGPWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CBCGPWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

class CAboutDlg;

// DmiApp construction
DmiApp::DmiApp()
{

	// Support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;


	SetApplicationLookMenu(ID_VIEW_APPLOOK, BCGP_THEMES_LIST_OFFICE_2007 | BCGP_THEMES_LIST_OFFICE_2010 | BCGP_THEMES_LIST_SCENIC | BCGP_THEMES_LIST_OFFICE_2013 | BCGP_THEMES_LIST_OFFICE_2016 | BCGP_THEMES_LIST_OFFICE_2019);

	// Enable VS 2019 look (Blue theme)
	SetVisualTheme(BCGP_VISUAL_THEME_VS_2019_BLUE);


	m_pcCommandAnalyzer = nullptr;
}


// The one and only DmiApp object
DmiApp theApp;


// DmiApp initialization
BOOL DmiApp::InitInstance()
{
	m_pcCommandAnalyzer = new RtCommandAnalyzerDll();

	Json::Object cObject;
	cObject.SetDwordPtr((UINT) DmiHps::Command::Id, (DWORD_PTR) DmiHps::Command::HpsInit);

	ExecuteHpsCommand(cObject);

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CBCGPWinApp::InitInstance();

	// Remove this line if you don't need themed product dialogs:
	globalData.m_bUseVisualManagerInBuiltInDialogs = TRUE;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: you should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("RCT3D"));

	LoadStdProfileSettings(20);  // Load standard INI file options (including MRU)

	SetRegistryBase(_T("Settings"));

	// Initialize all Managers for usage. They are automatically constructed
	// if not yet present
	// Enable user-defined tools. If you want allow more than 10 tools,
	// add tools entry to resources (ID_USER_TOOL11, ID_USER_TOOL12,...)
	EnableUserTools(ID_TOOLS_ENTRY, ID_USER_TOOL1, ID_USER_TOOL10);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_RCT3DTYPE,
		RUNTIME_CLASS(DmiDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(DmiView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame * pMainFrame = new CMainFrame;
	if(!pMainFrame->LoadFrame(IDR_MAINFRAME)) {
		return FALSE;
	}
		
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	
/*
	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
	{
		if (!pMainFrame->LoadMDIState(GetRegSectionPath()) ||
			DYNAMIC_DOWNCAST(CMDIChildWnd, pMainFrame->GetActiveFrame()) == NULL)
		{
			if (!ProcessShellCommand(cmdInfo))
				return FALSE;
		}
	}
	else
	{
		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}
*/

	// Dispatch commands specified on the command line
	if(!ProcessShellCommand(cmdInfo)) {
		return FALSE;
	}

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

// DmiApp message handlers

int DmiApp::ExitInstance()
{
	if(nullptr != m_pcCommandAnalyzer)
	{
		Json::Object cObject;
		cObject.SetDwordPtr((UINT) DmiHps::Command::Id, (DWORD_PTR) DmiHps::Command::HpsExitDisplay);
		ExecuteHpsCommand(cObject);

		delete m_pcCommandAnalyzer;
	}

	return CBCGPWinApp::ExitInstance();
}

// App command to run the dialog
void DmiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// DmiApp message handlers


void DmiApp::PreLoadState()
{
	GetContextMenuManager()->AddMenu(_T("My menu"), IDR_CONTEXT_MENU);

	// TODO: add another context menus here
}

BOOL DmiApp::SaveAllModified()
{
	if (!CBCGPWinApp::SaveAllModified())
	{
		return FALSE;
	}

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST(CBCGPMDIFrameWnd, m_pMainWnd);
	if (pMainFrame != NULL)
	{
		pMainFrame->SaveMDIState(GetRegSectionPath());
	}

	return TRUE;
}

void DmiApp::OnBeforeChangeVisualTheme(CBCGPAppOptions& appOptions, CWnd* pMainWnd)
{
	CBCGPWinApp::OnBeforeChangeVisualTheme(appOptions, pMainWnd);
	appOptions.m_strScenicRibbonLabel = _T("File");
}

bool DmiApp::ExecuteCommand(Json::Object & cInObject)
{
	CString strStringifyText;
	cInObject.Stringify(strStringifyText);

	return m_pcCommandAnalyzer->ExecuteCommand(strStringifyText);
}

bool DmiApp::ExecuteHpsCommand(Json::Object & cInObject)
{
	cInObject.SetDwordPtr((UINT) DmiHps::Command::Type, (DWORD_PTR) DmiHps::Command::MsgHpsType);

	return ExecuteCommand(cInObject);
}

bool DmiApp::ExecuteCommand(Json::Object & cInObject, Json::Object & cRetObject)
{
	return m_pcCommandAnalyzer->ExecuteCommandType2(cInObject, cRetObject);
}

void DmiApp::SetMessageCallback(void(CALLBACK * lpfnCallback)(DWORD_PTR *, Json::Object &), DWORD_PTR * pcTarget)
{
	m_pcCommandAnalyzer->SetMessageCallback(lpfnCallback, pcTarget);
}