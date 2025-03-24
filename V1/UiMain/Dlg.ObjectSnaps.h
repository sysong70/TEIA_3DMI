#pragma once

#include "Ctl.CheckListBox.h"
#include "Dlg.Standard.h"

//--------------------------------------------------------------------------------------------------

class DlgObjectSnaps : public DlgStandard
{
public:

	CtlCheckListBox ListBoxCtl;
	CBCGPButton OkCtl;
	CBCGPButton CancelCtl;

public:

	DlgObjectSnaps()
		: DlgStandard(IDD_DMI_DROPDOWN, "ObjectSnaps", nullptr)
	{}

	~DlgObjectSnaps() override {}

public:

	void OnCancel() override { __super::OnCancel(); }

	BOOL OnInitDialog() override;
	//:TODO - Sand data to connector
	void OnOK() override { __super::OnOK(); }

	DECLARE_DYNCREATE(DlgObjectSnaps);
	DECLARE_MESSAGE_MAP()

public:

	void ConstructBody(const CRect& boundary) override;

	void ConstructFooter(const CRect& boundary) override;
};
