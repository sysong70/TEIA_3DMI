#pragma once

#include "Dlg.Standard.h"

//--------------------------------------------------------------------------------------------------

class DlgFolders : public DlgStandard
{
public:

public:

	class EditListBox : public CBCGPEditListBox
	{
	public:

		EditListBox();

	public:

		void OnBrowse() override;

		void OnClickButton(int iButton) override;

		void OnSelectionChanged() override;
	};

	// Body controls
	EditListBox LixtBoxCtl;
	// Setting before OnInitDialog
	CString Value;
	// Footer controls
	CBCGPButton OkCtl;
	CBCGPButton CancelCtl;

public:

	DlgFolders()
		: DlgStandard(IDD_DMI_STANDARD, "Folders", nullptr)
	{}

	~DlgFolders() override {}

	CString& GetValue() { return Value; }

	void SetValue(CString value) { Value = value; }

public:

	void OnCancel() override { __super::OnCancel(); }

	BOOL OnInitDialog() override;

	void OnOK() override;

	DECLARE_MESSAGE_MAP()

public:

	void ConstructBody(const CRect& boundary) override;

	void ConstructFooter(const CRect& boundary) override;
};
