#include "stdafx.h"

#include "Ast.h"
#include "Ctl.PopupMenu.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlPopupMenu, CBCGPListBox)
END_MESSAGE_MAP()



CtlPopupMenu::CtlPopupMenu()
{
	m_bVisualManagerStyle = TRUE;

	m_hImageList = NULL;
	m_ImageList.Clear();
	m_ImageListDisabled.Clear();
}



void CtlPopupMenu::AddItem(UINT id)
{
	if (id == 0) {
		AddSeparator();
		return;
	}

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



void CtlPopupMenu::AddItems(const Ctl::ResourceIds& ids, CSize imageSize)
{
	m_sizeImage = imageSize;

	for (auto id : ids) {
		AddItem(id);
	}
}
