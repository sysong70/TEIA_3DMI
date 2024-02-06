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



#include "Window.Application.h"
#include <WStr.h>
//#define DEBUG_LOG(s) TheApplication.GetMainFrame().GetDebugTracer().AddLog(s)
#define DEBUG_LOG DEBUG_TRACE

#define DisableNotification(func) \
m_wndControl.EnableTreeCtrlNotifications(FALSE); \
func; \
m_wndControl.EnableTreeCtrlNotifications(TRUE)

#define PRESET PresetModelPanel

namespace PresetModelPanel
{
	const UINT Id = WM_USER;

	WCHAR DummyName[] = L"Expanding...";
}



using namespace Component;

BEGIN_MESSAGE_MAP(ModelPanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnTreeCheckClick)

	//ON_NOTIFY(NM_CLICK, PRESET::Id, OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, PRESET::Id, OnTreeDblClick)
	ON_NOTIFY(NM_RCLICK, PRESET::Id, OnTreeRClick)
	ON_NOTIFY(NM_RDBLCLK, PRESET::Id, OnTreeRDbClick)
	ON_NOTIFY(NM_SETFOCUS, PRESET::Id, OnTreeSetFocus)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Id, OnTreeBeginDrag)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Id, OnTreeBeginLabelEdit)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Id, OnTreeDeleteItem)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Id, OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Id, OnTreeItemExpanded)
	//ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Id, OnTreeItemExpanding)
	ON_NOTIFY(TVN_SELCHANGED, PRESET::Id, OnTreeSelChanged)
	//ON_NOTIFY(TVN_SELCHANGING, PRESET::Id, OnTreeSelChanging)

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
	case Signal::ModelPanel::Action::AddItem:		AddItem(pData);			break;
	case Signal::ModelPanel::Action::AddChildren:	AddChildren(pData);		break;
	case Signal::ModelPanel::Action::CheckItem:		CheckItem(pData);		break;
	case Signal::ModelPanel::Action::CollapseItem:	CollapseItem(pData);	break;
	case Signal::ModelPanel::Action::DeleteItem:	DeleteItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandItem:	ExpandItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandParent:	ExpandParent(pData);	break;

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

	//:TEST - item image
	/*
	CImageList* pImages = new CImageList;
	CBCGPToolBarImages images;
	images.SetImageSize(globalUtils.ScaleByDPI(CSize(16, 16)));

	for (int id = CUSTOM_3D_CMD_KEN_Test1; id <= CUSTOM_3D_CMD_KEN_Test9; id++) {
		CBCGPSVGImage* pImage = new CBCGPSVGImage();
		pImage->Load(id);
		images.AddSVG(pImage);
	}

	images.ExportToImageList(*pImages, TRUE);
	m_wndControl.SetImageList(pImages, TVSIL_NORMAL);
	*/
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

//--------------------------------------------------------------------------------------------------

LRESULT Component::ModelPanel::OnTreeCheckClick(WPARAM wp, LPARAM lp)
{
	CBCGPGridRow* pRow = (CBCGPGridRow*)lp;

	BOOL checked = !pRow->GetCheck();
	pRow->SetCheck(checked);
	pRow->CheckSubItems(checked);
	pRow->UpdateParentCheckbox(TRUE);

	m_wndControl.RedrawWindow();

	DWORD_PTR key = m_wndControl.GetItemData(m_wndControl.TreeItem(pRow));
	m_pView->GetDelivery().modelPanel.OnItemChecked(key, (bool)checked);

	return S_FALSE;
}



