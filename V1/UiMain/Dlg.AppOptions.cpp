#include "stdafx.h"

#include "Ast.AppOptions.h"
#include "Ast.AppResources.h"
#include "Cnt.h"
#include "Dlg.AppOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	enum class Id
	{
		This = WM_USER,
		PreferenceProp,
		FileProp,
		Initialize,
		Reset,
	};
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(DlgAppOptions, DlgStandard)
	ON_REGISTERED_MESSAGE(BCGM_CHANGE_ACTIVE_TAB, OnChangeActiveTab)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnChangedFileOption)

	ON_BN_CLICKED(Id::Initialize, OnInitialize)
	ON_BN_CLICKED(Id::Reset, OnReset)
	ON_BN_CLICKED(IDCONTINUE, OnApply)
END_MESSAGE_MAP()



BOOL DlgAppOptions::OnInitDialog()
{
	__super::OnInitDialog();

	BeginWaitCursor();

	CSize frame = GetFrameThickness();
	CSize size = GetWinSize();
	CRect body = { 0, frame.cy, size.cx, size.cy };

	ConstructFooter(body);
	body.bottom -= FooterHeight;
	ConstructBody(body);

	WindowSize = AdjustWindowSize(size);
	SetSizeLimit(true, true);

	// WARNING
	FileOptionsCtl.OnPropertyChangedHandler(this);

	// data initialize

	PreferencesSet.ValueResource = TheAppResources.Preferences;
	FileOptionsSet.ValueResource = TheAppResources.FileOptions;

	PreferencesSet.ValueLocal = TheAppOptions.Preferences;
	FileOptionsSet.ValueLocal = TheAppOptions.FileOptions;

	PreferencesSet.Value = PreferencesSet.ValueLocal;
	FileOptionsSet.Value = FileOptionsSet.ValueLocal;

	PreferencesCtl.InitializeValue(PreferencesSet.Value);
	FileOptionsCtl.InitializeValue(FileOptionsSet.Value);

	// design initialize

	Json::Array& tabs = GetUiData().GetAt("body").GetArray("tabs");
	PreferencesSet.Design = TheAppResources.GetDialog((CStringA)tabs.GetAt(0)->AsString());
	FileOptionsSet.Design = TheAppResources.GetDialog((CStringA)tabs.GetAt(1)->AsString());

	PreferencesCtl.InitializeDesign(PreferencesSet.Design);
	FileOptionsCtl.InitializeDesign(FileOptionsSet.Design);

	EndWaitCursor();

	return TRUE;
}



void DlgAppOptions::OnOK()
{
	OnApply();

	__super::OnOK();
}



LRESULT DlgAppOptions::OnChangedFileOption(WPARAM wp, LPARAM lp)
{
	Json::Value& target = *(Json::Value*)wp;
	CBCGPProp& source = *(CBCGPProp*)lp;

	// set value first
	Ast::SetValue(target, source);

	CtlPropList& propList = FileOptionsCtl.PropListCtl;
	CBCGPProp* pParent = source.GetParent();

	CString name = source.GetXMLTagName();

	if (name == L"TessLevel") {
		const int customTessLevel = 5;

		CBCGPProp* pCustom = propList.FindPropByName(pParent, L"CustomTessLevel");
		DEBUG_VALID(pCustom);

		int index = source.GetSelectedOption();
		pCustom->Enable(index == customTessLevel, TRUE);
	}

	return S_OK;
}



LRESULT DlgAppOptions::OnChangeActiveTab(WPARAM wp, LPARAM lp)
{
	int index = (int)wp;

	//:TODO

	return S_OK;
}



void DlgAppOptions::OnInitialize()
{
	int index = TabsCtl.GetActiveTab();
	if (index == 0) {
		PreferencesSet.Value = PreferencesSet.ValueResource;
		PreferencesCtl.RefreshData();
	}
	else {
		FileOptionsSet.Value = FileOptionsSet.ValueResource;
		FileOptionsCtl.RefreshData();
	}
}



