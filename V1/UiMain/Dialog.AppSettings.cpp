#include "stdafx.h"
#include "Dialog.AppSettings.h"
#include "Component.h"
#include "Facility.AppResources.h"
#include "Facility.AppSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define DDX_CONTROL(x) DDX_Control(pDX, (int)PRESET::x, m_wnd##x);

#define PRESET PresetAppSettings

namespace PresetAppSettings
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

BEGIN_MESSAGE_MAP(AppSettings, Standard)
	ON_WM_CREATE()

	ON_COMMAND(PRESET::Initialize, OnInitialize)
	ON_COMMAND(PRESET::Reset, OnReset)
	ON_COMMAND(PRESET::IDAPPLY, OnApply)

	ON_REGISTERED_MESSAGE(BCGM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
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



void Dialog::AppSettings::OnCancel()
{
	__super::OnCancel();
}



BOOL Dialog::AppSettings::OnInitDialog()
{
	__super::OnInitDialog();

	Json::Object& data = GetUiData().GetAt("size");

	CSize frame = GetFrameThickness();
	CSize winSize = globalUtils.ScaleByDPI(CSize(data.GetInteger("cx"), data.GetInteger("cy")));
	CRect body = { 0, frame.cy, winSize.cx, winSize.cy };

	ConstructFooter(body);
	body.bottom -= m_nFooterHeight;
	ConstructBody(body);

	m_windowSize = AdjustWindowSize(winSize);
	SetSizeLimit(true, true);

	// data initialize

	m_preferences.DataResource = TheAppResources.GetPreferences();
	m_fileOptions.DataResource = TheAppResources.GetFileOptions();

	if (TheAppSettings.GetPreferences().IsEmpty()) {
		m_preferences.DataLocal = TheAppResources.GetPreferences();
	}
	else {
		m_preferences.DataLocal = TheAppSettings.GetPreferences();
	}

	if (TheAppSettings.GetFileOptions().IsEmpty()) {
		m_fileOptions.DataLocal = TheAppResources.GetFileOptions();
	}
	else {
		m_fileOptions.DataLocal = TheAppSettings.GetFileOptions();
	}

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



void Dialog::AppSettings::OnOK()
{
	OnApply();
	//:TODO - sand data to connector

	__super::OnOK();
}



LRESULT Dialog::AppSettings::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
	int index = (int)wp;

	return 0;
}



int Dialog::AppSettings::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CBCGPDialog::OnCreate(lpCreateStruct) == -1) {
		DEBUG_STOP;
		return -1;
	}

	return 0;
}



void Dialog::AppSettings::OnInitialize()
{
	int index = m_tabs.GetActiveTab();
	if (index == 0) {
		m_preferences.Data = m_preferences.DataResource;
	}
	else {
		m_fileOptions.Data = m_fileOptions.DataResource;
	}
}



void Dialog::AppSettings::OnReset()
{
	int index = m_tabs.GetActiveTab();
	if (index == 0) {
		m_preferences.Data = m_preferences.DataLocal;
	}
	else {
		m_fileOptions.Data = m_fileOptions.DataLocal;
	}
}



void Dialog::AppSettings::OnApply()
{
	TheAppSettings.GetPreferences() = m_preferences.Data;
	TheAppSettings.GetFileOptions() = m_fileOptions.Data;
	TheAppSettings.Save();
}



void Dialog::AppSettings::ConstructBody(const CRect& boundary)
{
	Json::Object& data = GetUiData().GetAt("body");

	//:WARNING - setting before Create()
	m_tabs.SetTabHeight(Component::TabHeight());

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



void Dialog::AppSettings::ConstructFooter(const CRect& boundary)
{
	Json::Object& footer = GetUiData().GetAt("footer");
	Json::Object& buttons = TheAppResources.GetDialog("DefaultButtons");

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

	AlignControls({ &m_wndInitialize, &m_wndReset }, basePoint, Component::EAlign::VerticalCenter);
	DestributeControls({ &m_wndInitialize, &m_wndReset }, basePoint, FooterPadding(), Component::EDirection::ToRight);

	AlignControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, Component::EAlign::VerticalCenter);
	basePoint.x = boundary.right;
	DestributeControls({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, FooterPadding(), Component::EDirection::ToLeft);
}

#undef DDX_CONTROL
#undef PRESET
