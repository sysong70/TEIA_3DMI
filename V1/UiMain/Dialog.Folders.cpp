#include "stdafx.h"
#include "Dialog.Folders.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//--------------------------------------------------------------------------------------------------

#pragma region EditListBox Control

//--------------------------------------------------------------------------------------------------

Dialog::Folders::EditListBox::EditListBox()
{
	//m_bVisualManagerStyle(TRUE)
	SetGrayDisabledButtons(TRUE);
	EnableBrowseButton(TRUE);
}



void Dialog::Folders::EditListBox::OnBrowse()
{
}



void Dialog::Folders::EditListBox::OnClickButton(int iButton)
{
}



void Dialog::Folders::EditListBox::OnSelectionChanged()
{
}

#pragma endregion //:REGION

//--------------------------------------------------------------------------------------------------

#define PRESET PresetAppOptions

namespace PresetAppOptions
{
	enum EControlId
	{
		Id = WM_USER,
		ListBox,
	};
}



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



Json::Object* Dialog::Folders::GetResult()
{
	// create Json::Object, and return
	return nullptr;
}



//void Dialog::Folders::DoDataExchange(CDataExchange* pDX)
//{
//	CBCGPDialog::DoDataExchange(pDX);
//
//	DDX_Control(pDX, PRESET::ListBox, m_listBox);
//}



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
	//:TODO

	__super::OnOK();
}



void Dialog::Folders::ConstructBody(const CRect& boundary)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE;
	if (m_listBox.Create(NULL, dwStyle, boundary, this, PRESET::Id) == false) {
		DEBUG_RETURN;
	}

	//:TEST
	m_listBox.AddItem(_T("Item 1"));
	m_listBox.AddItem(_T("Item 2"));
	m_listBox.AddItem(_T("Item 3"));
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
