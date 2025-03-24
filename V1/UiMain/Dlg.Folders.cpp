#include "stdafx.h"

#include "Ctl.h"
#include "Dlg.Folders.h"
#include "Wnd.Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region EditListBox Control

DlgFolders::EditListBox::EditListBox()
{
	m_bVisualManagerStyle = TRUE;

	EnableBrowseButton(TRUE);
	SetGrayDisabledButtons(TRUE);
}



void DlgFolders::EditListBox::OnBrowse()
{
	int index = GetSelItem();

	CString folder = GetItemText(index);
	if (TheApp.GetShellManager()->BrowseForFolder(folder, NULL, folder) == FALSE) {
		return;
	}

	if (index == GetCount()) {
		SelectItem(AddItem(folder));
	}
	else {
		SetItemText(index, folder);
	}
}



void DlgFolders::EditListBox::OnClickButton(int iButton)
{
	// TODO
	UINT id = GetButtonID(iButton);

	CBCGPEditListBox::OnClickButton(iButton);
}



void DlgFolders::EditListBox::OnSelectionChanged()
{
	CBCGPEditListBox::OnSelectionChanged();

	// TODO
}

#pragma endregion // REGION

//**************************************************************************************************

BEGIN_MESSAGE_MAP(DlgFolders, DlgStandard)
END_MESSAGE_MAP()



BOOL DlgFolders::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructFooter(body);
	body.bottom -= FooterHeight;
	ConstructBody(body);

	WindowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void DlgFolders::OnOK()
{
	// Reset value

	WStringArray directories;
	for (int i = 0; i < LixtBoxCtl.GetCount(); i++) {
		directories.push_back(LixtBoxCtl.GetItemText(i));
	}

	Value = WStr::Join(directories, L';');

	__super::OnOK();
}



void DlgFolders::ConstructBody(const CRect& boundary)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (LixtBoxCtl.Create(NULL, dwStyle, boundary, this, WM_USER) == false) {
		DEBUG_RETURN;
	}

	LixtBoxCtl.SetStandardButtons();

	// Add Items

	if (Value.IsEmpty() == false) {
		WStringArray directories;
		WStr::Split(Value.GetBuffer(), L';', directories);

		for (auto& dir : directories) {
			LixtBoxCtl.AddItem(dir);
		}
	}
}



void DlgFolders::ConstructFooter(const CRect& boundary)
{
	Json::Object& buttons = GetDefaultButtons();
	CSize margin = Ctl::Gap();
	CSize size;
	int maxHeight = 0;

	size = Ctl::Setup(OkCtl, buttons.GetAt("Ok"), this); maxHeight = max(maxHeight, size.cy);
	size = Ctl::Setup(CancelCtl, buttons.GetAt("Cancel"), this); maxHeight = max(maxHeight, size.cy);

	FooterHeight = maxHeight + margin.cy;

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	Ctl::Align({ &CancelCtl, &OkCtl }, basePoint, Ctl::EAlign::VerticalCenter, this);
	basePoint.x = boundary.right;
	Ctl::Destribute({ &CancelCtl, &OkCtl }, basePoint, margin.cx, Ctl::EDirection::ToLeft, this);
}
