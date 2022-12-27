#pragma once

#include "Dialog.Standard.h"
#include "Component.TreePropList.h"



namespace Dialog
{
	class AppSettings : public Standard
	{
	public:

		AppSettings();

		~AppSettings() override;

	protected:

		void DoDataExchange(CDataExchange* pDX) override;

		BOOL OnInitDialog() override;

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

		DECLARE_MESSAGE_MAP()

	protected:

		CRect ConstructBody(const CRect& boundary) override;

		CRect ConstructFooter(const CRect& boundary) override;

	private:

		void CreateTabs();

	private: // Body controls

		CBCGPTabWnd m_wndTabs;

		Component::TreePropList m_PreferenceProp;
		Component::TreePropList m_fileProp;

	private: // Footer controls

		CBCGPButton m_wndInitialize;
		CBCGPButton m_wndReset;
		CBCGPButton m_wndOk;
		CBCGPButton m_wndApply;
		CBCGPButton m_wndCancel;
	};
}
