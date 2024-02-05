#include "stdafx.h"
#include "resource.h"
#include "Component.ModelPanel.h"
#include "Facility.h"
#include "Window.Document.h"
#include "Window.View.h"
#include <Path.h>
#include <Signal.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetModelPanel

namespace PresetModelPanel
{
	const UINT Id = WM_USER;

	WCHAR DummyName[] = L"_$_DUMMY_$_";
}



using namespace Component;

BEGIN_MESSAGE_MAP(ModelPanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnTreeCheckClick)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Id, OnTreeBeginDrag)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Id, OnTreeBeginLabelEdit)
	ON_NOTIFY(NM_CLICK, PRESET::Id, OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, PRESET::Id, OnTreeDblClick)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Id, OnTreeDeleteItem)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Id, OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Id, OnTreeItemExpanded)
	//ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Id, OnTreeItemExpanding)
	ON_NOTIFY(NM_RCLICK, PRESET::Id, OnTreeRClick)
	ON_NOTIFY(NM_RDBLCLK, PRESET::Id, OnTreeRDbClick)
	ON_NOTIFY(TVN_SELCHANGED, PRESET::Id, OnTreeSelChanged)
	//ON_NOTIFY(TVN_SELCHANGING, PRESET::Id, OnTreeSelChanging)
	ON_NOTIFY(NM_SETFOCUS, PRESET::Id, OnTreeSetFocus)

	//ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_COMMAND_RANGE(TOOLBAR_3D_CMD_Sort_ByOriginal, TOOLBAR_3D_CMD_Option_AlternateRows, OnCommand)
END_MESSAGE_MAP()



Component::ModelPanel::ModelPanel()
{
}



Component::ModelPanel::~ModelPanel()
{
}



void Component::ModelPanel::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::ModelPanel::Action action = (Signal::ModelPanel::Action)data.GetInteger(SKW_ACTION);

	switch (action) {
	case Signal::ModelPanel::Action::AddItems:
		AddItems(pData);
		break;

	case Signal::ModelPanel::Action::AddChildren:
		AddChildren(pData);
		break;

	case Signal::ModelPanel::Action::ExpandItem:
		ExpandItem(pData);
		break;

	default:
		DEBUG_STOP;
	}

	REMOVE_POINTER(pData);
}



void Component::ModelPanel::AdjustLayout(int cx, int cy)
{
	__super::AdjustLayout(cx, cy);

	CRect rect = GetBodyRect();
	m_wndControl.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE);
}



int Component::ModelPanel::ConstructHeader(int cx)
{
	m_toolBar.SetPivot(Control::EPivot::TopLeft);
	m_toolBar.Initialize(this);

	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByOriginal);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByAscending);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Sort_ByDescending);
	m_toolBar.AddSeperator();
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Option_GridLines);
	m_toolBar.AddButton(TOOLBAR_3D_CMD_Option_AlternateRows);

	return m_nHeaderHeight = m_toolBar.AdjustLayout().cy;
}



void Component::ModelPanel::ConstructBody()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		TVS_CHECKBOXES | TVS_FULLROWSELECT | TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | TVS_SHOWSELALWAYS;
	if (m_wndControl.Create(dwStyle, GetBodyRect(), this, PRESET::Id) == FALSE) {
		DEBUG_RETURN;
	}

	m_wndControl.SetVisualManagerColorTheme();
	m_wndControl.EnableColumnAutoSize();
	m_wndControl.EnableAlternateRows(FALSE);
	m_wndControl.EnableGridLines(FALSE);
	m_wndControl.ModifyStyle(0, TVS_CHECKBOXES); // EnableCheckBoxes() not working
	//m_wndControl.SetCustomRowHeight(TreeRowHeight());
	m_wndControl.SetSingleSel(FALSE);

	//:WARNING - do not use local string
	BCGP_GRID_FILTERBAR_OPTIONS filter(m_sFilterMessage = Facility::Local(L"Search models...|모델 검색..."));
	filter.m_clrMarkBackground = (COLORREF)Control::EColor::White;
	filter.m_clrMarkText = 0;
	filter.m_bAutoExpandGroups = TRUE;
	filter.m_bIncludeGroups = TRUE;

	m_wndControl.SetOutOfFilterLabel(Facility::Local(L"No items match your search.|일치하는 항목을 찾을 수 없습니다."));
	m_wndControl.EnableFilterBar(TRUE, filter);
	m_wndControl.OnFilterBarUpdate(0);
}



