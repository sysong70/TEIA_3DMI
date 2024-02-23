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
}

//**************************************************************************************************

class ModelTree : public CBCGPTreeCtrlEx
{
public:

	ModelTree()
		: CBCGPTreeCtrlEx()
	{
		m_filterMessage = Facility::Local(L"Search models...|모델 검색...");
		//:WANING - SetOutOfFilterLabel();
		m_strOutOfFilter = Facility::Local(L"No items match your search.|일치하는 항목을 찾을 수 없습니다.");
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
			| TVS_FULLROWSELECT
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

		//:WARNING - do not use local string
		BCGP_GRID_FILTERBAR_OPTIONS filter(m_filterMessage);
		//filter.m_clrMarkBackground = (COLORREF)Control::EColor::White;
		filter.m_clrMarkBackground = RGB(0xA0, 0xA0, 0xA0);
		filter.m_clrMarkText = 0;
		filter.m_bIncludeGroups = TRUE;
		filter.m_bAutoExpandGroups = TRUE;

		EnableFilterBar(TRUE, filter);

		CreateIcons();
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



BEGIN_MESSAGE_MAP(ModelTree, CBCGPGridCtrl)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

//**************************************************************************************************

using namespace Component;

BEGIN_MESSAGE_MAP(ModelPanel, Panel)
	ON_REGISTERED_MESSAGE(BCGM_GRID_ROW_CHECKBOX_CLICK, OnTreeCheckClick)

	//ON_NOTIFY(NM_CLICK, PRESET::Tree, OnTreeClick)
	ON_NOTIFY(NM_DBLCLK, PRESET::Tree, OnTreeDblClick)
	//ON_NOTIFY(NM_RCLICK, PRESET::Tree, OnTreeRClick)
	//ON_NOTIFY(NM_RDBLCLK, PRESET::Tree, OnTreeRDbClick)
	//ON_NOTIFY(NM_SETFOCUS, PRESET::Tree, OnTreeSetFocus)

	ON_NOTIFY(TVN_BEGINDRAG, PRESET::Tree, OnTreeBeginDrag)
	ON_NOTIFY(TVN_BEGINLABELEDIT, PRESET::Tree, OnTreeBeginLabelEdit)
	ON_NOTIFY(TVN_DELETEITEM, PRESET::Tree, OnTreeDeleteItem)
	ON_NOTIFY(TVN_ENDLABELEDIT, PRESET::Tree, OnTreeEndLabelEdit)
	ON_NOTIFY(TVN_ITEMEXPANDED, PRESET::Tree, OnTreeItemExpanded)
	ON_NOTIFY(TVN_ITEMEXPANDING, PRESET::Tree, OnTreeItemExpanding)
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
	case Signal::ModelPanel::Action::DeleteItem:	DeleteItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandItem:	ExpandItem(pData);		break;
	case Signal::ModelPanel::Action::ExpandParent:	ExpandParent(pData);	break;
	case Signal::ModelPanel::Action::SelectItem:	SelectItem(pData);		break;

	case Signal::ModelPanel::Action::RedrawTree:	RedrawTree(pData->GetBoolean(SKW_FLAG)); break;

	default:
		DEBUG_STOP;
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
	DEBUG_LOG(L"* OnTreeCheckClick");
	CBCGPGridRow* pRow = (CBCGPGridRow*)lp;

	BOOL checked = !pRow->GetCheck();
	pRow->SetCheck(checked);
	pRow->CheckSubItems(checked);
	pRow->UpdateParentCheckbox(TRUE);

	Control().RedrawWindow();

	DWORD_PTR key = Control().GetItemData(Control().TreeItem(pRow));
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
	DEBUG_LOG(L"* OnTreeDblClick");

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

	View().GetDelivery().modelPanel.OnItemDblClicked(key);
	//:CHECK - if S_OK, tree expand the item
	*pResult = S_FALSE;
}



void Component::ModelPanel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_LOG(L"* OnTreeDeleteItem");

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemOld.hItem;

	DWORD_PTR key = Control().GetItemData(hItem);
	Control().DeleteItem(hItem);

	m_keyMap.erase(key);
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
		DEBUG_LOG(L"* OnTreeItemExpanded");

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;
		DWORD_PTR key = Control().GetItemData(hItem);
		ASSERT(key != 0);
		//View().GetDelivery().modelPanel.OnItemExpanded(key);
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
	UNREFERENCED_PARAMETER(pNMHDR);

	CPoint point;
	GetCursorPos(&point);
	Control().ScreenToClient(&point);

	CMenu menu;
	menu.CreatePopupMenu();

