#pragma once

#include "Window.h"
#include "Component.CommandBar.h"
#include "Component.PanelBar.h"
#include "Component.RibbonBar.h"
#include "Component.StatusBar.h"
#include "Component.TaskBar.h"
#include "Dialog.h"
#include "Dialog.DebugTracer.h"

//--------------------------------------------------------------------------------------------------

namespace Window
{
	class MainFrame : public CBCGPMDIFrameWnd
	{
		DECLARE_DYNAMIC(MainFrame)

	public:

		friend class Document;
		friend class View;
		friend class View3d;
		friend class View2d;
		friend class TaskBar;

		MainFrame();

		~MainFrame() override;

	public:

		Dialog::DebugTracer& GetDebugTracer();

		Window::View* GetActiveView();

		CMenu& GetContextMenu(bool clearFirst);

		CRect GetMDIRect();

		Component::CommandBar& GetCommandBar();

		CBCGPEdit& GetCommandBarInput();

		Component::PanelBar& GetPanelBar();

		Component::RibbonBar& GetRibbonBar();

		Component::TaskBar& GetTaskBar();

		void ReceiveSignal(Json::Object* pData);

		void ShowPanelBar();

		void ShowProgress(bool bShow = true);

		void ShowTaskBar(bool bShow = true);

		void ViewChanged(UINT message, View* pView);

		bool HasCommandHandeler(UINT id);

	protected:

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

		afx_msg LRESULT OnSignal(WPARAM wp, LPARAM lp);

		afx_msg LRESULT OnNextFileOpen(WPARAM wp, LPARAM lp);

		afx_msg void OnClose();

		afx_msg void OnCommand(UINT id);

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		afx_msg void OnDropFiles(HDROP hDropInfo);

		afx_msg void OnFileOpen();

		afx_msg void OnAppOptions();

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	private: // Components

		Component::RibbonBar m_ribbonBar;
		Component::StatusBar m_statusBar;
		Component::PanelBar m_panelBar;
		Component::TaskBar m_taskBar;

	private: // Command prompt and Menu

		CMenu m_contextMenu;
		Component::CommandBar m_commandBar;

	private: // Windows

		Window::View* m_pActiveView = nullptr;
		Dialog::Instances m_dialogs;

	private:

		std::vector<CString> m_fileNames;

		bool HasNextFile();

		void OpenNextFile();
	};
}
