#include "stdafx.h"
#include "Dialog.FileOpenOption.h"
#include "Data.Base.h"
#include "Data.AppSettings.h"
#include "Window.h"
#include <WStr.h>

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif

OPEN_DIALOG_NAMESPACE;

#pragma region Preset

struct TessellationLevel
{
	CString ChordLimit;
	CString AngleTolerance;
};

TessellationLevel TESS_LEVEL[] = {
	{    L"50", L"40" },
	{   L"600", L"40" },
	{  L"2000", L"40" },
	{  L"5000", L"30" },
	{ L"10000", L"20" }
};

#pragma endregion

class CHelper
{
public:

	CHelper() { }

	FileOpenOption* pSource;

	FileOpenOption& source()
	{
		return *pSource;
	}

	void OnChangedTessLevel(CBCGPProp* pTarget)
	{
		OnChangedTessLevel(pTarget->GetSelectedOption());
	}

	void OnChangedTessLevel(int value)
	{
		source().GetPropertyByName("CUSTOMTESS")->Enable(value == (int)(Data::ImportOption::ETessLevel::Custom), TRUE);
	}

	void OnChangedAccurateTessellation(CBCGPProp* pTarget)
	{
		CBCGPProp* pAccurateTess = pTarget;
		CBCGPProp* pGridAlignedTess = source().GetPropertyByName("GridAlignedTess");

		bool b1 = (bool)pAccurateTess->GetValue();
		bool b2 = (bool)pGridAlignedTess->GetValue();
		pGridAlignedTess->Enable(b1);

		OnChangedGridAlignedTess(b1 && b2);
	}

	void OnChangedGridAlignedTess(CBCGPProp* pTarget)
	{
		OnChangedGridAlignedTess((bool)pTarget->GetValue());
	}

	void OnChangedGridAlignedTess(bool value)
	{
		source().GetPropertyByName("MaxStitchLength")->Enable(value);
		source().GetPropertyByName("SurfaceCurvatures")->Enable(value);
	}
};

class CHelper theHelper;

#pragma region Message Map

BEGIN_MESSAGE_MAP(FileOpenOption, PropertyList)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_COMMAND_CLICKED, OnCommandClicked)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_MENU_ITEM_SELECTED, OnMenuItemSelected)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)

	ON_BN_CLICKED((int)Extra::ResetDefaults, OnResetDefaults)
END_MESSAGE_MAP()

#pragma endregion

FileOpenOption::FileOpenOption(CStringA dialogName, Window::DocType eType, CWnd* pParent)
	: PropertyList(dialogName, pParent)
	, m_eDocType(eType)
{
	theHelper.pSource = this;
}



FileOpenOption::~FileOpenOption()
{
}



Data::Base* FileOpenOption::GetResultData(bool bNewInstance)
{
	if (bNewInstance) {
		//:WARNING - case by case
		return new Data::ImportOption(m_data);
	}
	else {
		return &m_data;
	}
}



Json::Object* FileOpenOption::GetResult()
{
	return m_data.Get();
}



CRect FileOpenOption::ConstructBody(CRect bound)
{
	CRect rect = __super::ConstructBody(bound);
	OnResetDefaults();
	return rect;
}



CRect FileOpenOption::ConstructFooter(CRect bound)
{
	CPoint point(bound.left, bound.bottom);
	CSize size;

	Json::Object& data = GetUiData().GetAt("commands");
	CString title = data.GetAt("ResetDefaults").GetString("title");
	title = Window::Local(title);

	size += CreateButton(m_wndResetDefaults, (UINT)Extra::ResetDefaults, title, point);

	return __super::ConstructFooter(bound);
}



BOOL FileOpenOption::OnInitDialog()
{
	BOOL bResult = __super::OnInitDialog();

	CMFCDynamicLayout* pLayout = GetDynamicLayout();
	pLayout->AddItem(m_wndResetDefaults, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeNone());
	//pLayout->AddItem(m_wndSetToDefaults, CMFCDynamicLayout::MoveVertical(100), CMFCDynamicLayout::SizeNone());

	return bResult;
}



