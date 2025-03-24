#include "stdafx.h"

#include "Ast.h"
#include "Ctl.TreePropList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	enum class Id
	{
		This = WM_USER,
		Tree,
		PropList,
	};
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlTreePropList, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()

	ON_NOTIFY(TVN_SELCHANGED, Id::Tree, OnTreeSelChanged)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()



CtlTreePropList::CtlTreePropList()
{
}



CtlTreePropList::~CtlTreePropList()
{
	DestroyWindow();
}



bool CtlTreePropList::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, rect, pParentWnd, (UINT)Id::This) == FALSE) {
		RETURN_FALSE;
	}

	TreeCtl.Initialize(this, (UINT)Id::Tree);
	PropListCtl.Initialize(this, (UINT)Id::PropList);

	return true;
}



bool CtlTreePropList::InitializeDesign(Json::Object& design)
{
	Dictionary = design.GetString("dictionary");
	DesignData = &design;
	TreeCtl.InitializeDesign(DesignData->GetArray("tree"));
	ChangePropList(TreeCtl.GetSelectedItem());

	return true;
}



bool CtlTreePropList::InitializeValue(Json::Object& data)
{
	ValueData = &data;

	return true;
}



void CtlTreePropList::RefreshData()
{
	ChangePropList(TreeCtl.GetSelectedItem());
}



BOOL CtlTreePropList::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd(pDC);
}



void CtlTreePropList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		const int treeWidth = globalUtils.ScaleByDPI(200);
		CSize padding = Ctl::Gap();

		int x = padding.cx;
		int y = padding.cy;
		int width = treeWidth;
		int height = cy - padding.cy * 2;
		TreeCtl.SetWindowPos(NULL, x, y, width, height, 0);

		x += width + padding.cx;
		width = cx - width - padding.cx * 3;
		PropListCtl.SetWindowPos(NULL, x, y, width, height, 0);
	}
}



void CtlTreePropList::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ChangePropList(pNMTreeView->itemNew.hItem);
}



LRESULT CtlTreePropList::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CBCGPProp* pProp = (CBCGPProp*)lp;
	Json::Value* pValue = reinterpret_cast<Json::Value*>(pProp->GetData());

	if (PropListCtl.Initialized && pValue != nullptr) {
		if (PropertyChangedHandler != nullptr) {
			PropertyChangedHandler->SendMessage(BCGM_PROPERTY_CHANGED, (WPARAM)pValue, (LPARAM)pProp);
		}
		else {
			Ast::SetValue(*pValue, *pProp);
		}
		Modified = true;
	}

	return S_OK;
}



void CtlTreePropList::ChangePropList(HTREEITEM pItem)
{
	if (pItem == nullptr || TreeCtl.Initialized == false) {
		return;
	}

	HTREEITEM pChild = TreeCtl.GetChildItem(pItem);
	if (pChild != nullptr) {
		TreeCtl.SelectItem(pChild);
		return;
	}

	CStringA path = (CStringA)TreeCtl.GetItemNamePath(pItem);
	Json::Object* pDesign = Json::Helper::FindObjectByPath(*DesignData, "properties/" + path);
	Json::Object* pData = Json::Helper::FindObjectByPath(*ValueData, path);

	if (pDesign != nullptr) {
		PropListCtl.InitializeDesign(*pDesign, Dictionary);

		if (pData != nullptr) {
			PropListCtl.InitializeValue(*pData);
		}
		else {
			DEBUG_STOP;
		}
	}
	else {
		PropListCtl.RemoveAll();
		// WARNING - UpdateWindow or RedrawWindow not working 
		PropListCtl.AdjustLayout();
		DEBUG_STOP;
	}
}