	menu.AppendMenu(MF_STRING, 111, L"Clear");
	menu.AppendMenu(MF_STRING, 222, L"Save...");

	menu.TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);

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
		DEBUG_LOG(L"* OnTreeSelChanged");

		DWORD_PTR key = Control().GetItemData(hItem);
		View().GetDelivery().modelPanel.OnItemSelected(key);
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
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}

//--------------------------------------------------------------------------------------------------

//:REF - https://learn.microsoft.com/en-us/windows/win32/api/commctrl/ns-commctrl-tvitemexw
//:REF - https://learn.microsoft.com/en-us/windows/win32/controls/tree-view-control-item-states

HTREEITEM Component::ModelPanel::AddItem(HTREEITEM parent, DWORD_PTR key, LPWSTR title, bool checked, int type)
{
	HTREEITEM hItem = Control().InsertItem(title, parent);
	DEBUG_VALID(hItem);
	DEBUG_LOG(WStr::Format(L"\t- %s", title));

	m_keyMap[key] = hItem;

#ifdef _TEST
	Control().SetItemImage(hItem, 0, 0);
#endif
	Control().SetItemData(hItem, key);
	Control().SetCheck(hItem, checked);

	return hItem;
}



HTREEITEM Component::ModelPanel::AddItem(Json::Object* pData)
{
	DWORD_PTR key = pData->GetDwordPtr(SKW_KEY);

	HTREEITEM hItem = Control().InsertItem(pData->GetString(SKW_TITLE), GetItem(pData->GetDwordPtr(SKW_PARENT)));
	DEBUG_VALID(hItem);
	DEBUG_LOG(WStr::Format(L"AddItem: %s", Control().GetItemText(hItem)));

	m_keyMap[key] = hItem;

	Control().SetItemData(hItem, key);
	Control().SetCheck(hItem, pData->GetBoolean(SKW_CHECKED));

	return hItem;
}



void Component::ModelPanel::AddChildren(Json::Object* pData)
{
	Json::Array& items = pData->GetArray(SKW_CHILDREN);

	HTREEITEM hParent = GetItem(pData->GetDwordPtr(SKW_PARENT));
	DEBUG_LOG(WStr::Format(L"AddChildren: %s", Control().GetItemText(hParent)));

	BOOL checked = Control().GetCheck(hParent);

	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		HTREEITEM hChild = AddItem(
			hParent,
			child.GetDwordPtr(SKW_KEY),
			(LPWSTR)(LPCTSTR)child.GetString(SKW_TITLE),
			child.GetBoolean(SKW_CHECKED),
			child.GetInteger(SKW_TYPE)
		);
	}
}



void Component::ModelPanel::CheckItem(Json::Object* pData)
{
	DisableNotification(
		Control().SetCheck(GetItem(pData->GetDwordPtr(SKW_KEY)), pData->GetBoolean(SKW_CHECKED))
	);
}



void Component::ModelPanel::DeleteItem(Json::Object* pData)
{
	DisableNotification(
		Control().DeleteItem(GetItem(pData->GetDwordPtr(SKW_KEY)))
	);
}



void Component::ModelPanel::ExpandItem(Json::Object* pData)
{
	DisableNotification(
		Control().Expand(GetItem(
			pData->GetDwordPtr(SKW_KEY)),
			pData->GetBoolean(SKW_EXPAND) ? TVE_EXPAND : TVE_COLLAPSE
		)
	);
}



void Component::ModelPanel::ExpandParent(Json::Object* pData)
{
	HTREEITEM hItem = GetItem(pData->GetDwordPtr(SKW_KEY));
	DEBUG_VALID(hItem);

	Control().EnsureVisible(hItem);
	DisableNotification(
		Control().SelectItem(hItem)
	);
}



void Component::ModelPanel::SelectItem(Json::Object* pData)
{
	HTREEITEM hItem = GetItem(pData->GetDwordPtr(SKW_KEY));
	DEBUG_VALID(hItem);

	CBCGPGridRow* pRow = Control().TreeItem(hItem);
	pRow->Select(pData->GetDwordPtr(SKW_FLAG));
}

//--------------------------------------------------------------------------------------------------

void Component::ModelPanel::GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& ancestor)
{
	HTREEITEM hParent = pItem;
	while (hParent != nullptr) {
		ancestor.push_front(Control().GetItemData(hParent));
		hParent = Control().GetParentItem(hParent);
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
	Control().SetRedraw(value);
	Control().EnableTreeCtrlNotifications(value);

	if (value) {
		Control().AdjustLayout();
		Control().RedrawWindow();
		EndWaitCursor();
	}
	else {
		BeginWaitCursor();
	}
}

#undef PRESET
#undef DEBUG_LOG
