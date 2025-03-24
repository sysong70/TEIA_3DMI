#include "stdafx.h"

#include "Ctl.Panel3dModel.h"
#include "Wnd.View.h"

#include <Signal3d.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

//#define _TEST
//#define _LOG

#ifdef _LOG
#define DEBUG_LOG(s) TheApp.GetMainFrame().GetDebugTracer().AddLog(s)
#else
//#define DEBUG_LOG DEBUG_TRACE
#define DEBUG_LOG
#endif

#define Control					(*TreeCtl)
#define FindTreeItem(x)			Control.FindItem(pData->GetDwordPtr(x))
#define DisableNotification()	CLocalState<bool> disableNofify(DisableNofify, true)
#define theDelivery				(*(ViewWnd->GetDelivery3d()))



namespace
{
	enum class Id
	{
		Tree = WM_USER,
		SortButton = TOOLBAR_3D_LST_Sort,
	};
}

//**************************************************************************************************

class CtlModelTree : public CBCGPTreeCtrl
{
public:

	enum ESort
	{
		Original,
		Ascending,
		Descending,
	};

	struct ItemData
	{
		DWORD_PTR Key = NULL;
		// Dummy index for sorting
		int Index = 0;
	};

	std::unordered_map<DWORD_PTR, HTREEITEM> KeyMap;
	std::vector<ItemData*> Items;
	ESort SortMethod = Original;
	PFNTVCOMPARE CompareFunc = OriginalSort;

public:

	CtlModelTree()
		: CBCGPTreeCtrl()
	{}

	virtual ~CtlModelTree()
	{
		for (auto item : Items) {
			delete item;
		}
		Items.clear();

		DestroyWindow();
	}

public:

	bool Initialize(CWnd* pParentWnd, CRect rect)
	{
		// REF - https://learn.microsoft.com/ko-kr/windows/win32/controls/tree-view-control-window-styles
		DWORD dwStyle = WS_CHILD | WS_VISIBLE
			/// Enables check boxes for items in a tree - view control.
			| TVS_CHECKBOXES
			/// Prevents the tree-view control from sending TVN_BEGINDRAG notification codes.
			| TVS_DISABLEDRAGDROP
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
			| TVS_TRACKSELECT
		;

		if (Create(dwStyle, rect, pParentWnd, (UINT)Id::Tree) == FALSE) {
			RETURN_FALSE;
		}

		m_bVisualManagerStyle = TRUE;
		m_bThemedInplaceTooltip = TRUE;
		// WARNING - special case for CBCGPTreeCtrl(CTreeCtrl)
		globalData.SetWindowTheme(this, L"DarkMode_Explorer", NULL);

		SetBkColor(CBCGPVisualManager::GetInstance()->GetTreeControlFillColor(this));
		SetItemHeight(globalUtils.ScaleByDPI(22));

		return true;
	}

	HTREEITEM AddItem(HTREEITEM hParent, DWORD_PTR key, int index, LPWSTR title, bool checked, bool hasChildren, int type)
	{
		ItemData* pData = new ItemData{ key, index };
		Items.push_back(pData);

		TVINSERTSTRUCT tvs;

		tvs.hParent = hParent;
		tvs.hInsertAfter = TVI_LAST;
		tvs.item.mask = TVIF_TEXT | TVIS_EXPANDED | TVIF_PARAM;
		if (hasChildren) {
			tvs.item.mask |= TVIF_CHILDREN;
			tvs.item.cChildren = 1;
		}

		tvs.item.pszText = title;
		tvs.item.lParam = (DWORD_PTR)pData;

		HTREEITEM hItem = InsertItem(&tvs);
		KeyMap[key] = hItem;

		SetCheck(hItem, checked);

		return hItem;
	}

	HTREEITEM FindItem(DWORD_PTR key)
	{
		auto result = KeyMap.find(key);
		if (result != KeyMap.end()) {
			return result->second;
		}
		else {
			return nullptr;
		}
	}

	DWORD_PTR GetItemKey(HTREEITEM hItem)
	{
		ItemData* pData = (ItemData*)GetItemData(hItem);
		if (pData != nullptr) {
			return pData->Key;
		}
		else {
			DEBUG_STOP;
			return NULL;
		}
	}

