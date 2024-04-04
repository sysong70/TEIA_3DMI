#include "stdafx.h"
#include "resource.h"
#include "Component.ModelPanel.h"
#include "Facility.h"
#include "Window.Application.h"
#include "Window.View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

//#define _TEST
//#define _LOG

#ifdef _LOG
#define DEBUG_LOG(s) TheApplication.GetMainFrame().GetDebugTracer().AddLog(s)
#else
//#define DEBUG_LOG DEBUG_TRACE
#define DEBUG_LOG
#endif

#define Control() (*m_pControl)
#define FindTreeItem(x) FindItem(pData->GetDwordPtr(x))

#define DisableNotification(func) \
Control().EnableTreeCtrlNotifications(FALSE); \
func; \
Control().EnableTreeCtrlNotifications(TRUE)

//**************************************************************************************************

#define PRESET PresetModelPanel

namespace PresetModelPanel
{
	enum EControlId
	{
		Tree = WM_USER,
		SortButton = TOOLBAR_3D_LST_Sort,
	};

	WCHAR DummyName[] = L"Working...";
}

//**************************************************************************************************

class ModelTree : public CBCGPTreeCtrlEx
{
public:

	ModelTree()
		: CBCGPTreeCtrlEx()
	{
		/*
		m_filterMessage = Facility::Local(L"Search models...|모델 검색...");
		//:WANING - SetOutOfFilterLabel();
		m_strOutOfFilter = Facility::Local(L"No items match your search.|일치하는 항목을 찾을 수 없습니다.");
		*/
	}

public:

	void Initialize(CWnd* pParent, CRect rect)
	{
		//:REF - https://learn.microsoft.com/ko-kr/windows/win32/controls/tree-view-control-window-styles
		DWORD dwStyle = WS_CHILD | WS_VISIBLE
			/// Enables check boxes for items in a tree - view control.
			| TVS_CHECKBOXES
			/// Prevents the tree-view control from sending TVN_BEGINDRAG notification codes.
			//| TVS_DISABLEDRAGDROP
			/// Allows the user to edit the labels of tree - view items.
			//| TVM_EDITLABEL
			/// Enables full-row selection in the tree view.
			/// This style cannot be used in conjunction with the TVS_HASLINES style.
			//| TVS_FULLROWSELECT
			/// Displays plus (+) and minus (-) buttons next to parent items.
			/// To include buttons with items at the root of the tree view, TVS_LINESATROOT must also be specified.
			| TVS_HASBUTTONS
			/// Uses lines to show the hierarchy of items.
			| TVS_HASLINES
			/// Obtains tooltip information by sending the TVN_GETINFOTIP notification.
			//| TVS_INFOTIP
			/// Uses lines to link items at the root of the tree-view control.
			/// This value is ignored if TVS_HASLINES is not also specified.
			| TVS_LINESATROOT
			/// Disables horizontal scrolling in the control.
			/// The control will not display any horizontal scroll bars.
			//| TVS_NOHSCROLL
			/// Disables tooltips.
			//| TVS_NOTOOLTIPS
			/// Causes text to be displayed from right-to-left (RTL).
			//| TVS_RTLREADING
			/// Causes a selected item to remain selected when the tree-view control loses focus.
			| TVS_SHOWSELALWAYS
			/// Causes the item being selected to expand and the item being unselected to collapse upon selection in the tree view.
			/// If the user holds down the CTRL key while selecting an item, the item being unselected will not be collapsed.
			//| TVS_SINGLEEXPAND
			/// Enables hot tracking in a tree-view control.
			//| TVS_TRACKSELECT
		;

		if (Create(dwStyle, rect, pParent, PRESET::Tree) == FALSE) {
			DEBUG_RETURN;
		}

		SetVisualManagerColorTheme();
		EnableDragSelection(FALSE);
		SetSingleSel(FALSE);
		
		//:CHECK
		//SetShowInPlaceToolTip(FALSE);

		/*
		//:WARNING - do not use local string
		BCGP_GRID_FILTERBAR_OPTIONS filter(m_filterMessage);
		//filter.m_clrMarkBackground = (COLORREF)Control::EColor::White;
		filter.m_clrMarkBackground = RGB(0xA0, 0xA0, 0xA0);
		filter.m_clrMarkText = 0;
		filter.m_bIncludeGroups = TRUE;
		filter.m_bAutoExpandGroups = TRUE;

		EnableFilterBar(TRUE, filter);

		CreateIcons();
		*/
	}



