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
	enum EControlId
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
	ON_REGISTERED_MESSAGE(BCGM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnChangedFileOption)

	ON_BN_CLICKED(PRESET::Initialize, OnInitialize)
	ON_BN_CLICKED(PRESET::Reset, OnReset)
	ON_BN_CLICKED(IDCONTINUE, OnApply)
END_MESSAGE_MAP()



Dialog::AppOptions::AppOptions()
	: Standard(IDD_DMI_STANDARD, "AppOptions", nullptr)
{
}



Dialog::AppOptions::~AppOptions()
{
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

	//:WARNING
	m_fileOptionsUi.OnPropertyChangedHandler(this);

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

	//:TODO

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
	//Connector2d::GetInstance().application.OnUpdatePreference(m_preferences.Data);
	//Connector2d::GetInstance().application.OnUpdatePreference(m_fileOptions.Data);
}



void Dialog::AppOptions::ConstructBody(const CRect& boundary)
{
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
	CSize margin = Control::Gap();
	CSize size;
	int maxHeight = 0;

	size = Control::Setup(m_wndInitialize, Facility::SetData(footer.GetAt("Initialize"), PRESET::Initialize), this); maxHeight = max(maxHeight, size.cy);
	size = Control::Setup(m_wndReset, Facility::SetData(footer.GetAt("Reset"), PRESET::Reset), this); maxHeight = max(maxHeight, size.cy);
	size = Control::Setup(m_wndOk, buttons.GetAt("Ok"), this); maxHeight = max(maxHeight, size.cy);
	size = Control::Setup(m_wndApply, buttons.GetAt("Apply"), this); maxHeight = max(maxHeight, size.cy);
	size = Control::Setup(m_wndCancel, buttons.GetAt("Cancel"), this); maxHeight = max(maxHeight, size.cy);

	m_nFooterHeight = maxHeight + margin.cy;

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	Control::Align({ &m_wndInitialize, &m_wndReset }, basePoint, Control::EAlign::VerticalCenter, this);
	Control::Destribute({ &m_wndInitialize, &m_wndReset }, basePoint, margin.cx, Control::EDirection::ToRight, this);

	Control::Align({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, Control::EAlign::VerticalCenter, this);
	basePoint.x = boundary.right;
	Control::Destribute({ &m_wndCancel, &m_wndApply, &m_wndOk }, basePoint, margin.cx, Control::EDirection::ToLeft, this);
}



LRESULT Dialog::AppOptions::OnChangedFileOption(WPARAM wp, LPARAM lp)
{
	Json::Value& target = *(Json::Value*)wp;
	CBCGPProp& source = *(CBCGPProp*)lp;

	// set value first
	Facility::SetValue(target, source);

	Control::PropList& propList = m_fileOptionsUi.GetPropList();
	CBCGPProp* pParent = source.GetParent();

	CString name = source.GetXMLTagName();

	if (name == L"TessLevel") {
		CBCGPProp* pCustom = propList.FindPropByName(pParent, L"CustomTessLevel");
		DEBUG_VALID(pCustom);

		int index = source.GetSelectedOption();
		pCustom->Enable(index == 5 /* Custom */, TRUE);
	}

	return S_OK;
}

#undef DDX_CONTROL
#undef PRESET
