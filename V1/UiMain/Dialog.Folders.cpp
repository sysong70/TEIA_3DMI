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

#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::x, m_wnd##x);

#define PRESET PresetAppOptions

namespace PresetAppOptions
{
	enum EControlId
	{
		Id = WM_USER,
		PreferenceProp,
		FileProp,
		Initialize,
		Reset,
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



void Dialog::Folders::OnCancel()
{
	__super::OnCancel();
}



BOOL Dialog::Folders::OnInitDialog()
{
	__super::OnInitDialog();

	return TRUE;
}



void Dialog::Folders::OnOK()
{
	//:TODO

	__super::OnOK();
}



void Dialog::Folders::ConstructBody(const CRect& boundary)
{
}



void Dialog::Folders::ConstructFooter(const CRect& boundary)
{
}

#undef DDX_CONTROL
#undef PRESET