	HTREEITEM HitFlag(UINT& flag) const
	{
		CPoint point;
		::GetCursorPos(&point);
		ScreenToClient(&point);

		return HitTest(point, &flag);
	}

	void InverseCheckedStatus()
	{
		// TODO
		DEBUG_STOP;
	}

	// from Root to last
	// return value - init(-1), off(0), on(1), mixed(2)

	int UpdateCheckState(HTREEITEM hItem)
	{
		std::vector<BOOL> result;

		HTREEITEM hNextItem = GetChildItem(hItem);
		while (hNextItem != nullptr) {
			if (ItemHasChildren(hNextItem)) {
				int value = UpdateCheckState(hNextItem);

				switch (value) {
				case 0:
					SetCheck(hNextItem, FALSE);
					break;

				case 1:
				case 2:
					SetCheck(hNextItem, TRUE);
					break;

				default:
					break;
				}
			}

			result.push_back(GetCheck(hNextItem));
			hNextItem = GetNextSiblingItem(hNextItem);
		}

		if (result.size() == 0) {
			return GetCheck(hItem);
		}
		else {
			BOOL checked = result.front();
			for (auto value : result) {
				if (checked != value) {
					return 2;
				}
			}

			SetCheck(hItem, checked);
			return checked;
		}
	}

	void UpdateChildrenCheckState(HTREEITEM hItem, BOOL checked)
	{
		HTREEITEM hNextItem = GetChildItem(hItem);
		while (hNextItem != nullptr) {
			SetCheck(hNextItem, checked);
			if (ItemHasChildren(hNextItem)) {
				UpdateChildrenCheckState(hNextItem, checked);
			}
			hNextItem = GetNextSiblingItem(hNextItem);
		}
	}

	void GetAncestorData(HTREEITEM hItem, std::list<DWORD_PTR>& ancestor)
	{
		HTREEITEM hParent = hItem;
		while (hParent != nullptr) {
			ancestor.push_front(GetItemData(hParent));
			hParent = GetParentItem(hParent);
		}
	}

	void Redraw(bool value)
	{
		SetRedraw(value);

		if (value) {
			EndWaitCursor();
			UpdateWindow();
		}
		else {
			BeginWaitCursor();
		}
	}

	void Sort(ESort eMethod)
	{
		if (SortMethod == eMethod) {
			return;
		}
		SortMethod = eMethod;

		switch (SortMethod) {
		case Original:		CompareFunc = OriginalSort;	break;
		case Ascending:		CompareFunc = AscendingSort;	break;
		case Descending:	CompareFunc = DescendingSort; break;

		default:
			DEBUG_STOP;
			return;
		}

		Sort(GetRootItem());
	}

	void Sort(HTREEITEM hItem)
	{
		if (!ItemHasChildren(hItem)) {
			return;
		}

		TVSORTCB tvs;
		tvs.hParent = hItem;
		tvs.lpfnCompare = CompareFunc;
		tvs.lParam = (LPARAM)this;

		SortChildrenCB(&tvs);

		HTREEITEM hNextItem = GetChildItem(hItem);
		while (hNextItem != nullptr) {
			Sort(hNextItem);
			hNextItem = GetNextSiblingItem(hNextItem);
		}
	}

