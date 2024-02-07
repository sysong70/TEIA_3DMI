#include "stdafx.h"
#include "Control.TreeCtrlEx.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetTreeCtrlEx

namespace PresetTreeCtrlEx
{
}



using namespace Control;

BEGIN_MESSAGE_MAP(TreeCtrlEx, CBCGPTreeCtrlEx)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



Control::TreeCtrlEx::TreeCtrlEx()
{
	m_bVisualManagerStyle = TRUE;
}



Control::TreeCtrlEx::~TreeCtrlEx()
{
}

//:REF - https://learn.microsoft.com/ko-kr/windows/win32/controls/tree-view-control-window-styles
// Component::ModelPanel::ConstructBody()

bool Control::TreeCtrlEx::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE
//		| TVS_CHECKBOXES
//		| TVS_DISABLEDRAGDROP
//		| TVM_EDITLABEL
		| TVS_FULLROWSELECT
		| TVS_HASBUTTONS
		| TVS_HASLINES
//		| TVS_LINESATROOT
//		| TVS_INFOTIP
//		| TVS_NOHSCROLL
//		| TVS_NOTOOLTIPS
//		| TVS_RTLREADING
		| TVS_SHOWSELALWAYS
//		| TVS_SINGLEEXPAND
//		| TVS_TRACKSELECT
		;

	if (Create(dwStyle, rect, pParentWnd, id) == FALSE) {
		RETURN_FALSE;
	}

	return true;
}



void Control::TreeCtrlEx::InitializeDesign(Json::Array& design)
{
	SetRedraw(FALSE);
	m_bInitialized = false;
	{
		CreateItem(design, nullptr);
	}
	m_bInitialized = true;
	SetRedraw(TRUE);

	AdjustLayout();
}



void Control::TreeCtrlEx::GetAncestor(HTREEITEM pItem, std::vector<HTREEITEM>& ancestor)
{
	HTREEITEM pParent = pItem;
	while (pParent != nullptr) {
		ancestor.push_back(pParent);
		pParent = GetParentItem(pParent);
	}
}



CString Control::TreeCtrlEx::GetItemNamePath(HTREEITEM pItem)
{
	std::vector<HTREEITEM> items;
	GetAncestor(pItem, items);

	CString path;
	std::vector<HTREEITEM>::reverse_iterator iter;
	for (iter = items.rbegin(); iter != items.rend(); iter++) {
		CString* pName = reinterpret_cast<CString*>(GetItemData(*iter));
		if (pName != nullptr) {
			path += L'/' + *pName;
		}
		else {
			DEBUG_STOP;
		}
	}

	path.TrimLeft(L'/');

	return path;
}



void Control::TreeCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT flag = 0;
	HTREEITEM hItem = HitTest(point, &flag);
	if (hItem == nullptr) {
		__super::OnLButtonDown(nFlags, point);
	}

	if (flag & TVHT_ONITEMBUTTON) {
		UINT state = GetItemState(hItem, TVIS_EXPANDED);
		Expand(hItem, (state & TVIS_EXPANDED ? TVE_COLLAPSE : TVE_EXPAND));
		return;
	}

	__super::OnLButtonDown(nFlags, point);
}



HTREEITEM Control::TreeCtrlEx::CreateItem(Json::Object& design, HTREEITEM pParent)
{
	if (design.GetBoolean("visible", true) == false) {
		return nullptr;
	}

	HTREEITEM pItem = InsertItem(Facility::GetTitle(design), pParent);
	if (GetSelectedItem() == nullptr) {
		SelectItem(pItem);
	}

	Json::Value* pValue = design.FindValue("name");
	if (pValue != nullptr) {
		SetItemData(pItem, (DWORD_PTR)&(pValue->AsString()));
	}

	Json::Array* pItems = Facility::GetItems(design);
	if (pItems != nullptr) {
		CreateItem(*pItems, pItem);
	}

	return pItem;
}



void Control::TreeCtrlEx::CreateItem(Json::Array& design, HTREEITEM pParent)
{
	for (auto item : design.GetBuffer()) {
		if (item == nullptr) {
			continue;
		}

		if (item->IsObject()) {
			CreateItem(item->AsObject(), pParent);
		}
	}
}

#undef PRESET
