#include "stdafx.h"
#include "Control.TaskPanel.h"
#include "Facility.AppResources.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET TaskPanelPreset

namespace TaskPanelPreset
{
	// CHECK
	const int ResetButtonIdMax = 10;
}

//**************************************************************************************************

using namespace Control;

BEGIN_MESSAGE_MAP(TaskPanel, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()



Control::TaskPanel::TaskPanel(UINT commandId, Json::Object* pUiData)
	: m_commandId(commandId)
	, m_pUiData(pUiData)
{
	DEBUG_VALID(pUiData);
	m_dictionary = m_pUiData->GetString("dictionary");
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
	if (m_propList.Initialize(this, WM_USER) == false) {
		DEBUG_RETURN;
	}

	m_propList.InitializeDesign(GetUiData().GetAt("properties"), m_dictionary);
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

	CStringA name = (CStringA)pParent->GetXMLTagName();
	Json::Object& target = m_data.GetAt(name);
	// grouped(checked) value
	bool checked = target.GetBoolean("checked");

	// WARNING - copy data, do not use reference
	target = m_defaultData.GetAt(name);
	target.SetBoolean("checked", checked);

	m_propList.InitializeData(m_data);

	m_bInitialized = true;
	m_propList.RedrawWindow();
}



void Control::TaskPanel::ReceiveSignal(Json::Object* pData)
{
	DEBUG_STOP;
}



void Control::TaskPanel::SetData(Json::Object& data)
{
	m_data = data;

	m_bInitialized = false;
	m_propList.InitializeData(m_data);
	m_bInitialized = true;
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
			if (pProp->IsEnabled() == TRUE) {
				OnReset(pProp);
			}
		}
		else {
			GetTaskBar().GetDelivery().taskBar.OnClickedValue(m_commandId, pProp->GetXMLTagName());
		}
	}

	ASSERT(m_commandId > 0);
	if (m_bSendAllData) {
		GetTaskBar().GetDelivery().taskBar.OnChangedValue(m_commandId, m_data);
	}
	else {
		GetTaskBar().GetDelivery().taskBar.OnChangedValue(m_commandId, pProp->GetXMLTagName(), *pValue);
	}

	return S_OK;
}



void Control::TaskPanel::OnSize(UINT nType, int cx, int cy)
{
	if (cx == 0 || cy == 0) {
		return;
	}

	int margin = Control::Gap().cy;
	CPoint propTop;
	CSize propSize;

	if (m_toolBar.GetSafeHwnd() != nullptr) {
		m_toolBar.AdjustLayout();
		CSize toolBarSize = Control::GetSize(&m_toolBar);

		propTop = { 0, toolBarSize.cy + margin };
		propSize = { cx, cy - toolBarSize.cy - margin };
	}
	else {
		propSize = { cx, cy };
	}

	m_propList.SetWindowPos(NULL, propTop.x, propTop.y, propSize.cx, propSize.cy, SWP_NOACTIVATE);
}

#undef PRESET
