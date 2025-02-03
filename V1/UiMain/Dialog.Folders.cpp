#include "stdafx.h"
#include "Dialog.Folders.h"
#include "Window.Application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#pragma region EditListBox Control

Dialog::Folders::EditListBox::EditListBox()
{
	m_bVisualManagerStyle = TRUE;

	EnableBrowseButton(TRUE);
	SetGrayDisabledButtons(TRUE);
}



void Dialog::Folders::EditListBox::OnBrowse()
{
	int index = GetSelItem();

	CString folder = GetItemText(index);
	if (TheApplication.GetShellManager()->BrowseForFolder(folder, NULL, folder) == FALSE) {
		return;
	}

	if (index == GetCount()) {
		SelectItem(AddItem(folder));
	}
	else {
		SetItemText(index, folder);
	}
}



void Dialog::Folders::EditListBox::OnClickButton(int iButton)
{
	// TODO
	UINT id = GetButtonID(iButton);

	CBCGPEditListBox::OnClickButton(iButton);
}



void Dialog::Folders::EditListBox::OnSelectionChanged()
{
	CBCGPEditListBox::OnSelectionChanged();

	// TODO
}

#pragma endregion // REGION

//**************************************************************************************************

#define PRESET PresetAppOptions

namespace PresetAppOptions
{
	enum EControlId
	{
		Id = WM_USER,
		ListBox,
	};
}

//**************************************************************************************************

using namespace Dialog;

BEGIN_MESSAGE_MAP(Folders, Standard)
END_MESSAGE_MAP()



Dialog::Folders::Folders()
	: Standard(IDD_DMI_STANDARD, "Folders", nullptr)
{
}



Dialog::Folders::~Folders()
{
}



CString& Dialog::Folders::GetValue()
{
	return m_value;
}



void Dialog::Folders::SetValue(CString value)
{
	m_value = value;
}



void Dialog::Folders::OnCancel()
{
	__super::OnCancel();
}



BOOL Dialog::Folders::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructFooter(body);
	body.bottom -= m_nFooterHeight;
	ConstructBody(body);

	m_windowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void Dialog::Folders::OnOK()
{
	// Reset value

	WStringArray directories;
	for (int i = 0; i < m_listBox.GetCount(); i++) {
		directories.push_back(m_listBox.GetItemText(i));
	}

	m_value = WStr::Join(directories, L';');

	__super::OnOK();
}



void Dialog::Folders::ConstructBody(const CRect& boundary)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (m_listBox.Create(NULL, dwStyle, boundary, this, PRESET::Id) == false) {
		DEBUG_RETURN;
	}

	m_listBox.SetStandardButtons();

	// Add Items

	if (m_value.IsEmpty() == false) {
		WStringArray directories;
		WStr::Split(m_value.GetBuffer(), L';', directories);

		for (auto& dir : directories) {
			m_listBox.AddItem(dir);
		}
	}
}



void Dialog::Folders::ConstructFooter(const CRect& boundary)
{
	Json::Object& buttons = GetDefaultButtons();
	CSize margin = Control::Gap();
	CSize size;
	int maxHeight = 0;

	size = Control::Setup(m_wndOk, buttons.GetAt("Ok"), this); maxHeight = max(maxHeight, size.cy);
	size = Control::Setup(m_wndCancel, buttons.GetAt("Cancel"), this); maxHeight = max(maxHeight, size.cy);

	m_nFooterHeight = maxHeight + margin.cy;

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	Control::Align({ &m_wndCancel, &m_wndOk }, basePoint, Control::EAlign::VerticalCenter, this);
	basePoint.x = boundary.right;
	Control::Destribute({ &m_wndCancel, &m_wndOk }, basePoint, margin.cx, Control::EDirection::ToLeft, this);
}

#undef PRESET
