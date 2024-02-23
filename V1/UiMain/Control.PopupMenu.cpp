#include "stdafx.h"
#include "Control.PopupMenu.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

using namespace Control;

BEGIN_MESSAGE_MAP(PopupMenu, CBCGPListBox)
END_MESSAGE_MAP()



Control::PopupMenu::PopupMenu()
{
	m_bVisualManagerStyle = TRUE;

	m_hImageList = NULL;
	m_ImageList.Clear();
	m_ImageListDisabled.Clear();
}



void Control::PopupMenu::AddItem(UINT id)
{
	if (id == 0) {
		AddSeparator();
		return;
	}

	CString title, desc;
	Facility::GetResource(id, title, desc);
	int index = AddString(title);
	SetItemDescription(index, desc);

	CBCGPSVGImage* pImage = new CBCGPSVGImage();
	BOOL success = pImage->Load(id);
	m_ImageList.AddSVG(pImage);

	ASSERT(success);
	ASSERT(GetCount() == m_ImageList.GetCount());

	SetItemImage(index, index);
}



void Control::PopupMenu::AddItems(const ResourceIds& ids, CSize imageSize)
{
	m_sizeImage = imageSize;

	for (auto id : ids) {
		AddItem(id);
	}
}



void Control::PopupMenu::SetCheckItems(const ResourceIds& ids)
{
}



void Control::PopupMenu::OnClickItem(int index)
{
	//:TODO - user color bar
}
