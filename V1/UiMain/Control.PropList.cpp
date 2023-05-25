#include "stdafx.h"
#include "Control.PropList.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetPropList

namespace PresetPropList
{
	int const Id = WM_USER;
	int ControlId = 0;

	void Initialize()
	{
		ControlId = Id + 1;
	}

	UINT GetControlId()
	{
		return ++ControlId;
	}
}



using namespace Control;

BEGIN_MESSAGE_MAP(PropList, CBCGPPropList)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



Control::PropList::PropList()
{
	m_bVisualManagerStyle = TRUE;

	EnableHeaderCtrl(FALSE);
	SetBooleanPropertiesStyle(CBCGPPropList::BOOL_PROPS_SWITCH);
	SetButtonsAppearance(
		BCGP_PROPLIST_ALWAYS_DISPLAY_DROPDOWN_BUTTONS |
		BCGP_PROPLIST_ALWAYS_DISPLAY_PUSH_BUTTONS
	);
}



Control::PropList::~PropList()
{
}



void Control::PropList::InitializeDesign(Json::Object& design)
{
	PRESET::Initialize();
	RemoveAll();

	m_bInitialized = false;
	m_pDesign = &design;

	SetRedraw(FALSE);
	{
		CreateProp(*m_pDesign);
	}
	SetRedraw(TRUE);
}



void Control::PropList::InitializeData(Json::Object& data)
{
	m_pData = &data;

	for (int i = 0; i < GetPropertyCount(); i++) {
		CBCGPProp* pProp = GetProperty(i);
		CString* pName = reinterpret_cast<CString*>(pProp->GetData());
		if (pName != nullptr) {
			ReplacePropData(pProp, m_pData->FindValue((CStringA)*pName));
		}
	}

	m_bInitialized = true;
}



void Control::PropList::GetAncestor(CBCGPProp* pItem, std::vector<CBCGPProp*>& ancestor)
{
	CBCGPProp* pParent = pItem;
	while (pParent != nullptr) {
		ancestor.push_back(pParent);
		pParent = pParent->GetParent();
	}
}



Json::Object& Control::PropList::GetData()
{
	DEBUG_VALID(m_pData);
	return *m_pData;
}



CString Control::PropList::GetItemNamePath(CBCGPProp* pItem)
{
	std::vector<CBCGPProp*> items;
	GetAncestor(pItem, items);

	CString path;
	std::vector<CBCGPProp*>::reverse_iterator iter;
	for (iter = items.rbegin(); iter != items.rend(); iter++) {
		CBCGPProp* pParent = *iter;
		CString* pName = reinterpret_cast<CString*>(pParent->GetData());
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



CBCGPProp* Control::PropList::CreateProp(Json::Object& design)
{
	CBCGPProp* pProp = nullptr;
	CString type = Facility::GetType(design);

	if (type == L"group") {
		pProp = CreateGroupProp(design);
	}
	else if (type == L"check") {
		pProp = CreateCheckProp(design);
	}
	else if (type == L"color") {
		pProp = CreateColorProp(design);
	}
	else if (type == L"drop") {
		pProp = CreateDropdownProp(design);
	}
	else if (type == L"edit") {
		pProp = CreateEditProp(design);
	}
	else if (type == L"file") {
		pProp = CreateFileProp(design);
	}
	else if (type == L"folder") {
		pProp = CreateFolderProp(design);
	}
	else if (type == L"root") {
		Json::Array* pItems = Facility::GetItems(design);
		if (pItems != nullptr) {
			for (Json::Value* pValue : pItems->GetBuffer()) {
				CBCGPProp* pSubItem = CreateProp(pValue->AsObject());
				if (pSubItem != nullptr) {
					AddProperty(pSubItem);
				}
			}
		}
	}
	else {
		DEBUG_STOP;
	}
	CStringA a;

	return pProp;
}



CBCGPProp* Control::PropList::CreateGroupProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design));
	SetPropName(pProp, design);

	Json::Array* pItems = Facility::GetItems(design);
	if (pItems != nullptr) {
		for (Json::Value* pValue : pItems->GetBuffer()) {
			CBCGPProp* pSubItem = CreateProp(pValue->AsObject());
			if (pSubItem != nullptr) {
				pProp->AddSubItem(pSubItem);
			}
		}
	}

	return pProp;
}