void Component::ModelPanel::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//CString text = m_wndControl.GetItemText(pNMTreeView->itemNew.hItem);
	//POINT pos = pNMTreeView->ptDrag;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
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

	DWORD_PTR key = m_wndControl.GetItemData(hItem);
	ASSERT(key != 0);

	if (flag & TVHT_NOWHERE) {
		DEBUG_LOG(L"NM_CLICK: TVHT_NOWHERE");
	}
	else if (flag & TVHT_ONITEMICON) {
		DEBUG_LOG(L"NM_CLICK: TVHT_ONITEMICON");
	}
	else if (flag & TVHT_ONITEMLABEL) {
		DEBUG_LOG(L"NM_CLICK: Label");
	}
	else if (flag & TVHT_ONITEMINDENT) {
		DEBUG_LOG(L"NM_CLICK: TVHT_ONITEMINDENT");
	}
	// Expand button
	else if (flag & TVHT_ONITEMBUTTON) {
		DEBUG_LOG(L"NM_CLICK: Expand button");
	}
	else if (flag & TVHT_ONITEMRIGHT) {
		DEBUG_LOG(L"NM_CLICK: TVHT_ONITEMRIGHT");
	}
	else if (flag & TVHT_ONITEMBUTTON) {
		DEBUG_LOG(L"NM_CLICK: TVHT_ONITEMBUTTON");
	}
	else if (flag & TVHT_ONITEMSTATEICON) {
		DEBUG_LOG(L"NM_CLICK: Check box");
	}
	else {
		DEBUG_LOG(L"NM_CLICK: Other");
	}
}



void Component::ModelPanel::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT flag = 0;
	CPoint point;
	GetCursorPos(&point);
	m_wndControl.ScreenToClient(&point);

	HTREEITEM hItem = m_wndControl.HitTest(point, &flag);
	if (hItem == nullptr) {
		return;
	}

	DWORD_PTR key = m_wndControl.GetItemData(hItem);
	ASSERT(key != 0);

	m_pView->GetDelivery().modelPanel.OnItemDblClicked(key);
	//:CHECK - if S_OK, tree expand the item
	*pResult = S_FALSE;
}



void Component::ModelPanel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;

	DWORD_PTR key = m_wndControl.GetItemData(hItem);
	m_wndControl.DeleteItem(hItem);

	m_keyMap.erase(key);
	m_pView->GetDelivery().modelPanel.OnItemDeleted(key);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
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

		if (m_wndControl.GetItemData(hChild) == 0) {
			ASSERT(m_wndControl.GetItemText(hChild) == PRESET::DummyName);
			DisableNotification(m_wndControl.DeleteItem(hChild));
			m_bExpanding = true;

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

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	//:WARNING - dummy (by keyboard expanding)
	if (hItem == nullptr) {
	}
	else {
		DWORD_PTR key = m_wndControl.GetItemData(hItem);
		m_pView->GetDelivery().modelPanel.OnItemSelected(key);
	}

	*pResult = S_OK;
}

//:WARNING - not use

void Component::ModelPanel::OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	UNREFERENCED_PARAMETER(pNMHDR);

	*pResult = S_OK;
}

//--------------------------------------------------------------------------------------------------

//:REF - https://learn.microsoft.com/en-us/windows/win32/api/commctrl/ns-commctrl-tvitemexw
//:REF - https://learn.microsoft.com/en-us/windows/win32/controls/tree-view-control-item-states

HTREEITEM Component::ModelPanel::AddItem(HTREEITEM parent, DWORD_PTR key, LPWSTR title, bool hasChildren, int type)
{
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

	HTREEITEM hItem = m_wndControl.InsertItem(title, parent);
	DEBUG_VALID(hItem);
	DEBUG_LOG(WStr::Format(L"\t- %s", title));

	//:TEST - no avilable, TVHT_ONITEMLABEL
	//m_wndControl.SetItemImage(hItem, 0, 1);
	m_wndControl.SetItemData(hItem, key);
	m_keyMap[key] = hItem;

	if (hasChildren) {
		m_wndControl.InsertItem(PRESET::DummyName, hItem);
		m_wndControl.Expand(hItem, TVE_COLLAPSE);
	}

	// parent checked state
	if (m_wndControl.GetCheck(parent)) {
		m_wndControl.SetCheck(hItem);
	}

	return hItem;
}



