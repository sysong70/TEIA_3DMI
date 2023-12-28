#include "stdafx.h"
#include "Control.PropList.h"
#include "Control.Property.h"
#include "Facility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



#define PRESET PresetPropList

namespace PresetPropList
{
	int ControlId = 0;

	void Initialize()
	{
		ControlId = WM_USER + 1;
	}

	UINT GetControlId()
	{
		return ++ControlId;
	}
}



using namespace Control;

BEGIN_MESSAGE_MAP(PropList, CBCGPPropList)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



Control::PropList::PropList()
{
	//:WARING - static members for Float/Double format (how to initialize?)
	CBCGPProp::m_strFormatFloat = _T("%.4f");
	CBCGPProp::m_strFormatDouble = _T("%.4lf");

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



bool Control::PropList::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
{
	const DWORD dwStyle = WS_VISIBLE | WS_CHILD;
	if (Create(dwStyle, rect, pParentWnd, id) == FALSE) {
		RETURN_FALSE;
	}

	EnableDesciptionArea(TRUE);
	//SetAlternateRowColor(TRUE);
	//SetVSDotNetLook(TRUE);

	return true;
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

	m_bInitialized = false;

	for (int i = 0; i < GetPropertyCount(); i++) {
		CBCGPProp* pProp = GetProperty(i);
		CString name = pProp->GetXMLTagName();

		if (pProp != nullptr && name.IsEmpty() == false) {
			SetPropData(pProp, m_pData->FindValue((CStringA)name));
		}
	}

	m_bInitialized = true;
	//:WARNING - update window
	AdjustLayout();
}



void Control::PropList::Enable(bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Enable((BOOL)value, TRUE);
	}
}

void Control::PropList::Enable(const CString& name, bool value)
{
	CBCGPProp* pFound = FindPropByName(name);
	if (pFound != nullptr) {
		pFound->Enable((BOOL)value, TRUE);
	}
	else {
		DEBUG_STOP;
	}
}



void Control::PropList::Expand(const CString& name, bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Expand((BOOL)value);
	}
}



CBCGPProp* Control::PropList::FindPropByName(const CString& name)
{
	for (int i = 0; i < GetPropertyCount(); i++) {
		CBCGPProp* pProp = GetProperty(i);
		//:WARNING - do not use Name, use XMLTagName
		if (pProp->GetXMLTagName() == name) {
			return pProp;
		}

		// find children
		CBCGPProp* pFound = FindPropByName(GetProperty(i), name);
		if (pFound != nullptr) {
			return pFound;
		}
	}

	RETURN_NULL;
}



CBCGPProp* Control::PropList::FindPropByName(CBCGPProp* pParent, const CString& name)
{
	for (int i = 0; i < pParent->GetSubItemsCount(); i++) {
		CBCGPProp* pProp = pParent->GetSubItem(i);
		//:WARNING - do not use Name, use XMLTagName
		if (pProp->GetXMLTagName() == name) {
			return pProp;
		}

		// find children
		pProp = FindPropByName(pProp, name);
		if (pProp != nullptr) {
			return pProp;
		}
	}

	return nullptr;
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
		//:WARNING - do not use Name, use XMLTagName
		path += L'/' + pParent->GetXMLTagName();
	}

	path.TrimLeft(L'/');

	return path;
}



void Control::PropList::SetPropData(CBCGPProp* pProp, Json::Value* pValue)
{
	if (pProp == nullptr || pValue == nullptr) {
		return;
	}

	if (pValue->GetType() == Json::EValueType::Object) {
		ASSERT(pProp->IsGroup());
		pProp->SetData((DWORD_PTR)pValue);
		Json::Object& data = pValue->AsObject();

		if (pProp->IsGroupWithCheckBox()) {
			pProp->SetValue(data.GetBoolean("checked"));
		}

		for (int i = 0; i < pProp->GetSubItemsCount(); i++) {
			CBCGPProp* pSubItem = pProp->GetSubItem(i);
			DEBUG_VALID(pSubItem);

			CString name = pSubItem->GetXMLTagName();
			if (name.IsEmpty() == false) {
				SetPropData(pSubItem, data.FindValue((CStringA)name));
			}
		}
	}
	else if (pValue->GetType() == Json::EValueType::Array) {
		DEBUG_STOP;
	}
	else {
		pProp->SetData((DWORD_PTR)pValue);
		SetPropValue(pProp, pValue);
	}
}



