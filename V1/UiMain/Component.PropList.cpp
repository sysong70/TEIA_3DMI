#include "stdafx.h"
#include "Component.PropList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetPropList

namespace PresetPropList
{
	int const Id = WM_USER;
}



Component::PropList::PropList()
{
}



Component::PropList::~PropList()
{
}



void Component::PropList::Initialize(CWnd* pParentWnd)
{
	if (Create(WS_VISIBLE | WS_CHILD, {}, pParentWnd, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	EnableToolBar();
	EnableSearchBox();
	EnableHeaderCtrl(FALSE);
	EnableDescriptionArea();
	EnableContextMenu();

	MarkModifiedProperties();
	SetVSDotNetLook();
	SetGroupNameFullWidth();

	SetRowPadding(PropListRowPadding());
}
