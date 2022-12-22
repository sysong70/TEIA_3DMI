#pragma once

// BackStagePageInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBackStagePageInfo dialog

class CBackStagePageInfo : public CBCGPDialog
{
	DECLARE_DYNCREATE(CBackStagePageInfo)

// Construction
public:
	CBackStagePageInfo(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORM_INFO };
#endif
	CBCGPStatic	m_wndPath;
	CBCGPStatic	m_wndInfo;
	CBCGPStatic	m_wndDocName;
	CBCGPButton	m_btnPreview;
	CString	m_strDocName;
	CString	m_strPath;

// Overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPreview();
	DECLARE_MESSAGE_MAP()

	CFont	m_fontCaption;

	void PreparePreviewBitmap();
};
