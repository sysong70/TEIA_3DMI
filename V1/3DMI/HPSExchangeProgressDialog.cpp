#include "pch.h"
#include "HPSExchangeProgressDialog.h"
#include "afxdialogex.h"

#include "DmiApp.h"
#include "DLL/DmiJsonCmd.h"
#include "../Common/DmiCommandDefine.h"

#include <Json.h>
#include <vector>
#include <mutex>

std::mutex mtx;

IMPLEMENT_DYNAMIC(HPSExchangeProgressDialog, CBCGPDialog)

BEGIN_MESSAGE_MAP(HPSExchangeProgressDialog, CBCGPDialog)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &HPSExchangeProgressDialog::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK_KEEP_OPEN, &HPSExchangeProgressDialog::OnBnClickedCheckKeepOpen)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

HPSExchangeProgressDialog::HPSExchangeProgressDialog(DWORD_PTR nDocId) :// , int nViewId, CString strTitle) : 
	CBCGPDialog(HPSExchangeProgressDialog::IDD)
	, m_nDocId(nDocId)
	, m_bSuccess(false)
	, m_bKeepDialogOpen(false)
{
	m_nLogMessageLineCount = 0;
	// _import_status_event = new ImportStatusEventHandler(this);

	EnableLayout();
}

HPSExchangeProgressDialog::~HPSExchangeProgressDialog()
{
	// delete _import_status_event;
}

void HPSExchangeProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CBCGPDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_MESSAGES, m_wndEditBoxCtrl);
	DDX_Control(pDX, IDC_PROGRESS_BAR, m_wndProgressCtrl);
}

BOOL HPSExchangeProgressDialog::OnInitDialog()
{
	CBCGPDialog::OnInitDialog();

	EnableLoadWindowPlacement();

	// Set our progress bar range
	m_wndProgressCtrl.SetRange(0, 100);
	m_wndProgressCtrl.ModifyStyle(0, PBS_MARQUEE);
	m_wndProgressCtrl.SetMarquee(TRUE, 50);

	GetDlgItem(IDC_IMPORT_MESSAGE)->SetWindowTextW(L"Stage 1/3 : Import and Tessellation");

	CString strTitle = m_strFilePathName.Right(m_strFilePathName.GetLength() - m_strFilePathName.ReverseFind('\\') - 1);
	SetWindowText(strTitle);

	SetAnchorLayout();

	// ImportExchangeFile();

	return TRUE;
}

void HPSExchangeProgressDialog::SetFilePathName(CString strFilePathName) 
{ 
	m_strFilePathName = strFilePathName; 
}