void Component::ModelPanel::OnCommand(UINT id)
{
	if (m_wndControl.GetSafeHwnd() == nullptr) {
		return;
	}

	switch (id) {
	case TOOLBAR_3D_CMD_Sort_ByOriginal:      m_wndControl.RemoveSortColumn(0);     break;
	case TOOLBAR_3D_CMD_Sort_ByAscending:     m_wndControl.SetSortColumn(0, TRUE);  break;
	case TOOLBAR_3D_CMD_Sort_ByDescending:    m_wndControl.SetSortColumn(0, FALSE); break;
	case TOOLBAR_3D_CMD_Option_GridLines:     EnableGridLines();                    break;
	case TOOLBAR_3D_CMD_Option_AlternateRows: EnableAlternateRows();                break;

	default:
		DEBUG_RETURN;
	}

	m_wndControl.AdjustLayout();
	m_wndControl.RedrawWindow();
}

//:WARNING
// m_wndControl.EnableTreeCtrlNotifications(FALSE)
// use 

LRESULT Component::ModelPanel::OnTreeCheckClick(WPARAM wp, LPARAM lp)
{
	DEBUG_STOP;

	CBCGPGridRow* pRow = (CBCGPGridRow*)lp;
	if (pRow == nullptr) {
		return 0;
	}

	if (pRow->HasCheckBox()) {
		//:CHECK
		BOOL checked = !pRow->GetCheck();
		pRow->SetCheck(checked);
		pRow->CheckSubItems(checked);
		pRow->UpdateParentCheckbox(TRUE);

		m_wndControl.RedrawWindow();

		DWORD_PTR key = pRow->GetData();
		m_pView->GetDelivery().modelPanel.OnItemChecked(key, (bool)checked);
	}

	return TRUE; // disable the default implementation
}



void Component::ModelPanel::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//CString text = m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	//POINT pos = pNMTreeView->ptDrag;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	//CString text = pTVDispInfo->item.pszText;
	CEdit* pEdit = (CEdit*)CWnd::FromHandle((HWND)m_wndControl.SendMessage(TVM_GETEDITCONTROL));
	if (pEdit->GetSafeHwnd() != nullptr) {
		pEdit->PostMessage(EM_SETSEL, 0, (LPARAM)-1);
	}

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	UINT flag = 0;
	CPoint point;
	GetCursorPos(&point);
	m_wndControl.ScreenToClient(&point);

	HTREEITEM hItem = m_wndControl.HitTest(point, &flag);
	if (hItem == nullptr) {
		return;
	}

	if (flag & TVHT_NOWHERE) {
		//DEBUG_TRACE(L"NM_CLICK: TVHT_NOWHERE\r\n");
	}
	//else if (flag & TVHT_ONITEMICON) {
	//	DEBUG_TRACE(L"NM_CLICK: TVHT_ONITEMICON\r\n");
	//}
	//else if (flag & TVHT_ONITEMLABEL) {
	//	DEBUG_TRACE(L"NM_CLICK: TVHT_ONITEMLABEL\r\n");
	//}
	//else if (flag & TVHT_ONITEMINDENT) {
	//	DEBUG_TRACE(L"NM_CLICK: TVHT_ONITEMINDENT\r\n");
	//}
	else if (flag & TVHT_ONITEMBUTTON) {
		// clicked expand button
		UINT state = m_wndControl.GetItemState(hItem, TVIS_EXPANDED);
		m_wndControl.Expand(hItem, (state & TVIS_EXPANDED ? TVE_COLLAPSE : TVE_EXPAND));

		//:WARNING - prevent OnTreeSelChanged()
		*pResult = S_FALSE;
	}
	//else if (flag & TVHT_ONITEMRIGHT) {
	//	DEBUG_TRACE(L"NM_CLICK: TVHT_ONITEMRIGHT\r\n");
	//}
	else if (flag & TVHT_ONITEMBUTTON) {
		DEBUG_TRACE(L"NM_CLICK: TVHT_ONITEMBUTTON\r\n");
	}
	else if (flag & TVHT_ONITEMSTATEICON) {
		// clicked check box
		CBCGPGridRow* pRow = m_wndControl.TreeItem(hItem);

		BOOL checked = !pRow->GetCheck();
		pRow->SetCheck(checked);
		pRow->CheckSubItems(checked);
		pRow->UpdateParentCheckbox(TRUE);

		DWORD_PTR key = m_wndControl.GetItemData(hItem);
		m_pView->GetDelivery().modelPanel.OnItemChecked(key, (bool)checked);

		//:WARNING - prevent OnTreeSelChanged()
		*pResult = S_FALSE;
	}
	//else {
	//	DEBUG_TRACE(L"NM_CLICK: other\r\n");
	//}
}



