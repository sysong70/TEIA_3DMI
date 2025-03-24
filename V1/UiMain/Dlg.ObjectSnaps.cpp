#include "stdafx.h"

#include "Dlg.ObjectSnaps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

IMPLEMENT_DYNCREATE(DlgObjectSnaps, DlgStandard)

BEGIN_MESSAGE_MAP(DlgObjectSnaps, DlgStandard)
END_MESSAGE_MAP()



BOOL DlgObjectSnaps::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { frame.cx, frame.cy, size.cx, size.cy };

	ConstructFooter(body);
	body.bottom -= FooterHeight;
	ConstructBody(body);

	size -= frame;
	WindowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void DlgObjectSnaps::ConstructBody(const CRect& boundary)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
		LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED;
	if (ListBoxCtl.Create(dwStyle, boundary, this, WM_USER) == FALSE) {
		DEBUG_RETURN;
	}

	const int extraHeight = globalUtils.ScaleByDPI(6);

	ListBoxCtl.SetAlternateRowColor();
	ListBoxCtl.SetItemExtraHeight(extraHeight);

	ListBoxCtl.AddItems({
		HOME_3D_CMD_ObjectSnap_Point,
		HOME_3D_CMD_ObjectSnap_End,
		HOME_3D_CMD_ObjectSnap_Mid,
		HOME_3D_CMD_ObjectSnap_Intersection,
		HOME_3D_CMD_ObjectSnap_Perpendicular,
		HOME_3D_CMD_ObjectSnap_Center,
		HOME_3D_CMD_ObjectSnap_Quadrant,
		HOME_3D_CMD_ObjectSnap_Near,
		HOME_3D_CMD_ObjectSnap_OnSurface,
		HOME_3D_CMD_ObjectSnap_BoundaryCenter,
		HOME_3D_CMD_ObjectSnap_Axis,
	}, Ctl::ImageSize());
}



void DlgObjectSnaps::ConstructFooter(const CRect& boundary)
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
