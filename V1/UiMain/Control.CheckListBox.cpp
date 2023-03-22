#include "stdafx.h"
#include "Control.CheckListBox.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetListBox

namespace PresetListBox
{
}



using namespace Control;

BEGIN_MESSAGE_MAP(CheckListBox, CBCGPListBox)
END_MESSAGE_MAP()



Control::CheckListBox::CheckListBox()
{
	m_bVisualManagerStyle = TRUE;
}



void Control::CheckListBox::AddItem(UINT resourceId)
{
	CString title, desc;
	Facility::GetResource(resourceId, title, desc);
	int index = AddString(title);
	SetItemDescription(index, desc);

	CBCGPSVGImage* pImage = new CBCGPSVGImage();
	BOOL success = pImage->Load(resourceId);
	m_ImageList.AddSVG(pImage);

	ASSERT(success);
	ASSERT(GetCount() == m_ImageList.GetCount());

	SetItemImage(index, index);
}



void Control::CheckListBox::AddItems(std::vector<UINT> ids, CSize imageSize)
{
	m_hImageList = NULL;
	m_ImageList.Clear();
	m_ImageListDisabled.Clear();
	m_sizeImage = imageSize;

	m_ImageList.SetImageSize(m_sizeImage);

	for (auto id : ids) {
		AddItem(id);
	}
}



void Control::CheckListBox::SetImageList(std::vector<UINT> ids, CSize imageSize)
{
	m_hImageList = NULL;
	m_ImageList.Clear();
	m_ImageListDisabled.Clear();
	m_sizeImage = imageSize;

	m_ImageList.SetImageSize(m_sizeImage);

	for (auto id : ids) {
		CBCGPSVGImage* pImage = new CBCGPSVGImage();
		BOOL success = pImage->Load(id);
		ASSERT(success);
		m_ImageList.AddSVG(pImage);
	}

	m_ImageList.CopyTo(m_ImageListDisabled);
	m_ImageListDisabled.ConvertToGrayScale();
}



void Control::CheckListBox::OnClickItem(int index)
{
	int check = GetCheck(index);
	SetCheck(index, check == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);
}

#undef PRESET