	HTREEITEM CustomHitTest(UINT& flag)
	{
	#ifdef _TEST
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);

		HTREEITEM hItem = HitTest(point, &flag);

		//:WARING - check image clicked(row has check box and image)
		if (hItem != nullptr) {
			CBCGPGridRow* pRow = TreeItem(hItem);
			ASSERT(pRow != nullptr && pRow->HasCheckBox());

			int dx = GetHierarchyOffset(pRow);
			CRect check = pRow->GetCheckBoxRect(dx);

			int margin = globalUtils.ScaleByDPI(GetImageMargin());
			check.left = check.right + margin;
			check.right = check.left + GetScaledImageSize().cx + margin;

			//:CHECK
			if (check.PtInRect(point)) {
				flag = TVHT_ONITEMICON;
			}
		}

		return hItem;
	#else
		RETURN_NULL;
	#endif
	}



	void AddDummyItem(HTREEITEM hParent, bool checked)
	{
		HTREEITEM hDummy = InsertItem(PRESET::DummyName, hParent);
		SetCheck(hDummy, checked);
		Expand(hParent, TVE_COLLAPSE);
	}



	bool RemoveDummyItem(HTREEITEM hParent)
	{
		// Check first child item
		HTREEITEM hChild = GetChildItem(hParent);
		if (GetItemText(hChild) == PRESET::DummyName) {
			CLocalState<BOOL> notify(m_bTreeCtrlNotifications, FALSE);
			DeleteItem(hChild);

			return true;
		}

		return false;
	}



	HTREEITEM AddItem(HTREEITEM hParent, DWORD_PTR key, LPWSTR title, bool checked, bool hasChildren, int type)
	{
		HTREEITEM hItem = InsertItem(title, hParent);
		DEBUG_VALID(hItem);
		DEBUG_LOG(WStr::Format(L"\t- %s", title));

	#ifdef _TEST
		SetItemImage(hItem, 0, 0);
	#endif
		SetItemData(hItem, key);
		SetCheck(hItem, checked);

		if (hasChildren) {
			AddDummyItem(hItem, checked);
		}

		return hItem;
	}



	void ClearSelection()
	{
		CList<CBCGPGridItem*, CBCGPGridItem*> selected;
		GetSelectedItems(selected);

		POSITION pos = selected.GetHeadPosition();
		while (pos != NULL) {
			CBCGPGridItem* pItem = selected.GetNext(pos);
			if (pItem != nullptr) {
				pItem->Select(FALSE);
			}
		}
	}



	void InverseCheckedStatus()
	{
		CLocalState<BOOL> notify(m_bTreeCtrlNotifications, FALSE);

		//:CHECK
		const CList<CBCGPGridRow*, CBCGPGridRow*>& list = m_lstItems;

		POSITION pos = list.GetHeadPosition();
		while (pos != nullptr) {
			auto item = list.GetNext(pos);
			item->ToggleCheck();
			item->UpdateParentCheckbox();
		}

		RedrawWindow();
	}



	void GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& ancestor)
	{
		HTREEITEM hParent = pItem;
		while (hParent != nullptr) {
			ancestor.push_front(GetItemData(hParent));
			hParent = GetParentItem(hParent);
		}
	}



	void Redraw(bool value)
	{
		SetRedraw(value);
		EnableTreeCtrlNotifications(value);

		if (value) {
			AdjustLayout();
			RedrawWindow();
			EndWaitCursor();
		}
		else {
			BeginWaitCursor();
		}
	}