void DlgAppOptions::OnReset()
{
	int index = TabsCtl.GetActiveTab();
	if (index == 0) {
		PreferencesSet.Value = PreferencesSet.ValueLocal;
		PreferencesCtl.RefreshData();
	}
	else {
		FileOptionsSet.Value = FileOptionsSet.ValueLocal;
		FileOptionsCtl.RefreshData();
	}
}



void DlgAppOptions::OnApply()
{
	TheAppOptions.Preferences = PreferencesSet.Value;
	TheAppOptions.FileOptions = FileOptionsSet.Value;
	TheAppOptions.Save();

	Cnt3d::GetInstance().application.OnUpdatePreference(PreferencesSet.Value);
	Cnt3d::GetInstance().application.OnUpdateFileOption(FileOptionsSet.Value);
	//Cnt2d::GetInstance().application.OnUpdatePreference(m_preferences.Data);
	//Cnt2d::GetInstance().application.OnUpdatePreference(m_fileOptions.Data);
}



void DlgAppOptions::ConstructBody(const CRect& boundary)
{
	// WARNING - setting before Create()
	TabsCtl.SetTabHeight(Ctl::TabHeight());

	if (TabsCtl.Create(CBCGPTabWnd::STYLE_3D, boundary, this, (UINT)Id::This) == FALSE) {
		DEBUG_RETURN;
	}

	TabsCtl.SetLocation(CBCGPTabWnd::LOCATION_TOP);

	PreferencesCtl.Initialize(&TabsCtl);
	FileOptionsCtl.Initialize(&TabsCtl);

	TabsCtl.AddTab(&PreferencesCtl, Ast::Local(L"Preference|환경 설정"));
	TabsCtl.AddTab(&FileOptionsCtl, Ast::Local(L"File Options|파일 설정"));

	TabsCtl.SetActiveTab(0);
}



void DlgAppOptions::ConstructFooter(const CRect& boundary)
{
	Json::Object& footer = GetUiData().GetAt("footer");
	Json::Object& buttons = GetDefaultButtons();
	CSize margin = Ctl::Gap();
	CSize size;
	int maxHeight = 0;

	size = Ctl::Setup(InitializeCtl, Ast::SetData(footer.GetAt("Initialize"), (UINT)Id::Initialize), this); maxHeight = max(maxHeight, size.cy);
	size = Ctl::Setup(ResetCtl, Ast::SetData(footer.GetAt("Reset"), (UINT)Id::Reset), this); maxHeight = max(maxHeight, size.cy);
	size = Ctl::Setup(OkCtl, buttons.GetAt("Ok"), this); maxHeight = max(maxHeight, size.cy);
	size = Ctl::Setup(ApplyCtl, buttons.GetAt("Apply"), this); maxHeight = max(maxHeight, size.cy);
	size = Ctl::Setup(CancelCtl, buttons.GetAt("Cancel"), this); maxHeight = max(maxHeight, size.cy);

	FooterHeight = maxHeight + margin.cy;

	CPoint basePoint;
	basePoint.y = boundary.bottom - maxHeight / 2;
	basePoint.x = boundary.left;

	Ctl::Align({ &InitializeCtl, &ResetCtl }, basePoint, Ctl::EAlign::VerticalCenter, this);
	Ctl::Destribute({ &InitializeCtl, &ResetCtl }, basePoint, margin.cx, Ctl::EDirection::ToRight, this);

	Ctl::Align({ &CancelCtl, &ApplyCtl, &OkCtl }, basePoint, Ctl::EAlign::VerticalCenter, this);
	basePoint.x = boundary.right;
	Ctl::Destribute({ &CancelCtl, &ApplyCtl, &OkCtl }, basePoint, margin.cx, Ctl::EDirection::ToLeft, this);
}