CBCGPProp* Control::PropList::CreateCheckProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design), id,
		design.GetBoolean("value"), Facility::GetDesciption(design));
	SetPropName(pProp, design);

	return pProp;
}



CBCGPProp* Control::PropList::CreateColorProp(Json::Object& data, UINT id)
{
	RETURN_NULL;
}



CBCGPProp* Control::PropList::CreateDropdownProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design), id,
		(LPCTSTR)L"", Facility::GetDesciption(design));
	SetPropName(pProp, design);

	std::vector<CString> items;
	Facility::GetItems(design, items);
	for (CString& item : items) {
		pProp->AddOption(item);
	}

	pProp->AllowEdit(FALSE);
	pProp->SelectOption(design.GetInteger("value"));

	return pProp;
}



CBCGPProp* Control::PropList::CreateEditProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CString sValue;
	Json::Value& value = design.GetValue("value");
	if (value.IsValid()) {
		sValue = value.ToString();
	}

	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design), id,
		(LPCTSTR)sValue, Facility::GetDesciption(design));
	SetPropName(pProp, design);

	return pProp;
}



CBCGPProp* Control::PropList::CreateFileProp(Json::Object& design, UINT id)
{
	RETURN_NULL;
}



CBCGPProp* Control::PropList::CreateFolderProp(Json::Object& design, UINT id)
{
	RETURN_NULL;
}



CBCGPProp* Control::PropList::FindPropByData(CString& name)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		CString* pName = reinterpret_cast<CString*>(pProp->GetData());
		if (pName != nullptr && *pName == name) {
			return pProp;
		}
	}

	RETURN_NULL;
}



CBCGPProp* Control::PropList::FindPropByData(CBCGPProp* pParent, CString& name)
{
	for (int i = 0; i < pParent->GetSubItemsCount(); i++) {
		CBCGPProp* pProp = pParent->GetSubItem(i);
		CString* pName = reinterpret_cast<CString*>(pProp->GetData());
		if (pName != nullptr && *pName == name) {
			return pProp;
		}
	}

	RETURN_NULL;
}



void Control::PropList::ReplacePropData(CBCGPProp* pProp, Json::Value* pValue)
{
	if (pProp == nullptr || pValue == nullptr) {
		DEBUG_RETURN;
	}

	if (pValue->GetType() == Json::EValueType::Object) {
		ASSERT(pProp->IsGroup());
		Json::Object& data = pValue->AsObject();

		for (int i = 0; i < pProp->GetSubItemsCount(); i++) {
			CBCGPProp* pSubItem = pProp->GetSubItem(i);
			CString* pName = reinterpret_cast<CString*>(pSubItem->GetData());
			if (pName != nullptr) {
				ReplacePropData(pSubItem, data.FindValue((CStringA)*pName));
			}
		}
	}
	else if (pValue->GetType() == Json::EValueType::Array) {
		DEBUG_STOP;
	}
	else {
		pProp->SetData((DWORD_PTR)pValue);

		if (pProp->GetOptionCount() > 0) {
			pProp->SelectOption(pValue->ToInteger());
		}
		else {
			switch (pValue->GetType()) {
			case Json::EValueType::Boolean: pProp->SetValue(pValue->ToBoolean()); break;
			case Json::EValueType::Int:     pProp->SetValue(pValue->ToInteger()); break;
			case Json::EValueType::Uint:    pProp->SetValue(pValue->ToInteger()); break;
			case Json::EValueType::Real:    pProp->SetValue(pValue->ToReal()); break;
			case Json::EValueType::String:  pProp->SetValue((LPCTSTR)pValue->AsString()); break;

			default:
				DEBUG_STOP;
			}
		}
	}
}



void Control::PropList::SetPropName(CBCGPProp* pProp, Json::Object& design)
{
	Json::Value* pValue = design.FindValue("name");
	if (pValue != nullptr) {
		pProp->SetData((DWORD_PTR)&(pValue->AsString()));
	}
	else {
		DEBUG_STOP;
	}
}

#undef PRESET
