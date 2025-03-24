#pragma once

#include "Ctl.TabWnd.h"
#include "Ctl.TreePropList.h"
#include "Dlg.Standard.h"

//--------------------------------------------------------------------------------------------------

class DlgAppOptions : public DlgStandard
{
public:
	
	// Body controls
	CtlTabWnd TabsCtl;
	CtlTreePropList PreferencesCtl;
	CtlTreePropList FileOptionsCtl;
	// Footer controls
	CBCGPButton InitializeCtl;
	CBCGPButton ResetCtl;
	CBCGPButton OkCtl;
	CBCGPButton ApplyCtl;
	CBCGPButton CancelCtl;

	struct DataSet
	{
		Json::Object Design;
		// Current
		Json::Object Value;
		// From resource
		Json::Object ValueResource;
		// Previous
		Json::Object ValueLocal;
	};

	DataSet PreferencesSet;
	DataSet FileOptionsSet;

public:

	DlgAppOptions()
		: DlgStandard(IDD_DMI_STANDARD, "AppOptions", nullptr)
	{}

	~DlgAppOptions() override {}

public:

	void OnCancel() override { __super::OnCancel(); }

	BOOL OnInitDialog() override;

	void OnOK() override;

	afx_msg LRESULT OnChangedFileOption(WPARAM wp, LPARAM lp);

	afx_msg LRESULT OnChangeActiveTab(WPARAM wp, LPARAM lp);

	afx_msg void OnApply();

	afx_msg void OnInitialize();

	afx_msg void OnReset();

	DECLARE_MESSAGE_MAP()

public:

	void ConstructBody(const CRect& boundary) override;

	void ConstructFooter(const CRect& boundary) override;
};