	ESort GetSortMethod()
	{
		return SortMethod;
	}

public:

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point)
	{
	#ifdef _TEST
	#define AddMenu(id) AppendMenu(MF_STRING, id, Ast::GetTitle(id))

		CMenu menu;
		menu.CreatePopupMenu();

		menu.AddMenu(HOME_3D_CMD_Zoom_Object);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AddMenu(HOME_3D_CMD_Visualize_ShowAll);
		menu.AddMenu(HOME_3D_CMD_Visualize_Hide);
		menu.AddMenu(HOME_3D_CMD_Visualize_ShowOnly);
		menu.AppendMenu(MF_SEPARATOR);
		menu.AddMenu(HOME_3D_CMD_Visualize_Toggle);

		UINT id = TheApp.GetContextMenuManager()->TrackPopupMenu(menu.Detach(), point.x, point.y, this);
		((CtlPanel3dModel*)GetParent())->GetView().SendMessage(WM_COMMAND, (WPARAM)id);

	#undef AddMenu
	#endif
	}

	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
	{
		if (m_bVisualManagerStyle && !m_bImageListStateChanged && (GetStyle() & TVS_CHECKBOXES) == TVS_CHECKBOXES) {
			CImageList* pImageList = GetImageList(TVSIL_STATE);
			if (m_ilState.GetSafeHandle() == NULL && pImageList != NULL) {
				int cx = 0;
				int cy = 0;
				if (ImageList_GetIconSize(pImageList->GetSafeHandle(), &cx, &cy)) {
					if (cx != 0 && cy != 0) {
						m_ilState.Create(cx, cy, ILC_COLOR32, 2, 0);

						SetImageList(&m_ilState, TVSIL_STATE);

						UpdateImageListState();
					}
				}
			}
		}

		LPNMTVCUSTOMDRAW pNMTVCD = (LPNMTVCUSTOMDRAW)pNMHDR;
		if (pNMTVCD == NULL) {
			*pResult = -1;
			return;
		}

		*pResult = CDRF_DODEFAULT;

		if (!m_bVisualManagerStyle) {
			return;
		}

		if (pNMTVCD->nmcd.dwDrawStage == CDDS_PREPAINT) {
			*pResult = CDRF_NOTIFYPOSTPAINT | CDRF_NOTIFYITEMDRAW;
		}
		else if (pNMTVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
			BOOL bIsSelected = FALSE;
			BOOL bIsFocused = FALSE;
			BOOL bIsDisabled = FALSE;

			COLORREF clrBk = CBCGPVisualManager::GetInstance()->GetTreeControlFillColor(this, bIsSelected, bIsFocused, bIsDisabled);
			COLORREF clrText = CBCGPVisualManager::GetInstance()->GetTreeControlTextColor(this, bIsSelected, bIsFocused, bIsDisabled);

			if (!IsWindowEnabled()) {
				bIsDisabled = TRUE;
			}
			else {
				UINT nState = GetItemState((HTREEITEM)pNMTVCD->nmcd.dwItemSpec, TVIF_STATE);
				if ((nState & (TVIS_SELECTED | TVIS_DROPHILITED)) != 0) {
					if (GetFocus() != this && (nState & TVIS_DROPHILITED) == 0) {
						clrText = (COLORREF)Ctl::EColor::DeepSkyBlue;
					}
				}
			}

			if (clrText != (COLORREF)-1) {
				pNMTVCD->clrText = clrText;
			}
		}
	}

	DECLARE_MESSAGE_MAP()

public:

	// WARNING - lParam1(and lParm2) is ItemData(not HTREEITEM)
	// REF - https://learn.microsoft.com/ko-kr/windows/win32/api/shlwapi/nf-shlwapi-strcmplogicalw

	static int CALLBACK OriginalSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ASSERT(lParam1 != NULL && lParam2 != NULL);

		CtlModelTree* pTree = (CtlModelTree*)lParamSort;
		
		CtlModelTree::ItemData* pItem1 = (CtlModelTree::ItemData*)lParam1;
		CtlModelTree::ItemData* pItem2 = (CtlModelTree::ItemData*)lParam2;
		ASSERT(pItem1 != NULL && pItem2 != NULL);

		return pItem1->Index < pItem2->Index ? -1 : 1;
	}

	static int CALLBACK AscendingSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ASSERT(lParam1 != NULL && lParam2 != NULL);

		CtlModelTree* pTree = (CtlModelTree*)lParamSort;

		CtlModelTree::ItemData* pItem1 = (CtlModelTree::ItemData*)lParam1;
		CtlModelTree::ItemData* pItem2 = (CtlModelTree::ItemData*)lParam2;
		ASSERT(pItem1 != NULL && pItem2 != NULL);

		CString text1 = pTree->GetItemText(pTree->FindItem(pItem1->Key));
		CString text2 = pTree->GetItemText(pTree->FindItem(pItem2->Key));

		return StrCmpLogicalW(text1, text2);
	}

	static int CALLBACK DescendingSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		ASSERT(lParam1 != NULL && lParam2 != NULL);

		CtlModelTree* pTree = (CtlModelTree*)lParamSort;

		CtlModelTree::ItemData* pItem1 = (CtlModelTree::ItemData*)lParam1;
		CtlModelTree::ItemData* pItem2 = (CtlModelTree::ItemData*)lParam2;
		ASSERT(pItem1 != NULL && pItem2 != NULL);

		CString text1 = pTree->GetItemText(pTree->FindItem(pItem1->Key));
		CString text2 = pTree->GetItemText(pTree->FindItem(pItem2->Key));

		return StrCmpLogicalW(text2, text1);
	}

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
		// WARNING - 20, maximum height
		return globalUtils.ScaleByDPI(CSize(20, 20));
	}
};