protected:

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point)
	{
	#ifdef _TEST
	#define AddMenu(id) AppendMenu(MF_STRING, id, Facility::GetTitle(id))

		CMenu menu;
		menu.CreatePopupMenu();

		menu.AddMenu(HOME_3D_CMD_Zoom_Object);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AddMenu(HOME_3D_CMD_Visualize_ShowAll);
		menu.AddMenu(HOME_3D_CMD_Visualize_Hide);
		menu.AddMenu(HOME_3D_CMD_Visualize_ShowOnly);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AddMenu(HOME_3D_CMD_Visualize_Toggle);

		UINT id = TheApplication.GetContextMenuManager()->TrackPopupMenu(menu.Detach(), point.x, point.y, this);
		((Component::ModelPanel*)GetParent())->GetView().SendMessage(WM_COMMAND, (WPARAM)id);

	#undef AddMenu
	#endif
	}



	DECLARE_MESSAGE_MAP()

private:

	CString m_filterMessage;

private:

	void CreateIcons()
	{
	#ifdef _TEST
		CImageList* pImages = new CImageList;
		CBCGPToolBarImages images;
		images.SetImageSize(GetScaledImageSize());

		for (int id = CUSTOM_3D_CMD_KEN_Test1; id <= CUSTOM_3D_CMD_KEN_Test9; id++) {
			CBCGPSVGImage* pImage = new CBCGPSVGImage();
			pImage->Load(id);
			images.AddSVG(pImage);
		}

		images.ExportToImageList(*pImages, TRUE);
		SetImageList(pImages, TVSIL_NORMAL);
	#endif
	}



	CSize GetScaledImageSize()
	{
		//:WARNING - 20, maximum height
		return globalUtils.ScaleByDPI(CSize(20, 20));
	}
};



BEGIN_MESSAGE_MAP(ModelTree, CBCGPTreeCtrlEx)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//**************************************************************************************************

using namespace Component;

BEGIN_MESSAGE_MAP(ModelPanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnTreeCheckClick)

	//ON_NOTIFY(NM_CLICK, PRESET::Tree, OnTreeClick)
	//ON_NOTIFY(NM_DBLCLK, PRESET::Tree, OnTreeDblClick)
	//ON_NOTIFY(NM_RCLICK, PRESET::Tree, OnTreeRClick)
	//ON_NOTIFY(NM_RDBLCLK, PRESET::Tree, OnTreeRDbClick)
	//ON_NOTIFY(NM_SETFOCUS, PRESET::Tree, OnTreeSetFocus)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Tree, OnTreeBeginDrag)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Tree, OnTreeBeginLabelEdit)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Tree, OnTreeDeleteItem)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Tree, OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Tree, OnTreeItemExpanded)
	//ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Tree, OnTreeItemExpanding)
	ON_NOTIFY(TVN_SELCHANGED, PRESET::Tree, OnTreeSelChanged)
	//ON_NOTIFY(TVN_SELCHANGING, PRESET::Tree, OnTreeSelChanging)

	//ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_BN_CLICKED(PRESET::SortButton, OnCommandSort)
END_MESSAGE_MAP()



Component::ModelPanel::ModelPanel()
{
	m_pControl = new ModelTree();
}



Component::ModelPanel::~ModelPanel()
{
	REMOVE_POINTER(m_pControl);
}