void HPSExchangeProgressDialog::SetAnchorLayout()
{
	CBCGPStaticLayout* pcLayout = (CBCGPStaticLayout*) GetLayout();
	ASSERT(pcLayout);

	pcLayout->AddAnchor(IDC_PROGRESS_BAR, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pcLayout->AddAnchor(IDC_IMPORT_MESSAGE, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeHorz);
	pcLayout->AddAnchor(IDC_EDIT_MESSAGES, CBCGPStaticLayout::e_MoveTypeNone, CBCGPStaticLayout::e_SizeTypeBoth);

	pcLayout->AddAnchor(IDC_CHECK_KEEP_OPEN, CBCGPStaticLayout::e_MoveTypeVert, CBCGPStaticLayout::e_SizeTypeNone);
	pcLayout->AddAnchor(IDCANCEL, CBCGPStaticLayout::e_MoveTypeBoth, CBCGPStaticLayout::e_SizeTypeNone);
}

void HPSExchangeProgressDialog::DisplayLogMessage(CString strLogMessage)
{
	if(true == strLogMessage.IsEmpty()) {
		return;
	}

	Json::Object cObject;

	wchar_t* pBuffer = (wchar_t*) (const wchar_t*) strLogMessage;
	Json::Generator::ReadObject(pBuffer, cObject);

	Json::Array & cMessageArray = cObject.GetArray("MessageArray");

	CString strEditBoxText;
	m_wndEditBoxCtrl.GetWindowText(strEditBoxText);

	for(int nIndex = 0; nIndex < cMessageArray.GetSize(); nIndex++)
	{
		CString strText;
		strText.Format(L"%d. Reading %s\r\n", ++m_nLogMessageLineCount, cMessageArray[nIndex]->AsString());
		strEditBoxText += strText;
	}

	m_wndEditBoxCtrl.SetWindowText(strEditBoxText);
	m_wndEditBoxCtrl.LineScroll(m_wndEditBoxCtrl.GetLineCount());

	strLogMessage.Empty();
}


void HPSExchangeProgressDialog::OnBnClickedCancel()
{
	if(m_bSuccess) {
		EndDialog(0);
	}
	else {

/*
		m_ptrNotifier.Cancel();

		KillTimer(HPSExchangeProgressDialog::TIMER_ID);
		this->OnCancel();*/
	}
}


void HPSExchangeProgressDialog::OnBnClickedCheckKeepOpen()
{
	CWnd * item = GetDlgItem(IDC_CHECK_KEEP_OPEN);
	CButton * button = static_cast<CButton *>(item);

	int check_status = button->GetCheck();

	if(check_status == BST_CHECKED) {
		m_bKeepDialogOpen = true;
	}
	else if(check_status == BST_UNCHECKED) {
		m_bKeepDialogOpen = false;
	}
}

// Dialog가 Show 상태가 변할때 처리. (Modeless Dialog에서 사용)
// Show 상태가 될때, Import 처리를 시작하도록 한다.
void HPSExchangeProgressDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CBCGPDialog::OnShowWindow(bShow, nStatus);

	if(TRUE == bShow) {
		CString strTitle = m_strFilePathName.Right(m_strFilePathName.GetLength() - m_strFilePathName.ReverseFind('\\') - 1);
		SetWindowText(strTitle);

		ImportExchangeFile();
	}
}

//== Import Process 관련 함수 ========================================================================

// 1. HPS File Interface에 Import를 시작하도록 Message를 전달.
bool HPSExchangeProgressDialog::ImportExchangeFile()
{
	DmiJsonCmd::SetCommandCallback(CommandCallback, (DWORD_PTR *) this);

	DmiJsonCmd::ExecuteHpsCommand((DWORD_PTR) DmiHps::Command::ImportExchangeFile, m_nDocId, m_strFilePathName);

	SetTimer(STATUS_TIMER_ID, 100, nullptr);
	SetTimer(TIMER_ID, 50, nullptr);

	return true;
}

// 2. HPS File Interface에서 전달되는 Message를 받기 위한 Callback 함수
void CALLBACK HPSExchangeProgressDialog::CommandCallback(DWORD_PTR * pcTarget, Json::Object & cObject)
{
	if(nullptr == pcTarget) {
		return;
	}

	HPSExchangeProgressDialog * pcDlg = dynamic_cast<HPSExchangeProgressDialog *>((HPSExchangeProgressDialog *) pcTarget);
	if(nullptr == pcDlg) {
		return;
	}

	pcDlg->ExecuteCommand(cObject);
}

