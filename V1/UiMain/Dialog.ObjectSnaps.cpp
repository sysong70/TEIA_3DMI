#include "stdafx.h"
#include "resource.h"
#include "Dialog.ObjectSnaps.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetObjectSnap

namespace PresetObjectSnap
{
	const UINT Id = WM_USER;

	CSize ImageSize() {
		return globalUtils.ScaleByDPI(CSize(24, 24));
	}

	int ExtraHeight() {
		return globalUtils.ScaleByDPI(6);
	}
}



using namespace Dialog;

IMPLEMENT_DYNCREATE(ObjectSnaps, Standard)

BEGIN_MESSAGE_MAP(ObjectSnaps, Standard)
END_MESSAGE_MAP()



Dialog::ObjectSnaps::ObjectSnaps()
	: Standard(IDD_DMI_DROPDOWN, "ObjectSnaps", nullptr)
{
}



Dialog::ObjectSnaps::~ObjectSnaps()
{
}



void Dialog::ObjectSnaps::OnCancel()
{
	__super::OnCancel();
}



BOOL Dialog::ObjectSnaps::OnInitDialog()
{
	__super::OnInitDialog();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { frame.cx, frame.cy, size.cx, size.cy };

	ConstructFooter(body);
	body.bottom -= m_nFooterHeight;
	ConstructBody(body);

	size -= frame;
	m_windowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	return TRUE;
}



void Dialog::ObjectSnaps::OnOK()
{
	//:TODO - sand data to connector

	__super::OnOK();
}



void Dialog::ObjectSnaps::ConstructBody(const CRect& boundary)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL |
		LBS_HASSTRINGS | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED;
	if (m_list.Create(dwStyle, boundary, this, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	m_list.SetAlternateRowColor();
	m_list.SetItemExtraHeight(PRESET::ExtraHeight());

	m_list.AddItems({
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
	}, PRESET::ImageSize());
}



void Dialog::ObjectSnaps::ConstructFooter(const CRect& boundary)
{
	Json::Object& buttons = GetDefaultButtons();

	int maxHeight = 0;

	maxHeight = max(maxHeight, SetupControl(m_wndOk, buttons.GetAt("Ok")).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndCancel, buttons.GetAt("Cancel")).cy);

	m_nFooterHeight = maxHeight + FooterPadding();

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	AlignControls({ &m_wndCancel, &m_wndOk }, basePoint, Component::EAlign::VerticalCenter);
	basePoint.x = boundary.right;
	DestributeControls({ &m_wndCancel, &m_wndOk }, basePoint, FooterPadding(), Component::EDirection::ToLeft);
}

#undef PRESET
