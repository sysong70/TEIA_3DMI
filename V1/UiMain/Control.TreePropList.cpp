#include "stdafx.h"
#include "Control.TreePropList.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

#define PRESET PresetTreePropList

namespace PresetTreePropList
{
	enum EControlId
	{
		Id = WM_USER,
		Tree,
		PropList,
	};



	int TreeWidth()
	{
		return globalUtils.ScaleByDPI(200);
	}
}

//**************************************************************************************************

using namespace Control;

BEGIN_MESSAGE_MAP(TreePropList, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()

	ON_NOTIFY(TVN_SELCHANGED, PRESET::Tree, OnTreeSelChanged)
	ON_REGISTERED_MESSAGE(BCGM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()



Control::TreePropList::TreePropList()
{
}



Control::TreePropList::~TreePropList()
{
}



bool Control::TreePropList::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN;
	if (Create(NULL, L"", dwStyle, rect, pParentWnd, PRESET::Id) == FALSE) {
		RETURN_FALSE;
	}

	m_tree.Initialize(this, PRESET::Tree);
	m_propList.Initialize(this, PRESET::PropList);

	return true;
}



void Control::TreePropList::InitializeDesign(Json::Object& design)
{
	m_pDesign = &design;
	m_tree.InitializeDesign(m_pDesign->GetArray("tree"));
	ChangePropList(m_tree.GetSelectedItem());
}



void Control::TreePropList::InitializeData(Json::Object& data)
{
	m_pData = &data;
}



void Control::TreePropList::RefreshData()
{
	ChangePropList(m_tree.GetSelectedItem());
}



BOOL Control::TreePropList::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd(pDC);
}



void Control::TreePropList::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if (cx > 0 && cy > 0) {
		CSize padding = Gap();

		int x = padding.cx;
		int y = padding.cy;
		int width = PRESET::TreeWidth();
		int height = cy - padding.cy * 2;
		m_tree.SetWindowPos(NULL, x, y, width, height, 0);

		x += width + padding.cx;
		width = cx - width - padding.cx * 3;
		m_propList.SetWindowPos(NULL, x, y, width, height, 0);
	}
}



void Control::TreePropList::OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = S_OK;

	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	ChangePropList(pNMTreeView->itemNew.hItem);
}



LRESULT Control::TreePropList::OnPropertyChanged(WPARAM wp, LPARAM lp)
{
	CBCGPProp* pProp = (CBCGPProp*)lp;
	Json::Value* pValue = reinterpret_cast<Json::Value*>(pProp->GetData());

	if (m_propList.m_bInitialized && pValue != nullptr) {
		if (m_onPropertyChangedHandler != nullptr) {
			m_onPropertyChangedHandler->SendMessage(BCGM_PROPERTY_CHANGED, (WPARAM)pValue, (LPARAM)pProp);
		}
		else {
			Facility::SetValue(*pValue, *pProp);
		}
		m_bModified = true;
	}

	return S_OK;
}



void Control::TreePropList::ChangePropList(HTREEITEM pItem)
{
	if (pItem == nullptr || m_tree.m_bInitialized == false) {
		return;
	}

	HTREEITEM pChild = m_tree.GetChildItem(pItem);
	if (pChild != nullptr) {
		m_tree.SelectItem(pChild);
		return;
	}

	CStringA path = (CStringA)m_tree.GetItemNamePath(pItem);
	Json::Object* pDesign = Json::Helper::FindObjectByPath(*m_pDesign, "properties/" + path);
	Json::Object* pData = Json::Helper::FindObjectByPath(*m_pData, path);

	if (pDesign != nullptr) {
		m_propList.InitializeDesign(*pDesign);

		if (pData != nullptr) {
			m_propList.InitializeData(*pData);
		}
		else {
			DEBUG_STOP;
		}
	}
	else {
		m_propList.RemoveAll();
		//:WARNING - UpdateWindow or RedrawWindow not working 
		m_propList.AdjustLayout();
		DEBUG_STOP;
	}
}

#undef PRESET