BEGIN_MESSAGE_MAP(CtlModelTree, CBCGPTreeCtrl)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlPanel3dModel, CtlPanel)
	//ON_NOTIFY(TVN_BEGINDRAG, Id::Tree,		OnTreeBeginDrag)
	//ON_NOTIFY(TVN_BEGINLABELEDIT, Id::Tree,	OnTreeBeginLabelEdit)
	ON_NOTIFY(NM_CLICK,	Id::Tree,				OnTreeClick)
	//ON_NOTIFY(NM_DBLCLK, Id::Tree,			OnTreeDblClick)
	//ON_NOTIFY(TVN_DELETEITEM, Id::Tree,		OnTreeDeleteItem)
	//ON_NOTIFY(TVN_ENDLABELEDIT, Id::Tree,		OnTreeEndLabelEdit)
	//ON_NOTIFY(TVN_ITEMEXPANDED, Id::Tree,		OnTreeItemExpanded)
	ON_NOTIFY(TVN_ITEMEXPANDING, Id::Tree,		OnTreeItemExpanding)
	//ON_NOTIFY(NM_RCLICK, Id::Tree,			OnTreeRClick)
	//ON_NOTIFY(NM_RDBLCLK, Id::Tree,			OnTreeRDbClick)
	ON_NOTIFY(TVN_SELCHANGED, Id::Tree,			OnTreeSelChanged)
	//ON_NOTIFY(TVN_SELCHANGING, Id::Tree,		OnTreeSelChanging)
	//ON_NOTIFY(NM_SETFOCUS, Id::Tree,			OnTreeSetFocus)

	//ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, OnDPIChangedAfterParent)
	ON_BN_CLICKED(Id::SortButton, OnCommandSort)
END_MESSAGE_MAP()



CtlPanel3dModel::CtlPanel3dModel()
{
	TreeCtl = new CtlModelTree();
}



CtlPanel3dModel::~CtlPanel3dModel()
{
	REMOVE_POINTER(TreeCtl);
}



void CtlPanel3dModel::AdjustLayout(int cx, int cy)
{
	__super::AdjustLayout(cx, cy);

	CRect rect = GetBodyRect();
	Control.SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE);
}



int CtlPanel3dModel::ConstructHeader(int cx)
{
	ToolBarCtl.SetPivot(Ctl::EPivot::TopLeft);
	ToolBarCtl.Initialize(this);
	ToolBarCtl.AddButtonWithMenu((UINT)Id::SortButton, {
		TOOLBAR_3D_CMD_Sort_ByOriginal,
		TOOLBAR_3D_CMD_Sort_ByAscending,
		TOOLBAR_3D_CMD_Sort_ByDescending
	});
	ToolBarCtl.CheckMenu((UINT)Id::SortButton, TOOLBAR_3D_CMD_Sort_ByOriginal, true);

	return HeaderHeight = ToolBarCtl.AdjustLayout().cy;
}



void CtlPanel3dModel::ConstructBody()
{
	Control.Initialize(this, GetBodyRect());
}



void CtlPanel3dModel::ReceiveSignal(Json::Object* pData)
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
	case Signal::ModelPanel::Action::ViewItem:
		Control.EnsureVisible(FindTreeItem(SKW_KEY));
		break;

	case Signal::ModelPanel::Action::InverseCheckedStatus:
		Control.InverseCheckedStatus();
		break;

	case Signal::ModelPanel::Action::RedrawTree:
		Control.Redraw(pData->GetBoolean(SKW_FLAG));
		break;

	default:
		DEBUG_STOP;
		break;
	}

	REMOVE_POINTER(pData);
}



