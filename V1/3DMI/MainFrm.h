// MainFrm.h : interface of the CMainFrame class
//


#pragma once
#include "ModelBrowser/ModelBrowserBar.h"
#include "WorkSpaceBar2.h"
#include "OutputBar.h"
#include "PropertiesViewBar.h"

class DmiDoc;

class CMainFrame : public CBCGPMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:

	CBitmap& GetAppPreview() { return m_bmpAppPreview; }

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	CBCGPPropBar & GetPropBar() { return m_wndPropGrid; }
	ModelBrowserBar & GetModelBrowserBar() { return m_wndModelBrowser; }

protected:  // control bar embedded members
	CBCGPRibbonStatusBar	m_wndStatusBar;
	CBCGPRibbonBar			m_wndRibbonBar;
	CBCGPToolBarImages		m_RibbonDisplayOptionsIcons;
	CBCGPMenuBar			m_wndMenuBar;
	CBCGPToolBar			m_wndToolBar;
	ModelBrowserBar			m_wndModelBrowser;
	CWorkSpaceBar2			m_wndWorkSpace2;
	COutputBar				m_wndOutput;
	CBCGPPropBar			m_wndPropGrid;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnMdiMoveToNextGroup();
	afx_msg void OnMdiMoveToPrevGroup();
	afx_msg void OnMdiNewHorzTabGroup();
	afx_msg void OnMdiNewVertGroup();
	afx_msg void OnMdiCancel();
	afx_msg void OnClose();
	afx_msg void OnToolsOptions();
	afx_msg void OnRibbonDisplayOptions(UINT nCmd);
	afx_msg void OnUpdateRibbonDisplayOptions(CCmdUI * pCmdUI);
	afx_msg void OnToolsManager();
	afx_msg void OnViewWorkspace();
	afx_msg void OnUpdateViewWorkspace(CCmdUI* pCmdUI);
	afx_msg void OnViewWorkspace2();
	afx_msg void OnUpdateViewWorkspace2(CCmdUI* pCmdUI);
	afx_msg void OnViewOutput();
	afx_msg void OnUpdateViewOutput(CCmdUI* pCmdUI);
	afx_msg void OnViewPropGrid();
	afx_msg void OnUpdateViewPropGrid(CCmdUI* pCmdUI);

	afx_msg LRESULT OnRibbonCustomize(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnBeforeShowRibbonBackstageView(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnMsgFileImportInformation(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnMsgFileImportExchangeInformation(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMsgSelectedItemInformation(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

	virtual BOOL OnShowMDITabContextMenu(CPoint point, DWORD dwAllowedItems, BOOL bDrop);
	virtual CBCGPMDIChildWnd* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/);
	BOOL CreateRibbonBar();
	void ShowOptions(int nPage);

	CBitmap m_bmpAppPreview;
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnDropFiles(HDROP hDropInfo);

protected:
	DmiDoc * GetDoc(DWORD_PTR nId);
};