void Control::PropList::SetPropName(CBCGPProp* pProp, Json::Object& design)
{
	Json::Value* pValue = design.FindValue("name");
	if (pValue != nullptr) {
		//:WARNING - do not use Name, use XMLTagName
		pProp->SetXMLTagName(pValue->AsString());
	}
	else {
		DEBUG_STOP;
	}
}



void Control::PropList::SetPropValue(CBCGPProp* pProp, Json::Value* pValue)
{
	DEBUG_VALID(pProp);
	DEBUG_VALID(pValue);

	Facility::SetValue(*pProp, *pValue);
}



void Control::PropList::Show(const CString& name, bool value)
{
	CBCGPProp* pFound = FindPropByName(name);
	if (pFound != nullptr) {
		pFound->Show((BOOL)value);
	}
	else {
		DEBUG_STOP;
	}
}



void Control::PropList::ShowAll(bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Show((BOOL)value);
	}
}



BOOL Control::PropList::PreTranslateMessage(MSG* pMsg)
{
	BOOL process = __super::PreTranslateMessage(pMsg);
	if (process == FALSE && pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) {
		if (m_pSel == nullptr) {
			return process;
		}

		if (m_pSel->IsDroppedDown()) {
			OnCloseCombo();
			SetFocus();
			process = TRUE;
		}
	}

	return process;
}



void Control::PropList::OnPropertyChanged(CBCGPProp* pProp) const
{
	__super::OnPropertyChanged(pProp);

	if (pProp->IsGroupWithCheckBox()) {
		GetOwner()->SendMessage(BCGM_PROPERTY_CHANGED, GetDlgCtrlID(), LPARAM(pProp));
	}
}



void Control::PropList::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if (m_pTracked != nullptr) {
		Property::CommandButton* button = dynamic_cast<Property::CommandButton*>(m_pTracked);
		if (button != nullptr) {
			button->m_bHighlighted = true;
			button->Redraw();
		}
	}
}



CBCGPProp* Control::PropList::CreateProp(Json::Object& design)
{
	CBCGPProp* pProp = nullptr;
	CString type = Facility::GetType(design);

	if (type == L"group") {
		pProp = CreateGroupProp(design);
	}
	else if (type == "button") {
		pProp = CreateButtonProp(design);
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
	else if (type == L"slider") {
		pProp = CreateSliderProp(design);
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

	BOOL hasCheck = design.GetBoolean("hasCheck", false) ? TRUE : FALSE;
	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design), NULL, FALSE, hasCheck);
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



CBCGPProp* Control::PropList::CreateButtonProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new Property::CommandButton(
		Facility::GetTitle(design),
		Facility::Local(design.GetString("command")),
		Facility::GetId(design),
		Facility::GetDesciption(design));
	SetPropName(pProp, design);

	return pProp;
}



CBCGPProp* Control::PropList::CreateCheckProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new CBCGPProp(Facility::GetTitle(design), id,
		false, Facility::GetDesciption(design));
	SetPropName(pProp, design);

	return pProp;
}



CBCGPProp* Control::PropList::CreateColorProp(Json::Object& design, UINT id)
{
	id = (id != 0 ? id : PRESET::GetControlId());

	CBCGPProp* pProp = new Property::Color(Facility::GetTitle(design), id,
		(COLORREF)0, Facility::GetDescription(design));
	SetPropName(pProp, design);

	return pProp;
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



CBCGPProp* Control::PropList::CreateSliderProp(Json::Object& design, UINT id)
{
	Property::Slider* pProp = new Property::Slider(Facility::GetTitle(design), id,
		0, Facility::GetDescription(design));
	SetPropName(pProp, design);

	pProp->SetRange(
		design.GetInteger("min"),
		design.GetInteger("max"),
		design.GetInteger("step", 1)
	);

	return pProp;
}

#undef PRESET
