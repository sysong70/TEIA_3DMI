#include "stdafx.h"
#include "Dialog.AppSettings.h"
#include "Component.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::EControlId::x, m_wnd##x);

#define PRESET PresetAppSettings

namespace PresetAppSettings
{
	enum class EControlId
	{
		Id = WM_USER,
		Initialize,
		Reset,
	};
}



using namespace Dialog;

BEGIN_MESSAGE_MAP(AppSettings, Standard)
	ON_WM_CREATE()
END_MESSAGE_MAP()



Dialog::AppSettings::AppSettings()
	: Standard(IDD_DMI_STANDARD, "AppSettings", nullptr)
{
}



Dialog::AppSettings::~AppSettings()
{
}



void Dialog::AppSettings::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	//DDX_CONTROL(Initialize);
	//DDX_CONTROL(Reset);
}



BOOL Dialog::AppSettings::OnInitDialog()
{
	__super::OnInitDialog();

	Json::Object& data = GetUiData().GetAt("size");

	CSize frame = GetFrameThickness();
	CSize winSize = globalUtils.ScaleByDPI(CSize(data.GetInteger("cx"), data.GetInteger("cy")));
	CRect body = { 0, frame.cy, winSize.cx, winSize.cy };
	ConstructBody(body);
	ConstructFooter(body);

	AdjustWindowSize(winSize);

	return TRUE;
}



int Dialog::AppSettings::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	return 0;
}



CRect Dialog::AppSettings::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	return {};
}



CRect Dialog::AppSettings::ConstructFooter(const CRect& boundary)
{
	Json::Object& footer = GetUiData().GetAt("footer");
	Json::Object& buttons = TheAppResources.GetDialog("DefaultButtons");

	int maxHeight = 0;

	maxHeight = max(maxHeight, SetupControl(m_wndInitialize, SetData(footer.GetAt("Initialize"), (UINT)PRESET::EControlId::Initialize)).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndReset, SetData(footer.GetAt("Reset"), (UINT)PRESET::EControlId::Reset)).cy);
	//maxHeight = max(maxHeight, SetupControl(m_wndOk, buttons.GetAt("Ok")).cy);
	//maxHeight = max(maxHeight, SetupControl(m_wndApply, buttons.GetAt("Apply")).cy);
	//maxHeight = max(maxHeight, SetupControl(m_wndCancel, buttons.GetAt("Cancel")).cy);

	m_nFooterHeight = maxHeight + FooterPadding();

	CPoint basePoint;

	basePoint.y = boundary.bottom - maxHeight / 2;
	AlignControls({ &m_wndInitialize, &m_wndReset }, basePoint, Component::EAlign::VerticalCenter);
	DestributeControls({ &m_wndInitialize, &m_wndReset }, basePoint, FooterPadding(), Component::EDirection::ToRight);

	basePoint.x = boundary.right;
	//AlignControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, Component::EAlign::VerticalCenter);
	//DestributeControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, FooterPadding(), Component::EDirection::ToLeft);

	return CRect();
}

#undef DDX_CONTROL
