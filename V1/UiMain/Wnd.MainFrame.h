#pragma once

#include "Cls.UserIoManager.h"
#include "Ctl.CommandBar.h"
#include "Ctl.PanelBar.h"
#include "Ctl.RibbonBar.h"
#include "Ctl.StatusBar.h"
#include "Ctl.TaskBar.h"
#include "Dlg.Base.h"
#include "Dlg.DebugTracer.h"
#include "Wnd.h"

//--------------------------------------------------------------------------------------------------

class WndMainFrame : public CBCGPMDIFrameWnd
{
public:

	// Components
	CtlRibbonBar RibbonBarCtl;
	CtlStatusBar StatusBarCtl;
	CtlPanelBar PanelBarCtl;
	CtlTaskBar TaskBarCtl;
	ClsUserIoManager UioManager;
	CMenu ContextMenu;
	WndView* ActiveView = nullptr;
	DlgInstances Dialogs;
	std::vector<CString> WaitingFileNames;

public:

	WndMainFrame();

	~WndMainFrame() override {}

public:

	DlgDebugTracer& GetDebugTracer();

	CMenu& GetContextMenu(bool clearFirst);

	CRect GetMDIRect();

	void ShowPanelBar();

	void ShowProgress(bool bShow = true);

	void ShowTaskBar(bool bShow = true);

	void ViewChanged(UINT message, WndView* pView);

	bool HasCommandHandeler(UINT id);

public:

	CBCGPMDIChildWnd* CreateDocumentWindow(LPCTSTR lpcszDocName, CObject* /*pObj*/) override;
	// WARNING - for remove debug message (AppMsg - Warning: GetWindowMenuPopup failed!)
	HMENU GetWindowMenuPopup(HMENU hMenuBar) override;

	//BOOL OnDrawMenuImage(CDC* pDC, const CBCGPToolbarMenuButton* pMenuButton, const CRect& rectImage) override;

	BOOL OnEraseMDIClientBackground(CDC* pDC) override;

	void OnSizeMDIClient(const CRect& rectOld, const CRect& rectNew) override;

	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

#ifdef _DEBUG
	void AssertValid() const override;

	void Dump(CDumpContext& dc) const override;
#endif

	afx_msg LRESULT OnSignal2d(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnSignal3d(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnNextFileOpen(WPARAM wp, LPARAM lp);

	afx_msg void OnClose();

	afx_msg void OnCommand(UINT id);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnDropFiles(HDROP hDropInfo);

	afx_msg void OnFileOpen();

	afx_msg void OnAppOptions();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_DYNAMIC(WndMainFrame)
	DECLARE_MESSAGE_MAP()

public:

	bool HasNextFile();

	void OpenNextFile();
};
