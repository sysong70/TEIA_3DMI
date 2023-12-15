#include "stdafx.h"
#include "Control.TaskPanel.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET TaskPanelPreset

namespace TaskPanelPreset
{
	//:CHECK
	const int ResetButtonIdMax = 10;
}



using namespace Control;

BEGIN_MESSAGE_MAP(TaskPanel, CWnd)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
END_MESSAGE_MAP()



Control::TaskPanel::TaskPanel(UINT commandId, Json::Object* pUiData)
	: m_commandId(commandId)
	, m_pUiData(pUiData)
{
}



Control::TaskPanel::~TaskPanel()
{
}



bool Control::TaskPanel::Initialize(CWnd* pParentWnd)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	ConstructHeader();
	ConstructFooter();
	ConstructBody();

	m_bInitialized = true;

	return true;
}



void Control::TaskPanel::ConstructBody()
{
}



void Control::TaskPanel::ConstructFooter()
{
}



void Control::TaskPanel::ConstructHeader()
{
}



void Control::TaskPanel::OnReset(CBCGPProp* pProp)
{
	m_bInitialized = false;

	CBCGPProp* pParent = pProp->GetParent();
	DEBUG_VALID(pParent);
	ASSERT(pParent->IsGroup());

	//:WARNING - copy data, do not use reference
	CStringA name = (CStringA)pParent->GetXMLTagName();
	m_data.GetAt(name) = m_defaultData.GetAt(name);

	m_propList.InitializeData(m_data);

	m_bInitialized = true;
	m_propList.RedrawWindow();
}



void Control::TaskPanel::SetData(Json::Object& data)
{
	m_data = data;
}



void Control::TaskPanel::SetDefaultData(Json::Object& data)
{
	m_defaultData = data;
}



CSize Control::TaskPanel::GetSize()
{
	Json::Object& header = GetUiData().GetAt("header");
	return globalUtils.ScaleByDPI(Facility::GetSize(header.GetAt("size")));
}



CString Control::TaskPanel::GetTitle()
{
	DEBUG_VALID(m_pUiData);
	return Facility::GetTitle(GetUiData().GetAt("header"));
}



Json::Object& Control::TaskPanel::GetUiData()
{
	return *m_pUiData;
}



Component::TaskBar& Control::TaskPanel::GetTaskBar()
{
	DEBUG_VALID(GetParent());
	return *(Component::TaskBar*)GetParent();
}



LRESULT Control::TaskPanel::OnDPIChangedAfterParent(WPARAM wp, LPARAM lp)
{
	return 0;
}



BOOL Control::TaskPanel::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Control::EColor::DialogBack);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



LRESULT Control::TaskPanel::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	if (m_bInitialized == false) {
		return S_OK;
	}

	CBCGPProp* pProp = (CBCGPProp*)lp;
	Json::Value* pValue = reinterpret_cast<Json::Value*>(pProp->GetData());

	if (pValue != nullptr) {
		if (pProp->IsGroupWithCheckBox()) {
			pValue->AsObject().SetBoolean("checked", (bool)pProp->IsGroupChecked());
		}
		else {
			Facility::SetValue(*pValue, *pProp);
		}
	}
	else {
		UINT id = pProp->GetID();
		if (id < PRESET::ResetButtonIdMax) {
			OnReset(pProp);
		}
		else {
			DEBUG_STOP;
		}
	}

	ASSERT(m_commandId > 0);
	GetTaskBar().GetDelivery().taskBar.OnChangedValue(m_commandId, m_data);

	return S_OK;
}

#undef PRESET
