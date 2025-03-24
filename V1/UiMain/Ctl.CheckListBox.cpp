#include "stdafx.h"

#include "Ast.h"
#include "Ctl.CheckListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlCheckListBox, CBCGPListBox)
END_MESSAGE_MAP()



CtlCheckListBox::CtlCheckListBox()
{
	m_bVisualManagerStyle = TRUE;
}



void CtlCheckListBox::AddItem(UINT id)
{
	CString title, desc;
	Ast::GetResource(id, title, desc);
	int index = AddString(title);
	SetItemDescription(index, desc);

	CBCGPSVGImage* pImage = new CBCGPSVGImage();
	BOOL success = pImage->Load(id);
	m_ImageList.AddSVG(pImage);

	ASSERT(success);
	ASSERT(GetCount() == m_ImageList.GetCount());

	SetItemImage(index, index);
}



void CtlCheckListBox::AddItems(const Ctl::ResourceIds& ids, CSize imageSize)
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



void CtlCheckListBox::SetImageList(const Ctl::ResourceIds& ids, CSize imageSize)
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



void CtlCheckListBox::OnClickItem(int index)
{
	int check = GetCheck(index);
	SetCheck(index, check == BST_CHECKED ? BST_UNCHECKED : BST_CHECKED);
}
