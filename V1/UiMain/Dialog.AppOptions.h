#pragma once

#include "Dialog.Standard.h"
#include "Control.TreePropList.h"
#include "Control.TabWnd.h"



namespace Dialog
{
	class AppOptions : public Standard
	{
	public:

		AppOptions();

		~AppOptions() override;

	protected:

		void DoDataExchange(CDataExchange* pDX) override;

		void OnCancel() override;

		BOOL OnInitDialog() override;

		void OnOK() override;

		afx_msg LRESULT OnChangeActiveTab(WPARAM wp, LPARAM lp);

		afx_msg void OnApply();

		afx_msg void OnInitialize();

		afx_msg void OnReset();

		DECLARE_MESSAGE_MAP()

	protected:

		void ConstructBody(const CRect& boundary) override;

		void ConstructFooter(const CRect& boundary) override;

	private:

		void CreateTabs();

	private: // Body controls

		Control::TabWnd m_tabs;
		Control::TreePropList m_preferencesUi;
		Control::TreePropList m_fileOptionsUi;

	private: // Footer controls

		CBCGPButton m_wndInitialize;
		CBCGPButton m_wndReset;
		CBCGPButton m_wndOk;
		CBCGPButton m_wndApply;
		CBCGPButton m_wndCancel;

	private: // Data set

		struct DataSet
		{
			Json::Object Design;
			Json::Object Data;
			Json::Object DataResource;
			Json::Object DataLocal;
		};

		DataSet m_preferences;
		DataSet m_fileOptions;
	};
}