void Component::ModelPanel::ReceiveSignal(Json::Object* pData)
{
	Json::Object& data = *pData;
	Signal::ModelPanel::Action action = (Signal::ModelPanel::Action)data.GetInteger(SKW_ACTION);

	switch (action) {
	case Signal::ModelPanel::Action::AddItem:		AddItem(pData);			break;
	case Signal::ModelPanel::Action::AddChildren:	AddChildren(pData);		break;
	case Signal::ModelPanel::Action::CheckItem:		CheckItem(pData);		break;
	case Signal::ModelPanel::Action::CheckItems:	CheckItems(pData);		break;
	case Signal::ModelPanel::Action::DeleteItem:	DeleteItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandItem:	ExpandItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandParent:	ExpandParent(pData);	break;
	case Signal::ModelPanel::Action::SelectItem:	SelectItem(pData);		break;
	case Signal::ModelPanel::Action::SelectItems:	SelectItems(pData);		break;

	case Signal::ModelPanel::Action::InverseCheckedStatus:
		Control().InverseCheckedStatus();
		break;

	case Signal::ModelPanel::Action::RedrawTree:
		Control().Redraw(pData->GetBoolean(SKW_FLAG));
		break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void Component::ModelPanel::AdjustLayout(int cx, int cy)
{
	__super::AdjustLayout(cx, cy);

	CRect rect = GetBodyRect();
	Control().SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE);
}



int Component::ModelPanel::ConstructHeader(int cx)
{
	ToolBar().SetPivot(Control::EPivot::TopLeft);
	ToolBar().Initialize(this);
	ToolBar().AddButtonWithMenu(PRESET::SortButton, {
		TOOLBAR_3D_CMD_Sort_ByOriginal,
		TOOLBAR_3D_CMD_Sort_ByAscending,
		TOOLBAR_3D_CMD_Sort_ByDescending
	});
	ToolBar().CheckMenu(PRESET::SortButton, TOOLBAR_3D_CMD_Sort_ByOriginal, true);

	return m_nHeaderHeight = ToolBar().AdjustLayout().cy;
}



void Component::ModelPanel::ConstructBody()
{
	Control().Initialize(this, GetBodyRect());
}



void Component::ModelPanel::OnCommandSort()
{
	UINT id = ToolBar().GetMenuResult(PRESET::SortButton);

	switch (id) {
	case TOOLBAR_3D_CMD_Sort_ByOriginal:
		Control().RemoveSortColumn(0);
		break;

	case TOOLBAR_3D_CMD_Sort_ByAscending:
		Control().SetSortColumn(0, TRUE);
		break;

	case TOOLBAR_3D_CMD_Sort_ByDescending:
		Control().SetSortColumn(0, FALSE);
		break;

	default:
		DEBUG_RETURN;
	}

	ToolBar().CheckMenu(PRESET::SortButton, id, true);
	ToolBar().UpdateWindow();

	Control().AdjustLayout();
	Control().RedrawWindow();
	Control().SetFocus();
}

//--------------------------------------------------------------------------------------------------

LRESULT Component::ModelPanel::OnTreeCheckClick(WPARAM wp, LPARAM lp)
{
	CBCGPGridRow* pRow = (CBCGPGridRow*)lp;

	BOOL checked = !pRow->GetCheck();
	pRow->SetCheck(checked);
	pRow->CheckSubItems(checked);
	pRow->UpdateParentCheckbox(TRUE);

	Control().RedrawWindow();

	DWORD_PTR key = Control().GetItemData(Control().TreeItem(pRow));
	ASSERT(key != 0);

	DEBUG_LOG(L"* OnItemChecked activated");
	View().GetDelivery().modelPanel.OnItemChecked(key, (bool)checked);

	return S_FALSE;
}



void Component::ModelPanel::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//CString text = Control().GetItemText(pNMTreeView->itemNew.hItem);
	//POINT pos = pNMTreeView->ptDrag;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	//CString text = pTVDispInfo->item.pszText;
	CEdit* pEdit = (CEdit*)CWnd::FromHandle((HWND)Control().SendMessage(TVM_GETEDITCONTROL));
	if (pEdit->GetSafeHwnd() != nullptr) {
		pEdit->PostMessage(EM_SETSEL, 0, (LPARAM)-1);
	}

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	UINT flag = 0;
#ifdef _TEST
	//:WARNING - TVHT_ONITEMICON no avialable in CBCGPTreeCtrlEx! only TVHT_ONITEMLABEL
	HTREEITEM hItem = Control().CustomHitTest(flag);
#else
	HTREEITEM hItem = Control().HitTest(flag);
#endif

	if (hItem != nullptr) {
		DWORD_PTR key = Control().GetItemData(hItem);
		ASSERT(key != 0);
	}

	if (flag & TVHT_NOWHERE) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_NOWHERE");
	}
	if (flag & TVHT_ONITEMICON) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMICON");
#ifdef _TEST
		int image;
		BOOL success = Control().GetItemImage(hItem, image, image);
		ASSERT(success);

		image = (image == 0 ? 1 : 0);
		Control().SetItemImage(hItem, image, image);
