#include "stdafx.h"
#include "Control.TreePropList.h"
#include "Facility.h"
#include <Json.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetTreePropList

namespace PresetTreePropList
{
#define GetName(data) data.GetString("name")

	enum ControlId
	{
		Id = WM_USER,
		Tree,
		PropList,
	};

	int TreeWidth()
	{
		return globalUtils.ScaleByDPI(200);
	}

	CSize Padding()
	{
		return globalUtils.ScaleByDPI(CSize(6, 6));
	}
}



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



bool Control::TreePropList::Initialize(CWnd* pParentWnd)
{
	if (__super::Create(NULL, L"", WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, {}, pParentWnd, PRESET::Id) == FALSE) {
		return false;
	}

	CreateTreeCtrl();
	CreatePropList();

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
		CSize padding = PRESET::Padding();

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
		if (pProp->GetOptionCount() > 0) {
			pValue->SetInteger(pProp->GetSelectedOption());
		}
		else {
			switch (pValue->GetType()) {
			case Json::EValueType::Boolean: pValue->SetBoolean(pProp->GetValue()); break;
			case Json::EValueType::Int:     pValue->SetInteger(pProp->GetValue()); break;
			case Json::EValueType::Uint:    pValue->SetInteger(pProp->GetValue()); break;
			case Json::EValueType::Real:    pValue->SetReal(pProp->GetValue()); break;
			case Json::EValueType::String:  pValue->SetString(pProp->GetValue().bstrVal); break;

			default:
				DEBUG_STOP;
				return S_OK;
			}
		}

		m_bModified = true;
	}

	return S_OK;
}



void Control::TreePropList::CreatePropList()
{
	const DWORD dwStyle = WS_VISIBLE | WS_CHILD;
	if (m_propList.Create(dwStyle, {}, this, PRESET::PropList) == FALSE) {
		DEBUG_RETURN;
	}
}



void Control::TreePropList::CreateTreeCtrl()
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		TVS_FULLROWSELECT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS;
	if (m_tree.Create(dwStyle, {}, this, PRESET::Tree) == FALSE) {
		DEBUG_RETURN;
	}
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
		DEBUG_VALID(pData);
		m_propList.InitializeDesign(*pDesign);
		m_propList.InitializeData(*pData);
	}
}

#undef PRESET