void CtlPanel3dModel::OnCommandSort()
{
	UINT id = ToolBarCtl.GetMenuResult((UINT)Id::SortButton);
	ToolBarCtl.CheckMenu((UINT)Id::SortButton, id, true);
	ToolBarCtl.UpdateWindow();

	switch (id) {
	case TOOLBAR_3D_CMD_Sort_ByOriginal:
		Control.Sort(Control.Original);
		break;

	case TOOLBAR_3D_CMD_Sort_ByAscending:
		Control.Sort(Control.Ascending);
		break;

	case TOOLBAR_3D_CMD_Sort_ByDescending:
		Control.Sort(Control.Descending);
		break;

	default:
		DEBUG_RETURN;
	}

	Control.RedrawWindow();
}

//--------------------------------------------------------------------------------------------------

void CtlPanel3dModel::OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	//CString text = Control.GetItemText(pNMTreeView->itemNew.hItem);
	//POINT pos = pNMTreeView->ptDrag;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	//CString text = pTVDispInfo->item.pszText;
	CEdit* pEdit = (CEdit*)CWnd::FromHandle((HWND)Control.SendMessage(TVM_GETEDITCONTROL));
	if (pEdit->GetSafeHwnd() != nullptr) {
		pEdit->PostMessage(EM_SETSEL, 0, (LPARAM)-1);
	}

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DisableNotification();

	*pResult = S_OK;

	UINT flag = 0;
	HTREEITEM hItem = Control.HitFlag(flag);
	if (hItem == nullptr) {
		return;
	}

//	if (flag & TVHT_NOWHERE) {
//		DEBUG_LOG(L"* OnTreeClick: TVHT_NOWHERE");
//	}
//	if (flag & TVHT_ONITEMICON) {
//		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMICON");
//#ifdef _TEST
//		int image;
//		BOOL success = Control.GetItemImage(hItem, image, image);
//		ASSERT(success);
//
//		image = (image == 0 ? 1 : 0);
//		Control.SetItemImage(hItem, image, image);
//#endif
//	}
	//if (flag & TVHT_ONITEMLABEL) {
	//	DEBUG_LOG(L"* OnTreeClick: Label");
	//}
//	if (flag & TVHT_ONITEMINDENT) {
//		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMINDENT");
//	}
//	if (flag & TVHT_ONITEMBUTTON) {
//		DEBUG_LOG(L"* OnTreeClick: Expand button");
//	}
//	if (flag & TVHT_ONITEMRIGHT) {
//		DEBUG_LOG(L"* OnTreeClick: TVHT_ONITEMRIGHT");
//	}
	if (flag & TVHT_ONITEMSTATEICON) {
		DWORD_PTR key = Control.GetItemKey(hItem);
		ASSERT(key != 0);

		// WARNING - the state is not changed yet
		BOOL checked = !Control.GetCheck(hItem);

		Control.SetCheck(hItem, checked);
		Control.UpdateChildrenCheckState(hItem, checked);

		HTREEITEM hRoot = Control.GetRootItem();
		int state = Control.UpdateCheckState(hRoot);
		Control.SetCheck(hRoot, state > 0 ? TRUE : FALSE);

		DEBUG_LOG(WStr::Format(L"* OnItemChecked activated - %d", checked));
		theDelivery.modelPanel.OnItemChecked(key, checked);

		*pResult = S_FALSE;
		return;
	}

	//if (flag & TVHT_ABOVE) {
	//	DEBUG_LOG(L"* OnTreeClick: TVHT_ABOVE");
	//}
	//if (flag & TVHT_BELOW) {
	//	DEBUG_LOG(L"* OnTreeClick: TVHT_BELOW");
	//}
	//if (flag & TVHT_TORIGHT) {
	//	DEBUG_LOG(L"* OnTreeClick: TVHT_TORIGHT");
	//}
	//if (flag & TVHT_TOLEFT) {
	//	DEBUG_LOG(L"* OnTreeClick: TVHT_TOLEFT");
	//}
}