#endif
	}
	if (flag & TVHT_ONITEMLABEL) {
		DEBUG_LOG(L"* OnTreeClick: Label");
	}
	if (flag & TVHT_ONITEMINDENT) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMINDENT");
	}
	if (flag & TVHT_ONITEMBUTTON) {
		DEBUG_LOG(L"* OnTreeClick: Expand button");
	}
	if (flag & TVHT_ONITEMRIGHT) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMRIGHT");
	}
	if (flag & TVHT_ONITEMSTATEICON) {
		DEBUG_LOG(L"* OnTreeClick: Check box");
	}

	if (flag & TVHT_ABOVE) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_ABOVE");
	}
	if (flag & TVHT_BELOW) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_BELOW");
	}
	if (flag & TVHT_TORIGHT) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_TORIGHT");
	}
	if (flag & TVHT_TOLEFT) {
		DEBUG_LOG(L"* OnTreeClick: TVHT_TOLEFT");
	}
}



void Component::ModelPanel::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT flag = 0;
#ifdef _TEST
	HTREEITEM hItem = Control().CustomHitTest(flag);
#else
	HTREEITEM hItem = Control().HitTest(flag);
#endif

	if (hItem == nullptr) {
		return;
	}

	DWORD_PTR key = Control().GetItemData(hItem);
	ASSERT(key != 0);

	DEBUG_LOG(L"* OnItemDblClicked activated");
	View().GetDelivery().modelPanel.OnItemDblClicked(key);
	//:CHECK - if S_OK, tree expand the item
	*pResult = S_FALSE;
}



void Component::ModelPanel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;

	DWORD_PTR key = Control().GetItemData(hItem);

	Control().DeleteItem(hItem);
	m_keyMap.erase(key);

	DEBUG_LOG(L"* OnItemDeleted activated");
	View().GetDelivery().modelPanel.OnItemDeleted(key);

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

		if (Control().RemoveDummyItem(hItem)) {
			DWORD_PTR key = Control().GetItemData(hItem);
			ASSERT(key != 0);

			DEBUG_LOG(L"* OnItemExpanded activated");
			View().GetDelivery().modelPanel.OnItemExpanded(key);
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
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	//:WARNING - range selection, skip signal
	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		*pResult = S_OK;
		return;
	}
	//:WARNING - multi selection
	if (::GetAsyncKeyState(VK_CONTROL) & 0x8080) {
		//:TODO - how
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr) {
	}
	else {
		CPoint point;
		::GetCursorPos(&point);
		Control().ScreenToClient(&point);

		UINT flag = 0;
		HTREEITEM hSelected = Control().HitTest(point, &flag);

		if (flag & TVHT_ONITEMBUTTON) {
			//:WARNING - expand box clicked, skip signal
		}
		else {
			DWORD_PTR key = Control().GetItemData(hItem);
			ASSERT(key != 0);

			DEBUG_LOG(L"* OnItemSelected activated");
			View().GetDelivery().modelPanel.OnItemSelected(key);
		}
	}

	*pResult = S_OK;
}

//:WARNING - not use

void Component::ModelPanel::OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void Component::ModelPanel::OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}

//--------------------------------------------------------------------------------------------------

HTREEITEM Component::ModelPanel::AddItem(Json::Object* pData)
{
	DWORD_PTR key = pData->GetDwordPtr(SKW_KEY);
	bool checked = pData->GetBoolean(SKW_CHECKED);

	HTREEITEM hItem = Control().InsertItem(pData->GetString(SKW_TITLE), FindTreeItem(SKW_PARENT));
	DEBUG_VALID(hItem);
	DEBUG_LOG(WStr::Format(L"AddItem: %s", Control().GetItemText(hItem)));

	m_keyMap[key] = hItem;

	Control().SetItemData(hItem, key);
	Control().SetCheck(hItem, checked);

	if (pData->GetBoolean(SKW_HASCHILDREN)) {
		Control().AddDummyItem(hItem, checked);
	}

	return hItem;
}



void Component::ModelPanel::AddChildren(Json::Object* pData)
{
	Json::Array& items = pData->GetArray(SKW_CHILDREN);

	HTREEITEM hParent = FindTreeItem(SKW_PARENT);
	DEBUG_LOG(WStr::Format(L"AddChildren: %s", Control().GetItemText(hParent)));

	Control().RemoveDummyItem(hParent);

	int childCount = 0;
	int checkedCount = 0;
	HTREEITEM hChild = nullptr;

	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		DWORD_PTR key = child.GetDwordPtr(SKW_KEY);
		bool checked = child.GetBoolean(SKW_CHECKED);
		childCount++;
		checkedCount += checked ? 1 : 0;

		hChild = Control().AddItem(
			hParent,
			key,
			(LPWSTR)(LPCTSTR)child.GetString(SKW_TITLE),
			checked,
			child.GetBoolean(SKW_HASCHILDREN),
			child.GetInteger(SKW_TYPE)
		);

		m_keyMap[key] = hChild;
	}

	// Use last item
	if (childCount != checkedCount) {
		Control().TreeItem(hChild)->UpdateParentCheckbox();
	}

	if (pData->GetBoolean(SKW_EXPAND) == false) {
		Control().Expand(hParent, TVE_COLLAPSE);
	}
}