void Component::ModelPanel::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_DBLCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;

	if (m_wndControl.GetItemText(hItem) != PRESET::DummyName) {
		DWORD_PTR key = m_wndControl.GetItemData(hItem);
		m_keyMap.erase(key);
		m_pView->GetDelivery().modelPanel.OnDeleteItem(key);
	}

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	if (pTVDispInfo->item.pszText == nullptr) {
		// canceled
	}
	else {
		//CString text = pTVDispInfo->item.pszText;
	}

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (pNMTreeView->action == TVE_EXPAND) {
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		// get first child item
		HTREEITEM hChild = m_wndControl.GetChildItem(hItem);

		if (m_wndControl.GetItemText(hChild) == PRESET::DummyName) {
			m_wndControl.DeleteItem(hChild);

			DWORD_PTR key = m_wndControl.GetItemData(hItem);
			ASSERT(key != 0);
			m_pView->GetDelivery().modelPanel.OnItemExpanded(key);
		}
	}
	else {
		ASSERT(pNMTreeView->action == TVE_COLLAPSE);
	}

	*pResult = S_OK;
}

//:WARNING - not use

void Component::ModelPanel::OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//if (pNMTreeView->action == TVE_EXPAND) {}
	//else if (pNMTreeView->action == TVE_COLLAPSE) {}
	//else {}

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_RDBLCLICK\r\n");

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//if (pNMTreeView->action == TVC_BYMOUSE) {}
	//else if (pNMTreeView->action == TVC_BYKEYBOARD) {}
	//else {}

	//:CHECK - pNMTreeView->itemOld.hItem == nullptr

	if (pNMTreeView->itemNew.hItem != nullptr) {
		DWORD_PTR key = m_wndControl.GetItemData(pNMTreeView->itemNew.hItem);
		m_pView->GetDelivery().modelPanel.OnSelChanged(key);
	}

	*pResult = S_OK;
}

//:WARNING - not use

void Component::ModelPanel::OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//if (pNMTreeView->action == TVC_BYMOUSE) {}
	//else if (pNMTreeView->action == TVC_BYKEYBOARD) {}
	//else {}

	//:CHECK - pNMTreeView->itemOld.hItem == nullptr

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);
	DEBUG_TRACE(L"NM_SETFOCUS\r\n");

	*pResult = S_OK;
}

//:REF - https://learn.microsoft.com/en-us/windows/win32/api/commctrl/ns-commctrl-tvitemexw
//:REF - https://learn.microsoft.com/en-us/windows/win32/controls/tree-view-control-item-states

void Component::ModelPanel::AddItem(Json::Object* pData)
{
	Json::Object& data = *pData;

	HTREEITEM hCurrent = m_wndControl.InsertItem(data.GetString(SKW_TITLE), GetItem(data.GetDwordPtr(SKW_PARENT)));
	DWORD_PTR key = data.GetDwordPtr(SKW_KEY);
	m_wndControl.SetItemData(hCurrent, key);
	m_keyMap[key] = hCurrent;

	if (data.GetBoolean(SKW_HASCHILDREN)) {
		m_wndControl.InsertItem(PRESET::DummyName, hCurrent);
		m_wndControl.Expand(hCurrent, TVE_COLLAPSE);
	}

	/*
	DWORD_PTR parentKey = data.GetDwordPtr(SKW_PARENT);
	DWORD_PTR key = data.GetDwordPtr(SKW_KEY);
	LPWSTR title = (LPWSTR)(LPCTSTR)data.GetString(SKW_TITLE);
	bool hasChildren = data.GetBoolean(SKW_HASCHILDREN);
	int type = data.GetInteger(SKW_TYPE);

	TVINSERTSTRUCT tvi;
	tvi.hParent = GetItem(parentKey);
	tvi.hInsertAfter = TVI_LAST;

	//:CHECK - item or itemex
	tvi.itemex.pszText = title;
	//:WARNING - is not single flag!!! (combination)
	tvi.itemex.mask = TVIF_TEXT | TVIF_PARAM;
	// TVIF_PARAM: add data. if not set, lParam is not assigned
	tvi.itemex.lParam = (LPARAM)key;
	//:CHECK - how to use tvi.itemex.cChildren?

	HTREEITEM hCurrent = m_wndControl.InsertItem(&tvi);
	m_keyMap[key] = hCurrent;

	if (hasChildren) {
		m_wndControl.InsertItem(PRESET::DummyName, hCurrent);
		m_wndControl.Expand(hCurrent, TVE_COLLAPSE);
	}
	*/
}