void FileOpenOption::OnOK()
{
	m_data.General.ReadingMode = (Data::ImportOption::EReadingMode)GetPropertyByName("ReadingMode")->GetSelectedOption();
	m_data.General.Solids = GetPropertyByName("Solids")->GetValue();
	m_data.General.Surfaces = GetPropertyByName("Surfaces")->GetValue();
	m_data.General.Wireframes = GetPropertyByName("Wireframes")->GetValue();
	m_data.General.Attributes = GetPropertyByName("Attributes")->GetValue();
	m_data.General.HiddenObjects = GetPropertyByName("HiddenObjects")->GetValue();
	m_data.General.References = GetPropertyByName("References")->GetValue();
	m_data.General.ActiveFilter = GetPropertyByName("ActiveFilter")->GetValue();
	m_data.General.SewModel = GetPropertyByName("SewModel")->GetValue();
	m_data.General.SewingTolerance = GetPropertyByName("SewingTolerance")->GetValue();
	m_data.General.ShellOrientation = GetPropertyByName("ShellOrientation")->GetValue();
	m_data.General.LoadingMode = (Data::ImportOption::ELoadingMode)GetPropertyByName("LoadingMode")->GetSelectedOption();

	m_data.Tessellation.TessLevel = (Data::ImportOption::ETessLevel)GetPropertyByName("TessLevel")->GetSelectedOption();
	m_data.Tessellation.ChordLimitType = (Data::ImportOption::EChordLimit)GetPropertyByName("ChordLimitType")->GetSelectedOption();
	m_data.Tessellation.ChordLimit = GetPropertyByName("ChordLimit")->GetValue();
	m_data.Tessellation.AngleTolerance = GetPropertyByName("AngleTolerance")->GetValue();
	m_data.Tessellation.PreserveUV = GetPropertyByName("PreserveUV")->GetValue();
	m_data.Tessellation.MaxEdgeLength = GetPropertyByName("MaxEdgeLength")->GetValue();
	m_data.Tessellation.AccurateTess = GetPropertyByName("AccurateTess")->GetValue();
	m_data.Tessellation.GridAlignedTess = GetPropertyByName("GridAlignedTess")->GetValue();
	m_data.Tessellation.MaxStitchLength = GetPropertyByName("MaxStitchLength")->GetValue();
	m_data.Tessellation.SurfaceCurvatures = GetPropertyByName("SurfaceCurvatures")->GetValue();

	PropertyList::OnOK();
}



void FileOpenOption::OnCancel()
{
	PropertyList::OnCancel();
}



LRESULT FileOpenOption::OnCommandClicked(WPARAM wp, LPARAM lp)
{
	int nCommandIndex = (int)lp;

	return 0;
}



LRESULT FileOpenOption::OnMenuItemSelected(WPARAM wp, LPARAM lp)
{
	int nMenuIndex = (int)wp;

	CBCGPProp* pProp = (CBCGPProp*)lp;
	ASSERT_VALID(pProp);

	// TODO: Set your menu item processing code here

	return 0;
}



LRESULT FileOpenOption::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CBCGPProp* pProp = (CBCGPProp*)lp;
	CString name = GetPropertyName(pProp->GetID());

	if (name == L"TessLevel") {
		theHelper.OnChangedTessLevel(pProp);
	}
	else if (name == L"AccurateTess") {
		theHelper.OnChangedAccurateTessellation(pProp);
	}
	else if (name == "GridAlignedTess") {
		theHelper.OnChangedGridAlignedTess(pProp);
	}

	return 0;
}



void FileOpenOption::OnResetDefaults()
{
	CString typeName = Window::GetDocTypeName(m_eDocType);
	Data::ImportOption* pOption = Data::theSettings.GetImportOption(typeName);

	GetPropertyByName("ReadingMode")->SelectOption((int)pOption->General.ReadingMode);
	GetPropertyByName("Solids")->SetValue(pOption->General.Solids);
	GetPropertyByName("Surfaces")->SetValue(pOption->General.Surfaces);
	GetPropertyByName("Wireframes")->SetValue(pOption->General.Wireframes);
	GetPropertyByName("Attributes")->SetValue(pOption->General.Attributes);
	GetPropertyByName("HiddenObjects")->SetValue(pOption->General.HiddenObjects);
	GetPropertyByName("References")->SetValue(pOption->General.References);
	GetPropertyByName("ActiveFilter")->SetValue(pOption->General.ActiveFilter);
	GetPropertyByName("SewModel")->SetValue(pOption->General.SewModel);
	GetPropertyByName("SewingTolerance")->SetValue((_variant_t)WStr::ToString(pOption->General.SewingTolerance, 3));
	GetPropertyByName("ShellOrientation")->SetValue(pOption->General.ShellOrientation);
	GetPropertyByName("LoadingMode")->SelectOption((int)pOption->General.LoadingMode);

	int nTessLevel = (int)pOption->Tessellation.TessLevel;
	GetPropertyByName("TessLevel")->SelectOption(nTessLevel);
	GetPropertyByName("ChordLimitType")->SelectOption((int)pOption->Tessellation.ChordLimitType);
	GetPropertyByName("ChordLimit")->SetValue((_variant_t)TESS_LEVEL[nTessLevel].ChordLimit);
	GetPropertyByName("AngleTolerance")->SetValue((_variant_t)TESS_LEVEL[nTessLevel].AngleTolerance);
	GetPropertyByName("PreserveUV")->SetValue(pOption->Tessellation.PreserveUV);
	GetPropertyByName("MaxEdgeLength")->SetValue((_variant_t)WStr::ToString(pOption->Tessellation.MaxEdgeLength));
	GetPropertyByName("AccurateTess")->SetValue(pOption->Tessellation.AccurateTess);
	GetPropertyByName("GridAlignedTess")->SetValue(pOption->Tessellation.GridAlignedTess);
	GetPropertyByName("MaxStitchLength")->SetValue((_variant_t)WStr::ToString(pOption->Tessellation.MaxStitchLength, 3));
	GetPropertyByName("SurfaceCurvatures")->SetValue(pOption->Tessellation.SurfaceCurvatures);

	// post process
	theHelper.OnChangedTessLevel(nTessLevel);
}

CLOSE_DIALOG_NAMESPACE