void Component::ModelPanel::CheckItem(Json::Object* pData)
{
	DisableNotification(
		Control().SetCheck(FindTreeItem(SKW_KEY), pData->GetBoolean(SKW_CHECKED))
	);
}



void Component::ModelPanel::CheckItems(Json::Object* pData)
{
	Control().Redraw(false);

	if (pData->FindValue(SKW_CHECKED) == nullptr) {
		// Multi-purpose
		Json::Array& items = pData->GetArray(SKW_ITEMS);

		HTREEITEM hItem = nullptr;
		for (auto item : items.GetBuffer()) {
			Json::Object& node = item->AsObject();

			bool check = node.GetBoolean(SKW_FLAG);
			hItem = FindItem(node.GetDwordPtr(SKW_KEY));
			DEBUG_VALID(hItem);

			Control().SetCheck(hItem, check);
			Control().TreeItem(hItem)->UpdateParentCheckbox();
		}
	}
	else {
		// Single status
		bool check = pData->GetBoolean(SKW_CHECKED);

		Signal::KeyItems keys;
		pData->GetArray(SKW_ITEMS).ToArray(keys);

		HTREEITEM hItem = nullptr;
		for (auto key : keys) {
			hItem = FindItem(key);
			DEBUG_VALID(hItem);

			Control().SetCheck(hItem, check);
			Control().TreeItem(hItem)->UpdateParentCheckbox();
		}
	}

	Control().Redraw(true);
}



void Component::ModelPanel::DeleteItem(Json::Object* pData)
{
	DisableNotification(
		Control().DeleteItem(FindTreeItem(SKW_KEY))
	);
}



void Component::ModelPanel::ExpandItem(Json::Object* pData)
{
	DisableNotification(
		Control().Expand(FindTreeItem(SKW_KEY), pData->GetBoolean(SKW_EXPAND) ? TVE_EXPAND : TVE_COLLAPSE)
	);
}



void Component::ModelPanel::ExpandParent(Json::Object* pData)
{
	HTREEITEM hItem = FindTreeItem(SKW_KEY);
	DEBUG_VALID(hItem);

	Control().EnsureVisible(hItem);
	DisableNotification(
		Control().SelectItem(hItem)
	);
}



void Component::ModelPanel::SelectItem(Json::Object* pData)
{
	HTREEITEM hItem = FindTreeItem(SKW_KEY);
	DEBUG_VALID(hItem);

	//:TODO - uncheck
	bool selected = pData->GetBoolean(SKW_FLAG);
	ASSERT(selected);

	DisableNotification(
		Control().SelectItem(hItem)
	);

	Control().RedrawWindow();
}



void Component::ModelPanel::SelectItems(Json::Object* pData)
{
	Control().ClearSelection();

	//:TODO - uncheck
	bool checked = pData->GetBoolean(SKW_FLAG);
	ASSERT(checked);

	Signal::KeyItems keys;
	pData->GetArray(SKW_ITEMS).ToArray(keys);

	Control().Redraw(false);

	HTREEITEM hItem = nullptr;
	for (auto key : keys) {
		hItem = FindItem(key);
		DEBUG_VALID(hItem);

		Control().SelectItem(hItem);
	}

	Control().Redraw(true);
}

//--------------------------------------------------------------------------------------------------

HTREEITEM Component::ModelPanel::FindItem(DWORD_PTR key)
{
	if (auto result = m_keyMap.find(key); result != m_keyMap.end()) {
		return result->second;
	}
	else {
		return nullptr;
	}
}

#undef PRESET
#undef DEBUG_LOG