void Component::ModelPanel::AddItem(HTREEITEM parent, DWORD_PTR key, LPWSTR title, bool hasChildren, int type)
{
	HTREEITEM hCurrent = m_wndControl.InsertItem(title, parent);
	m_wndControl.SetItemData(hCurrent, key);
	m_keyMap[key] = hCurrent;

	if (hasChildren) {
		m_wndControl.InsertItem(PRESET::DummyName, hCurrent);
		m_wndControl.Expand(hCurrent, TVE_COLLAPSE);
	}

	/*
	TVINSERTSTRUCT tvi;
	tvi.hParent = parent;
	tvi.hInsertAfter = TVI_LAST;

	//:WARNING - is not single flag!!! (combination)
	tvi.itemex.mask = TVIF_TEXT | TVIF_PARAM; // | TVIF_STATE;
	tvi.itemex.pszText = title;
	// TVIF_PARAM: add data. if not set, lParam is not assigned
	tvi.itemex.lParam = (LPARAM)key;
	//tvi.itemex.stateMask = TVIS_EXPANDED;
	//tvi.itemex.state = TVE_EXPAND;

	HTREEITEM hCurrent = m_wndControl.InsertItem(&tvi);
	m_keyMap[key] = hCurrent;

	if (hasChildren) {
		m_wndControl.InsertItem(PRESET::DummyName, hCurrent);
		m_wndControl.Expand(hCurrent, TVE_COLLAPSE);
	}
	*/
}



void Component::ModelPanel::AddItems(Json::Object* pData)
{
	m_wndControl.SetRedraw(FALSE);

	Json::Object& data = *pData;
	Json::Array& items = data.GetArray(SKW_ITEMS);

	for (auto item : items.GetBuffer()) {
		AddItem(item->ToObject());
	}

	m_wndControl.SetRedraw(TRUE);
	m_wndControl.AdjustLayout();
}



void Component::ModelPanel::AddChildren(Json::Object* pData)
{
	m_wndControl.SetRedraw(FALSE);

	Json::Object& data = *pData;
	Json::Array& items = data.GetArray(SKW_CHILDREN);

	HTREEITEM hParent = GetItem(data.GetDwordPtr(SKW_PARENT));
	
	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		AddItem(hParent,
			child.GetDwordPtr(SKW_KEY),
			(LPWSTR)(LPCTSTR)child.GetString(SKW_TITLE),
			child.GetBoolean(SKW_HASCHILDREN),
			child.GetInteger(SKW_TYPE)
		);
	}

	m_wndControl.SetRedraw(TRUE);
	m_wndControl.AdjustLayout();
}



void Component::ModelPanel::ExpandItem(Json::Object* pData)
{
	Json::Object& data = *pData;
	HTREEITEM hItem = GetItem(data.GetDwordPtr(SKW_KEY));
	if (hItem != nullptr) {
		m_wndControl.Expand(hItem, TVE_EXPAND);
	}
	else {
		DEBUG_STOP;
	}
}



void Component::ModelPanel::GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& ancestor)
{
	HTREEITEM pParent = pItem;
	while (pParent != nullptr) {
		ancestor.push_front(m_wndControl.GetItemData(pParent));
		pParent = m_wndControl.GetParentItem(pParent);
	}
}



HTREEITEM Component::ModelPanel::GetItem(DWORD_PTR key)
{
	if (auto result = m_keyMap.find(key); result != m_keyMap.end()) {
		return result->second;
	}
	else {
		return nullptr;
	}
}



void Component::ModelPanel::EnableAlternateRows()
{
	m_bAlternateRows = !m_bAlternateRows;
	m_wndControl.EnableAlternateRows(m_bAlternateRows);
}



void Component::ModelPanel::EnableGridLines()
{
	m_bGridLines = !m_bGridLines;
	m_wndControl.EnableGridLines(m_bGridLines);
}

#undef PRESET