void CtlPanel3dModel::OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	UINT flag = 0;
	HTREEITEM hItem = Control.HitFlag(flag);
	if (hItem == nullptr) {
		return;
	}

	DWORD_PTR key = Control.GetItemKey(hItem);
	ASSERT(key != 0);

	DEBUG_LOG(L"* OnItemDblClicked activated");
	theDelivery.modelPanel.OnItemDblClicked(key);

	// CHECK - if S_OK, tree expand the item
	*pResult = S_FALSE;
}



void CtlPanel3dModel::OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
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



void CtlPanel3dModel::OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (DisableNofify) {
		return;
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (pNMTreeView->action == TVE_EXPAND) {
		HTREEITEM hItem = pNMTreeView->itemNew.hItem;

		if (Control.GetChildItem(hItem) == nullptr) {
			DWORD_PTR key = Control.GetItemKey(hItem);
			ASSERT(key != 0);

			DEBUG_LOG(L"* OnItemExpanded activated");
			theDelivery.modelPanel.OnItemExpanded(key);
		}
	}
	else {
		ASSERT(pNMTreeView->action == TVE_COLLAPSE);
	}

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (DisableNofify) {
		return;
	}

	// WARNING - range selection, skip signal
	if (::GetAsyncKeyState(VK_SHIFT) & 0x8000) {
		*pResult = S_OK;
		return;
	}
	// WARNING - multi selection
	if (::GetAsyncKeyState(VK_CONTROL) & 0x8080) {
		// TODO - how
	}

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	if (hItem == nullptr) {
	}
	else {
		DWORD_PTR key = Control.GetItemKey(hItem);
		ASSERT(key != 0);

		DEBUG_LOG(L"* OnItemSelected activated");
		theDelivery.modelPanel.OnItemSelected(key);
	}

	*pResult = S_OK;
}

// WARNING - not use

void CtlPanel3dModel::OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}



void CtlPanel3dModel::OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult)
{
	DEBUG_STOP;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = S_OK;
}

//--------------------------------------------------------------------------------------------------

HTREEITEM CtlPanel3dModel::AddItem(Json::Object* pData)
{
	static int DummyIndex = 0;

	DWORD_PTR key = pData->GetDwordPtr(SKW_KEY);

	HTREEITEM hItem = Control.AddItem(
		FindTreeItem(SKW_PARENT),
		key,
		++DummyIndex,
		(LPWSTR)(LPCTSTR)pData->GetString(SKW_TITLE),
		pData->GetBoolean(SKW_CHECKED),
		pData->GetBoolean(SKW_HASCHILDREN),
		pData->GetInteger(SKW_TYPE)
	);

	return hItem;
}



void CtlPanel3dModel::AddChildren(Json::Object* pData)
{
	DisableNotification();

	Json::Array& items = pData->GetArray(SKW_CHILDREN);

	HTREEITEM hParent = FindTreeItem(SKW_PARENT);
	if (hParent == nullptr) {
		DEBUG_RETURN;
	}

	DEBUG_LOG(WStr::Format(L"AddChildren: %s", Control.GetItemText(hParent)));

	int dummyIndex = 0;
	for (auto item : items.GetBuffer()) {
		Json::Object& child = item->AsObject();

		HTREEITEM hItem = Control.AddItem(
			hParent,
			child.GetDwordPtr(SKW_KEY),
			++dummyIndex,
			(LPWSTR)(LPCTSTR)child.GetString(SKW_TITLE),
			child.GetBoolean(SKW_CHECKED),
			child.GetBoolean(SKW_HASCHILDREN),
			child.GetInteger(SKW_TYPE)
		);
	}

	if (Control.GetSortMethod() != CtlModelTree::Original) {
		Control.Sort(hParent);
	}
	Control.Expand(hParent, pData->GetBoolean(SKW_EXPAND) ? TVE_EXPAND : TVE_COLLAPSE);
}