HTREEITEM Component::ModelPanel::AddItem(Json::Object* pData)
{
	/*
	Json::Object& data = *pData;

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

	RedrawTree(false);

	Json::Object& data = *pData;
	DWORD_PTR key = data.GetDwordPtr(SKW_KEY);

	HTREEITEM hItem = m_wndControl.InsertItem(data.GetString(SKW_TITLE), GetItem(data.GetDwordPtr(SKW_PARENT)));
	DEBUG_VALID(hItem);
	DEBUG_LOG(WStr::Format(L"AddItem: %s", m_wndControl.GetItemText(hItem)));

	//:TEST - no available, TVHT_ONITEMLABEL
	//m_wndControl.SetItemNotificationBadge(hItem, L"[HIDE]");
	m_wndControl.SetItemData(hItem, key);
	m_keyMap[key] = hItem;

	if (data.GetBoolean(SKW_HASCHILDREN)) {
		m_wndControl.InsertItem(PRESET::DummyName, hItem);
		m_wndControl.Expand(hItem, TVE_COLLAPSE);
	}

	RedrawTree(true);

	return hItem;
}



void Component::ModelPanel::AddChildren(Json::Object* pData)
{
	RedrawTree(false);

	Json::Object& data = *pData;
	Json::Array& items = data.GetArray(SKW_CHILDREN);

	HTREEITEM hParent = GetItem(data.GetDwordPtr(SKW_PARENT));
	DEBUG_LOG(WStr::Format(L"AddChildren: %s", m_wndControl.GetItemText(hParent)));

	//:WARNING - remove dummy first
	HTREEITEM hChild = m_wndControl.GetChildItem(hParent);
	if (hChild != nullptr && m_wndControl.GetItemData(hChild) == 0) {
		ASSERT(m_wndControl.GetItemText(hChild) == PRESET::DummyName);
		m_wndControl.DeleteItem(hChild);
	}

	BOOL checked = m_wndControl.GetCheck(hParent);

	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		hChild = AddItem(hParent,
			child.GetDwordPtr(SKW_KEY),
			(LPWSTR)(LPCTSTR)child.GetString(SKW_TITLE),
			child.GetBoolean(SKW_HASCHILDREN),
			child.GetInteger(SKW_TYPE)
		);

		// parent checked state
		if (checked) {
			m_wndControl.SetCheck(hChild);
		}
	}

	//:WARNING - select first (keyboard expanding)
	if (m_bExpanding) {
		m_wndControl.SelectItem(m_wndControl.GetChildItem(hParent));
		m_bExpanding = false;
	}

	RedrawTree(true);
}



void Component::ModelPanel::CheckItem(Json::Object* pData)
{
	DisableNotification(
		m_wndControl.SetCheck(GetItem(pData->GetDwordPtr(SKW_KEY)), pData->GetBoolean(SKW_CHECKED))
	);
}



void Component::ModelPanel::CollapseItem(Json::Object* pData)
{
	DisableNotification(
		m_wndControl.Expand(GetItem(pData->GetDwordPtr(SKW_KEY)), TVE_COLLAPSE)
	);
}



void Component::ModelPanel::DeleteItem(Json::Object* pData)
{
	DisableNotification(
		m_wndControl.DeleteItem(GetItem(pData->GetDwordPtr(SKW_KEY)))
	);
}



void Component::ModelPanel::ExpandItem(Json::Object* pData)
{
	DisableNotification(
		m_wndControl.Expand(GetItem(pData->GetDwordPtr(SKW_KEY)), TVE_EXPAND)
	);
}



void Component::ModelPanel::ExpandParent(Json::Object* pData)
{
	RedrawTree(false);

	HTREEITEM hItem = GetItem(pData->GetDwordPtr(SKW_KEY));
	HTREEITEM hParent = hItem;
	std::list<HTREEITEM> ancestor;

	while (hParent != nullptr) {
		DEBUG_LOG(WStr::Format(L"Expand %s", (LPCTSTR)m_wndControl.GetItemText(hParent)));
		ancestor.push_front(hParent);
		hParent = m_wndControl.GetParentItem(hParent);
	}

	//:CHECK - remove last one
	ancestor.pop_back();
	// Expand root to child
	for (auto item : ancestor) {
		ASSERT(m_wndControl.GetItemText(item) != PRESET::DummyName);
		m_wndControl.Expand(item, TVE_EXPAND);
	}

	//:CHECK
	m_wndControl.SelectItem(hItem);

	RedrawTree(true);
}



void Component::ModelPanel::GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& ancestor)
{
	HTREEITEM hParent = pItem;
	while (hParent != nullptr) {
		ancestor.push_front(m_wndControl.GetItemData(hParent));
		hParent = m_wndControl.GetParentItem(hParent);
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



void Component::ModelPanel::RedrawTree(bool value)
{
	m_wndControl.SetRedraw(value);
	m_wndControl.EnableTreeCtrlNotifications(value);

	if (value) {
		m_wndControl.RedrawWindow();
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
#undef DEBUG_LOG
