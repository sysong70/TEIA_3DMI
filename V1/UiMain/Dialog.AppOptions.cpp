#include "stdafx.h"
#include "Dialog.AppOptions.h"
#include "Component.h"
#include "Connector.h"
#include "Facility.AppResources.h"
#include "Facility.AppOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::x, m_wnd##x);

#define PRESET PresetAppOptions

namespace PresetAppOptions
{
	const int IDAPPLY = 3;

	enum ControlId
	{
		Id = WM_USER,
		PreferenceProp,
		FileProp,
		Initialize,
		Reset,
	};
}



using namespace Dialog;

BEGIN_MESSAGE_MAP(AppOptions, Standard)
	ON_COMMAND(PRESET::Initialize, OnInitialize)
	ON_COMMAND(PRESET::Reset, OnReset)
	ON_COMMAND(PRESET::IDAPPLY, OnApply)

	ON_REGISTERED_MESSAGE(BCGM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
END_MESSAGE_MAP()



Dialog::AppOptions::AppOptions()
	: Standard(IDD_DMI_STANDARD, "AppOptions", nullptr)
{
}



Dialog::AppOptions::~AppOptions()
{
}



void Dialog::AppOptions::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	//DDX_CONTROL(Initialize);
	//DDX_CONTROL(Reset);
}



void Dialog::AppOptions::OnCancel()
{
	__super::OnCancel();
}



BOOL Dialog::AppOptions::OnInitDialog()
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

	// data initialize

	m_preferences.DataResource = TheAppResources.GetPreferences();
	m_fileOptions.DataResource = TheAppResources.GetFileOptions();

	m_preferences.DataLocal = TheAppOptions.GetPreferences();
	m_fileOptions.DataLocal = TheAppOptions.GetFileOptions();

	m_preferences.Data = m_preferences.DataLocal;
	m_fileOptions.Data = m_fileOptions.DataLocal;

	m_preferencesUi.InitializeData(m_preferences.Data);
	m_fileOptionsUi.InitializeData(m_fileOptions.Data);

	// design initialize

	Json::Array& tabs = GetUiData().GetAt("body").GetArray("tabs");
	m_preferences.Design = TheAppResources.GetDialog((CStringA)tabs.GetAt(0)->AsString());
	m_fileOptions.Design = TheAppResources.GetDialog((CStringA)tabs.GetAt(1)->AsString());

	m_preferencesUi.InitializeDesign(m_preferences.Design);
	m_fileOptionsUi.InitializeDesign(m_fileOptions.Design);

	return TRUE;
}



void Dialog::AppOptions::OnOK()
{
	OnApply();

	__super::OnOK();
}



LRESULT Dialog::AppOptions::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
	int index = (int)wp;

	return S_OK;
}



void Dialog::AppOptions::OnInitialize()
{
	int index = m_tabs.GetActiveTab();
	if (index == 0) {
		m_preferences.Data = m_preferences.DataResource;
		m_preferencesUi.RefreshData();
	}
	else {
		m_fileOptions.Data = m_fileOptions.DataResource;
		m_fileOptionsUi.RefreshData();
	}
}



void Dialog::AppOptions::OnReset()
{
	int index = m_tabs.GetActiveTab();
	if (index == 0) {
		m_preferences.Data = m_preferences.DataLocal;
		m_preferencesUi.RefreshData();
	}
	else {
		m_fileOptions.Data = m_fileOptions.DataLocal;
		m_fileOptionsUi.RefreshData();
	}
}



void Dialog::AppOptions::OnApply()
{
	TheAppOptions.GetPreferences() = m_preferences.Data;
	TheAppOptions.GetFileOptions() = m_fileOptions.Data;
	TheAppOptions.Save();

	Connector3d::GetInstance().application.OnUpdatePreference(m_preferences.Data);
	Connector3d::GetInstance().application.OnUpdateFileOption(m_fileOptions.Data);
	Connector2d::GetInstance().application.OnUpdatePreference(m_preferences.Data);
	Connector2d::GetInstance().application.OnUpdatePreference(m_fileOptions.Data);
}



void Dialog::AppOptions::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	//:WARNING - setting before Create()
	m_tabs.SetTabHeight(Control::TabHeight());

	if (m_tabs.Create(CBCGPTabWnd::STYLE_3D, boundary, this, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	m_tabs.SetLocation(CBCGPTabWnd::LOCATION_TOP);

	m_preferencesUi.Initialize(&m_tabs);
	m_fileOptionsUi.Initialize(&m_tabs);

	m_tabs.AddTab(&m_preferencesUi, Facility::Local(L"Preference|환경 설정"));
	m_tabs.AddTab(&m_fileOptionsUi, Facility::Local(L"File Options|파일 설정"));

	m_tabs.SetActiveTab(0);
}



void Dialog::AppOptions::ConstructFooter(const CRect& boundary)
{
	Json::Object& footer = GetUiData().GetAt("footer");
	Json::Object& buttons = GetDefaultButtons();

	int maxHeight = 0;

	maxHeight = max(maxHeight, SetupControl(m_wndInitialize, Facility::SetData(footer.GetAt("Initialize"), PRESET::Initialize)).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndReset, Facility::SetData(footer.GetAt("Reset"), PRESET::Reset)).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndOk, buttons.GetAt("Ok")).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndApply, buttons.GetAt("Apply")).cy);
	maxHeight = max(maxHeight, SetupControl(m_wndCancel, buttons.GetAt("Cancel")).cy);

	m_nFooterHeight = maxHeight + FooterPadding();

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	AlignControls({ &m_wndInitialize, &m_wndReset }, basePoint, Control::EAlign::VerticalCenter);
	DestributeControls({ &m_wndInitialize, &m_wndReset }, basePoint, FooterPadding(), Control::EDirection::ToRight);

	AlignControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, Control::EAlign::VerticalCenter);
	basePoint.x = boundary.right;
	DestributeControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, FooterPadding(), Control::EDirection::ToLeft);
}

#undef DDX_CONTROL
#undef PRESET