// 3. 전달 받은 Command 처리
bool HPSExchangeProgressDialog::ExecuteCommand(Json::Object & cObject)
{
	DmiHps::Command nCommandId = (DmiHps::Command) cObject.GetDwordPtr("CommandId");

	switch(nCommandId)
	{
		case DmiHps::Command::ReturnImportExchangeNotifierStatus:
			ProcessReceivedImportExchangeNotifierStatus(cObject);
			break;

		case DmiHps::Command::ImportMessage:
		{
			CString strMessage = cObject.GetString("Message");
			GetDlgItem(IDC_IMPORT_MESSAGE)->SetWindowTextW(strMessage);
/*

			int length = _edit_box.GetWindowTextLengthW();
			wchar_t * buffer = new wchar_t[length + 1];
			_edit_box.GetWindowTextW(buffer, length + 1);

			HPS::UTF8 text(buffer);
			delete[] buffer;

			for(auto it = _log_messages.begin(), e = _log_messages.end(); it != e; ++it)
			{
				if(text != HPS::UTF8(""))
					text += HPS::UTF8("\r\n");
				text = text + HPS::UTF8("Reading ") + *it;
			}
			_log_messages.clear();

			HPS::WCharArray wtext;
			text.ToWStr(wtext);
			_edit_box.SetWindowTextW(wtext.data());
			_edit_box.LineScroll(_edit_box.GetLineCount());
*/

		}
		break;

		case DmiHps::Command::ImportExchangeLog:
		{
			Json::Array & cFileLogArray = cObject.GetArray("FileLog");

			if(0 < cFileLogArray.GetSize()) {
				CString strLogText;
				m_wndEditBoxCtrl.GetWindowText(strLogText);

				for(int nIndex = 0; nIndex < cFileLogArray.GetSize(); nIndex++) {
					CString strText = cFileLogArray.GetAt(nIndex)->AsString();

					if(false == strLogText.IsEmpty()) {
						strLogText += L"\r\n";
					}

					strLogText += L"Reading " + strText;
				}

				m_wndEditBoxCtrl.SetWindowText(strLogText);
				m_wndEditBoxCtrl.LineScroll(m_wndEditBoxCtrl.GetLineCount());
			}
		}
		break;
	}

	return true;
}

// 3-1. 전달받은 Notifier Status를 처리하는 함수 
// (Timer에서 Import Notifer Status 요청을 전송하고 그 결과값이 전달되면 처리하는 함수)
void HPSExchangeProgressDialog::ProcessReceivedImportExchangeNotifierStatus(Json::Object & cObject)
{
	DmiHps::IoResult eImportStatus = (DmiHps::IoResult)cObject.GetDwordPtr((DWORD_PTR) DmiHps::Command::Result);

	if(eImportStatus != DmiHps::IoResult::InProgress) {
		KillTimer(HPSExchangeProgressDialog::TIMER_ID);

		if(eImportStatus == DmiHps::IoResult::Success) {
			PerformInitialUpdate();
		}

		EndDialog(0);
	}
}

void HPSExchangeProgressDialog::OnTimer(UINT_PTR nIDEvent)
{
	switch(nIDEvent)
	{
		case TIMER_ID:
			DmiJsonCmd::ExecuteHpsCommand((DWORD_PTR) DmiHps::Command::ImportExchangeLog, m_nDocId);
			break;

			// Log Message
/*
			CString strLogMessage;
			RtJsonCmd::ExcuteHpsCommand(L"GetImportLogMessage", strLogMessage, m_nDocId);

			DisplayLogMessage(strLogMessage);


			if(false == DmiJsonCmd::ExecuteHpsCommand(L"InProgressImportStatus", m_nDocId))
			{
				KillTimer(nIDEvent);
				KillTimer(STATUS_TIMER_ID);

				if(true == DmiJsonCmd::ExecuteHpsCommand(L"IsSuccessImportStatus", m_nDocId)) {
					PerformInitialUpdate();
				}

				if(false == m_bKeepDialogOpen) {
					EndDialog(0);
				}
			}
		}
*/
		

		case STATUS_TIMER_ID:
			DmiJsonCmd::ExecuteHpsCommand((DWORD_PTR) DmiHps::Command::RequestImportExchangeNotifierStatus, m_nDocId);
			break;

		default:
			break;
	}
}

void HPSExchangeProgressDialog::PerformInitialUpdate()
{
	GetDlgItem(IDCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_IMPORT_MESSAGE)->SetWindowText(L"Stage 3/3 : Performing Initial Update");

	DmiJsonCmd::ExecuteHpsCommand((DWORD_PTR) DmiHps::Command::CompleteImportExchangeFile, m_nDocId);

	GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	GetDlgItem(IDCANCEL)->SetWindowTextW(L"Close Dialog");
	m_wndProgressCtrl.SetMarquee(FALSE, 9999);
	m_wndProgressCtrl.SetRange(0, 100);
	m_wndProgressCtrl.SetPos(100);

	GetDlgItem(IDC_IMPORT_MESSAGE)->SetWindowText(L"Import Complete");
	m_wndProgressCtrl.Invalidate();
	m_wndProgressCtrl.UpdateWindow();

	m_bSuccess = true;
}
