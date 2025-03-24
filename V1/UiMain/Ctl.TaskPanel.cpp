#include "stdafx.h"

#include "Ast.h"
#include "Ctl.TaskPanel.h"
#include "Wnd.View.h"

#include <Signal3d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define theDelivery	(*GetTaskBar().GetView()->GetDelivery3d())

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlTaskPanel, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()



CtlTaskPanel::CtlTaskPanel(UINT commandId, Json::Object* pUiData)
	: CommandId(commandId)
	, DesignData(pUiData)
{
	DEBUG_VALID(pUiData);
	Dictionary = DesignData->GetString("dictionary");
}



bool CtlTaskPanel::Initialize(CWnd* pParentWnd)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, {}, pParentWnd, WM_USER) == FALSE) {
		RETURN_FALSE;
	}

	ConstructHeader();
	ConstructFooter();
	ConstructBody();

	Initialized = true;

	return true;
}



void CtlTaskPanel::ConstructBody()
{
	if (PropListCtl.Initialize(this, WM_USER) == false) {
		DEBUG_RETURN;
	}

	PropListCtl.InitializeDesign(GetUiData().GetAt("properties"), Dictionary);
}



void CtlTaskPanel::OnReset(CBCGPProp* pProp)
{
	Initialized = false;

	CBCGPProp* pParent = pProp->GetParent();
	DEBUG_VALID(pParent);
	ASSERT(pParent->IsGroup());

	CStringA name = (CStringA)pParent->GetXMLTagName();
	Json::Object& target = ValueData.GetAt(name);
	// grouped(checked) value
	bool checked = target.GetBoolean("checked");

	// WARNING - copy data, do not use reference
	target = DefaultData.GetAt(name);
	target.SetBoolean("checked", checked);

	PropListCtl.InitializeValue(ValueData);

	Initialized = true;
	PropListCtl.RedrawWindow();
}



void CtlTaskPanel::SetData(Json::Object& data)
{
	ValueData = data;

	Initialized = false;
	PropListCtl.InitializeValue(ValueData);
	Initialized = true;
}



CSize CtlTaskPanel::GetSize()
{
	Json::Object& header = GetUiData().GetAt("header");
	return globalUtils.ScaleByDPI(Ast::GetSize(header.GetAt("size")));
}



CString CtlTaskPanel::GetTitle()
{
	DEBUG_VALID(DesignData);
	return Ast::GetTitle(GetUiData().GetAt("header"));
}



CtlTaskBar& CtlTaskPanel::GetTaskBar()
{
	DEBUG_VALID(GetParent());
	return *(CtlTaskBar*)GetParent();
}



BOOL CtlTaskPanel::OnEraseBkgnd(CDC* pDC)
{
	const CBrush backgound((COLORREF)Ctl::EColor::DialogBack);

	CRect rect;
	GetClientRect(rect);

	pDC->FillRect(rect, (CBrush*)&backgound);

	return __super::OnEraseBkgnd(pDC);
}



LRESULT CtlTaskPanel::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	if (Initialized == false) {
		return S_OK;
	}

	CBCGPProp* pProp = (CBCGPProp*)lp;
	Json::Value* pValue = reinterpret_cast<Json::Value*>(pProp->GetData());

	if (pValue != nullptr) {
		if (pProp->IsGroupWithCheckBox()) {
			pValue->AsObject().SetBoolean("checked", (bool)pProp->IsGroupChecked());
		}
		else {
			Ast::SetValue(*pValue, *pProp);
		}
	}
	else {
		//:CHECK
		const int resetButtonIdMax = 10;

		UINT id = pProp->GetID();
		if (id < resetButtonIdMax) {
			if (pProp->IsEnabled() == TRUE) {
				OnReset(pProp);
			}
		}
		else {
			theDelivery.taskBar.OnClickedValue(CommandId, pProp->GetXMLTagName());
		}
	}

	ASSERT(CommandId > 0);
	if (AllowAllData) {
		theDelivery.taskBar.OnChangedValue(CommandId, ValueData);
	}
	else {
		theDelivery.taskBar.OnChangedValue(CommandId, pProp->GetXMLTagName(), *pValue);
	}

	return S_OK;
}



void CtlTaskPanel::OnSize(UINT nType, int cx, int cy)
{
	if (cx == 0 || cy == 0) {
		return;
	}

	int margin = Ctl::Gap().cy;
	CPoint propTop;
	CSize propSize;

	if (ToolBarCtl.GetSafeHwnd() != nullptr) {
		ToolBarCtl.AdjustLayout();
		CSize toolBarSize = Ctl::GetSize(&ToolBarCtl);

		propTop = { 0, toolBarSize.cy + margin };
		propSize = { cx, cy - toolBarSize.cy - margin };
	}
	else {
		propSize = { cx, cy };
	}

	PropListCtl.SetWindowPos(NULL, propTop.x, propTop.y, propSize.cx, propSize.cy, SWP_NOACTIVATE);
}

#undef theDelivery