void CtlPanel3dModel::CheckItem(Json::Object* pData)
{
	DisableNotification();

	Control.Redraw(false);

	HTREEITEM hItem = FindTreeItem(SKW_KEY);
	if (hItem == nullptr) {
		DEBUG_RETURN;
	}

	BOOL checked = (BOOL)pData->GetBoolean(SKW_CHECKED);
	Control.SetCheck(hItem, checked);
	Control.UpdateChildrenCheckState(hItem, checked);

	HTREEITEM hRoot = Control.GetRootItem();
	int state = Control.UpdateCheckState(hRoot);
	Control.SetCheck(hRoot, state > 0 ? TRUE : FALSE);

	Control.Redraw(true);
}



void CtlPanel3dModel::CheckItems(Json::Object* pData)
{
	DisableNotification();

	Control.Redraw(false);

	if (pData->FindValue(SKW_CHECKED) == nullptr) {
		// Multi-purpose
		Json::Array& items = pData->GetArray(SKW_ITEMS);

		HTREEITEM hItem = nullptr;
		for (auto item : items.GetBuffer()) {
			Json::Object& node = item->AsObject();

			BOOL checked = (BOOL)node.GetBoolean(SKW_FLAG);
			hItem = Control.FindItem(node.GetDwordPtr(SKW_KEY));
			if (hItem != nullptr) {
				Control.SetCheck(hItem, checked);
				Control.UpdateChildrenCheckState(hItem, checked);
			}
			else {
				DEBUG_STOP;
			}
		}
	}
	else {
		// Single status
		BOOL checked = (BOOL)pData->GetBoolean(SKW_CHECKED);

		Signal::KeyItems keys;
		pData->GetArray(SKW_ITEMS).ToArray(keys);

		HTREEITEM hItem = nullptr;
		for (auto key : keys) {
			hItem = Control.FindItem(key);
			if (hItem != nullptr) {
				Control.SetCheck(hItem, checked);
				Control.UpdateChildrenCheckState(hItem, checked);
			}
			else {
				DEBUG_STOP;
			}
		}
	}

	HTREEITEM hRoot = Control.GetRootItem();
	int state = Control.UpdateCheckState(hRoot);
	Control.SetCheck(hRoot, state > 0 ? TRUE : FALSE);

	Control.Redraw(true);
}



void CtlPanel3dModel::DeleteItem(Json::Object* pData)
{
	DisableNotification();

	BOOL success = Control.DeleteItem(FindTreeItem(SKW_KEY));
	ASSERT(success);
}



void CtlPanel3dModel::ExpandItem(Json::Object* pData)
{
	DisableNotification();

	BOOL success = Control.Expand(FindTreeItem(SKW_KEY), pData->GetBoolean(SKW_EXPAND) ? TVE_EXPAND : TVE_COLLAPSE);
	ASSERT(success);
}



void CtlPanel3dModel::ExpandParent(Json::Object* pData)
{
	DisableNotification();

	BOOL success = Control.Expand(FindTreeItem(SKW_KEY), TVE_EXPAND);
	//ASSERT(success);
}



void CtlPanel3dModel::SelectItem(Json::Object* pData)
{
	DisableNotification();

	HTREEITEM hItem = FindTreeItem(SKW_KEY);
	if (hItem != nullptr) {
		// TODO - uncheck
		bool selected = pData->GetBoolean(SKW_FLAG);
		ASSERT(selected);

		Control.EnsureVisible(hItem);
		Control.SelectItem(hItem);
	}
	else {
		DEBUG_STOP;
	}
}



void CtlPanel3dModel::SelectItems(Json::Object* pData)
{
	DEBUG_STOP;
	DisableNotification();

	Control.Redraw(false);

	// TODO - uncheck
	bool checked = pData->GetBoolean(SKW_FLAG);
	ASSERT(checked);

	Signal::KeyItems keys;
	pData->GetArray(SKW_ITEMS).ToArray(keys);

	HTREEITEM hItem = nullptr;
	for (auto key : keys) {
		hItem = Control.FindItem(key);
		if (hItem != nullptr) {
			Control.SelectItem(hItem);
		}
		else {
			DEBUG_STOP;
		}
	}

	Control.Redraw(true);
}

#undef DEBUG_LOG
#undef Control
#undef FindTreeItem
#undef DisableNotification
#undef theDelivery
