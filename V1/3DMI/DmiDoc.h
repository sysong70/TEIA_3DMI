#pragma once

class CMainFrame;
class DmiView;

#include <chrono>

#include "ModelBrowser/ModelTree.h"

class HPSExchangeProgressDialog;

class DmiDoc : public CDocument
{
protected: // create from serialization only
	DmiDoc();
	DECLARE_DYNCREATE(DmiDoc)

// Attributes
public:

// Operations
public:

// Overrides
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

	DWORD_PTR GetId() { return m_nId; }

	ModelTreeCtrl * GetModelTreeCtrl() { return m_pcModelTreeCtrl; }

// Implementation
public:
	virtual ~DmiDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	DmiView * GetView();

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
	DWORD_PTR m_nId;

	virtual void OnChangedViewList();
public:
	afx_msg void OnFileSaveAs();

protected:
	CMainFrame * GetMainFrame() { return (CMainFrame *)AfxGetMainWnd(); }

	// ----- Error Message 관련 함수 -----
public:
	void SetTimeTick(int nIndex, LONGLONG nTimeTick) { m_nTimeTick[nIndex] = nTimeTick; }
	void UpdatePropertyBarInformation();
	void UpdateImportExchangePropertyBarInformation();
	void SetHpsErrMsgArray(CStringArray & astrHpsErrMsgArray) { m_astrHpsErrMsgArray.Copy(astrHpsErrMsgArray); }

	//== 선택 항목 처리 ==============================================================================
	void SetSelectedItemInformation(CString strItemInfo) { m_strSelectItemInfo = strItemInfo; }

	//== Model Tree Function =======================================================================
	void CreateModelTreeInformation();
	void CreateModelTree(Json::Object & cInObject);

	void UpdateModelTreeInformation();

private:
	CStringArray m_astrHpsErrMsgArray;
	CString m_strSelectItemInfo;
	CString m_strFilePathName;
	LONGLONG m_nTimeTick[4];
	CString GetTimeSpanString(std::chrono::milliseconds & cTimeSpan);

	//HPSExchangeProgressDialog * m_pcExchangeProgressDialog = nullptr;

	ModelTreeCtrl * m_pcModelTreeCtrl = nullptr;
};


