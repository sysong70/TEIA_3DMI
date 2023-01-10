#pragma once

#include "Dialog.Standard.h"
#include "Component.TabWnd.h"
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

		void OnCancel() override;

		BOOL OnInitDialog() override;

		void OnOK() override;

		afx_msg LRESULT OnChangeActiveTab(WPARAM wp, LPARAM lp);

		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

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

		Component::TabWnd m_tabs;
		Component::TreePropList m_preferencesUi;
		Component::TreePropList m_fileOptionsUi;

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
