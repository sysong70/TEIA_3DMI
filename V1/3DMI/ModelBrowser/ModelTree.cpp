#include "pch.h"

#include "ModelTree.h"

#include "../MainFrm.h"
#include "../DLL/DmiJsonCmd.h"

ModelTreeCtrl::ModelTreeCtrl(DWORD_PTR nId) :
	m_nId(nId)
{
}

BEGIN_MESSAGE_MAP(ModelTreeCtrl, CTreeCtrl)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRClick)
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, &ModelTreeCtrl::OnTvnItemexpanded)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, &ModelTreeCtrl::OnTvnSelchanged)
END_MESSAGE_MAP()

//== Itme Function =================================================================================
void ModelTreeCtrl::InsertJsonItem(HTREEITEM hParentItem, Json::Object & cInObject)
{
	CString strTitle = cInObject.GetString("Title");
	int nType = (int)cInObject.GetDwordPtr("Type");
	bool bHasChildren = cInObject.GetBoolean("HasChildren");
	DWORD_PTR nItemData = cInObject.GetDwordPtr("CompPtr");

	int nImageIndex = GetMainFrame()->GetModelBrowserBar().GetImageBaseIndex(nType);

	TVINSERTSTRUCT cInsertStruct;
	cInsertStruct.hParent = hParentItem;
	cInsertStruct.hInsertAfter = TVI_LAST;
	cInsertStruct.item.mask = TVIF_CHILDREN | TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	cInsertStruct.item.cChildren = bHasChildren;
	cInsertStruct.item.lParam = (LPARAM) nItemData;
	cInsertStruct.item.pszText = (LPTSTR) strTitle.GetBuffer();
	cInsertStruct.item.iImage = nImageIndex;
	cInsertStruct.item.iSelectedImage = nImageIndex;

	HTREEITEM hInsertItem = InsertItem(&cInsertStruct);

	Json::Array & acChildArray = cInObject.GetArray("Child");

	for(int nIndex = 0; nIndex < acChildArray.GetSize(); nIndex++) {
		InsertJsonItem(hInsertItem, acChildArray[nIndex]->AsObject());
	}
}

//== Window Message Function =======================================================================
void ModelTreeCtrl::OnRClick(NMHDR * /*pNMHDR*/, LRESULT * pResult)
{
	SendMessage(WM_CONTEXTMENU, (WPARAM) m_hWnd, GetMessagePos());
	*pResult = 1;
}

void ModelTreeCtrl::OnContextMenu(CWnd * /*pWnd*/, CPoint ptMousePos)
{
	ScreenToClient(&ptMousePos);

	UINT flags;
	HTREEITEM item = HitTest(ptMousePos, &flags);
	if(item != NULL && (flags & TVHT_ONITEM) != 0)
	{

	}
/*
	UINT flags;
	HTREEITEM item = HitTest(ptMousePos, &flags);
	if(item != NULL && (flags & TVHT_ONITEM) != 0)
	{
		CMenu menu;
		menu.LoadMenuW(IDR_MB_CONTEXT_MENU);
		CMenu * pPopup = menu.GetSubMenu(0);

		CString showHideString;
		m_pcContextItem = (MFCComponentTreeItem *) GetItemData(item);
		if(m_pcContextItem != nullptr)
		{
			HPS::Component selected_component = m_pcContextItem->GetComponent();
			HPS::Component::ComponentType component_type = selected_component.GetComponentType();
			if(selected_component.HasComponentType(HPS::Component::ComponentType::ExchangeComponentMask))
			{
				if(m_pcContextItem->IsHidden())
					showHideString = "Show";
				else
					showHideString = "Hide";
				pPopup->ModifyMenuW(ID_MB_CONTEXT_SHOW_HIDE, MF_BYCOMMAND | MF_STRING, ID_MB_CONTEXT_SHOW_HIDE, showHideString);
			}
			else if(component_type == HPS::Component::ComponentType::DWGLayer)
			{
#ifdef USING_DWG
				pPopup = menu.GetSubMenu(1);
				HPS::DWG::Layer dwg_layer(selected_component);
				if(dwg_layer.IsOn())
					showHideString = "Turn OFF";
				else
					showHideString = "Turn ON";
				pPopup->ModifyMenuW(ID_DWGLAYER_TOGGLE, MF_BYCOMMAND | MF_STRING, ID_DWGLAYER_TOGGLE, showHideString);
#endif
			}
			else if(component_type == HPS::Component::ComponentType::DWGModelFile ||
				component_type == HPS::Component::ComponentType::DWGBlockTable ||
				component_type == HPS::Component::ComponentType::DWGLayerTable ||
				component_type == HPS::Component::ComponentType::DWGLayout)
				return;

			ClientToScreen(&ptMousePos);
			pPopup->TrackPopupMenu(TPM_LEFTALIGN, ptMousePos.x, ptMousePos.y, this);
		}
	}
*/
}

void ModelTreeCtrl::OnTvnItemexpanded(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
/*
	TVITEM cItem;
	cItem.mask = TVIF_HANDLE | TVIF_PARAM | TVIF_CHILDREN;
	cItem.hItem = hTreeItem;
	GetItem(&cItem);
*/

	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	HTREEITEM hChildTreeItem = GetChildItem(hTreeItem);

	while(nullptr != hChildTreeItem)
	{
		BOOL bChildFlag = ItemHasChildren(hChildTreeItem);
		HTREEITEM hChildChildTreeItem = GetChildItem(hChildTreeItem);

		// Child Flag이 True이면서 하부 Child가 없는 경우에는 확장을 시도한다.
		if(TRUE == bChildFlag && NULL == hChildChildTreeItem) {

			DWORD_PTR nItemData = GetItemData(hChildTreeItem);

			Json::Object cObject;
			cObject.SetDwordPtr("CompPtr", nItemData);

			Json::Object cRetObject;

			if(true == DmiJsonCmd::ExecuteHpsCommand(DmiHps::Command::AddChildComponentTreeItem, m_nId, cObject, cRetObject)) {
				Json::Array & acChildArray = cRetObject.GetArray("Child");
				for(int nIndex = 0; nIndex < acChildArray.GetSize(); nIndex++) {
					InsertJsonItem(hChildTreeItem, acChildArray[nIndex]->AsObject());
				}
			}
		}

		hChildTreeItem = GetNextItem(hChildTreeItem, TVGN_NEXT);
	}

	*pResult = 0;
}

// 선택된 아이탬 Highlight 처리
void ModelTreeCtrl::OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	
	HTREEITEM hTreeItem = pNMTreeView->itemNew.hItem;
	DWORD_PTR nItemData = GetItemData(hTreeItem);
	if(NULL == nItemData) {
		return;
	}

	Json::Object cObject;
	cObject.SetDwordPtr("CompPtr", nItemData);

	Json::Object cRetObject;
	DmiJsonCmd::ExecuteHpsCommand(DmiHps::Command::SelectComponentTreeItem, m_nId, cObject, cRetObject);

/*
	if(true == DmiJsonCmd::ExecuteHpsCommand(DmiHps::CommandId::SelectComponentTreeItem, m_nId, cObject)) {
		Json::Array & acChildArray = cRetObject.GetArray("Child");
		for(int nIndex = 0; nIndex < acChildArray.GetSize(); nIndex++) {
			InsertJsonItem(hChildTreeItem, acChildArray[nIndex]->AsObject());
		}
	}
*/

	*pResult = 0;
}
