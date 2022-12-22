#pragma once

#include "afxwin.h"
#include "afxdialogex.h"
#include "afxcmn.h"

#include "resource.h"

#include <Json.h>

class HPSExchangeProgressDialog : public CBCGPDialog
{
	DECLARE_DYNAMIC(HPSExchangeProgressDialog)

public:
	HPSExchangeProgressDialog(DWORD_PTR nDocId);
	//HPSExchangeProgressDialog(int nDocId, int nViewId, CString strTitle);
	virtual ~HPSExchangeProgressDialog();

	enum 
	{ 
		IDD				= IDD_EXCHANGE_IMPORT_DIALOG,
		TIMER_ID		= 1236,
		STATUS_TIMER_ID	= 1237,
	};

	void SetMessage(CString in_message) { m_strStatusMessage = in_message; }
	bool WasImportSuccessful() { return m_bSuccess; }

	void SetFilePathName(CString strFilePathName);

	//== Import Process 관련 함수 ====================================================================
protected:
	bool ImportExchangeFile();
	static void CALLBACK CommandCallback(DWORD_PTR * pcTarget, Json::Object & cObject);
	bool ExecuteCommand(Json::Object & cObject);
	void ProcessReceivedImportExchangeNotifierStatus(Json::Object & cObject);

protected:
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	BOOL OnInitDialog() override;

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheckKeepOpen();

	DECLARE_MESSAGE_MAP()

private:
	CEdit						m_wndEditBoxCtrl;
	CProgressCtrl				m_wndProgressCtrl;
	DWORD_PTR m_nDocId;
	int							m_nViewId;
	int m_nLogMessageLineCount;
	bool						m_bKeepDialogOpen;
	bool						m_bSuccess;
	CString						m_strStatusMessage;
	CString						m_strFilePathName;

	void						PerformInitialUpdate();

	void DisplayLogMessage(CString strLogMessage);
	void SetAnchorLayout();

public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
