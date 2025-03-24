#include "stdafx.h"

#include "Ast.h"
#include "Ctl.Properties.h"
#include "Ctl.PropList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//**************************************************************************************************

namespace
{
	int ControlId = 0;
}

//**************************************************************************************************

BEGIN_MESSAGE_MAP(CtlPropList, CBCGPPropList)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



CtlPropList::CtlPropList()
{
	// WARNING - static members for Float/Double format (how to initialize?)
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



CtlPropList::~CtlPropList()
{
	DestroyWindow();
}



bool CtlPropList::Initialize(CWnd* pParentWnd, UINT id, const RECT& rect)
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



bool CtlPropList::InitializeDesign(Json::Object& design, CString dictionary)
{
	ControlId = WM_USER + 1;
	RemoveAll();

	Dictionary = dictionary;
	Initialized = false;
	DesignData = &design;

	SetRedraw(FALSE);
	{
		CreateProp(*DesignData);
	}
	SetRedraw(TRUE);

	return true;
}



bool CtlPropList::InitializeValue(Json::Object& data)
{
	ValueData = &data;

	Initialized = false;

	for (int i = 0; i < GetPropertyCount(); i++) {
		CBCGPProp* pProp = GetProperty(i);
		CString name = pProp->GetXMLTagName();

		if (pProp != nullptr && name.IsEmpty() == false) {
			SetPropData(pProp, ValueData->FindValue((CStringA)name));
		}
	}

	Initialized = true;
	//:WARNING - update window
	AdjustLayout();

	return true;
}



void CtlPropList::Enable(bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Enable((BOOL)value, TRUE);
	}
}

void CtlPropList::Enable(const CString& name, bool value)
{
	CBCGPProp* pFound = FindPropByName(name);
	if (pFound != nullptr) {
		pFound->Enable((BOOL)value, TRUE);
	}
	else {
		DEBUG_STOP;
	}
}



void CtlPropList::Expand(const CString& name, bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Expand((BOOL)value);
	}
}



CBCGPProp* CtlPropList::FindPropByName(const CString& name)
{
	for (int i = 0; i < GetPropertyCount(); i++) {
		CBCGPProp* pProp = GetProperty(i);
		// WARNING - do not use Name, use XMLTagName
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

CBCGPProp* CtlPropList::FindPropByName(CBCGPProp* pParent, const CString& name)
{
	for (int i = 0; i < pParent->GetSubItemsCount(); i++) {
		CBCGPProp* pProp = pParent->GetSubItem(i);
		// WARNING - do not use Name, use XMLTagName
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



void CtlPropList::GetAncestor(CBCGPProp* pItem, std::vector<CBCGPProp*>& ancestor)
{
	CBCGPProp* pParent = pItem;
	while (pParent != nullptr) {
		ancestor.push_back(pParent);
		pParent = pParent->GetParent();
	}
}



CString CtlPropList::GetItemNamePath(CBCGPProp* pItem)
{
	std::vector<CBCGPProp*> items;
	GetAncestor(pItem, items);

	CString path;
	std::vector<CBCGPProp*>::reverse_iterator iter;
	for (iter = items.rbegin(); iter != items.rend(); iter++) {
		CBCGPProp* pParent = *iter;
		// WARNING - do not use Name, use XMLTagName
		path += L'/' + pParent->GetXMLTagName();
	}

	path.TrimLeft(L'/');

	return path;
}



void CtlPropList::Select(const CString& name, UINT key)
{
	CBCGPProp* pFound = FindPropByName(name);
	if (pFound != nullptr) {
		pFound->DoEdit();
		if (key != 0) {
			pFound->PushChar(key);
		}
	}
}



void CtlPropList::SetPropData(CBCGPProp* pProp, Json::Value* pValue)
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



void CtlPropList::SetPropEnable(CBCGPProp* pProp, Json::Object& design)
{
	if (design.GetBoolean("enable", true) == false) {
		pProp->Enable(FALSE, TRUE);
	}
}



void CtlPropList::SetPropId(CBCGPProp* pProp, Json::Object& design)
{
	UINT id = Ast::GetId(design);
	pProp->SetID(id != 0 ? id : ++ControlId);
}



void CtlPropList::SetPropName(CBCGPProp* pProp, Json::Object& design)
{
	Json::Value* pValue = design.FindValue("name");
	if (pValue != nullptr) {
		// WARNING - do not use Name, use XMLTagName
		pProp->SetXMLTagName(pValue->AsString());
	}
	else {
		DEBUG_STOP;
	}
}



void CtlPropList::SetPropValue(CBCGPProp* pProp, Json::Value* pValue)
{
	DEBUG_VALID(pProp);
	DEBUG_VALID(pValue);

	Ast::SetValue(*pProp, *pValue);
}



void CtlPropList::Show(const CString& name, bool value)
{
	CBCGPProp* pFound = FindPropByName(name);
	if (pFound != nullptr) {
		pFound->Show((BOOL)value);
	}
	else {
		DEBUG_STOP;
	}
}



void CtlPropList::ShowAll(bool value)
{
	for (POSITION pos = m_lstProps.GetHeadPosition(); pos != nullptr;) {
		CBCGPProp* pProp = m_lstProps.GetNext(pos);
		pProp->Show((BOOL)value);
	}
}



BOOL CtlPropList::PreTranslateMessage(MSG* pMsg)
{
	BOOL process = __super::PreTranslateMessage(pMsg);

	if (pMsg->message == WM_KEYDOWN) {
		switch (pMsg->wParam) {
			case VK_ESCAPE:
				if (m_pSel == nullptr) {
					return process;
				}

				if (m_pSel->IsDroppedDown()) {
					OnCloseCombo();
					SetFocus();
					process = TRUE;
				}
				break;

			default:
				break;
		}
	}

	return process;
}



void CtlPropList::OnPropertyChanged(CBCGPProp* pProp) const
{
	__super::OnPropertyChanged(pProp);

	if (pProp->IsGroupWithCheckBox()) {
		GetOwner()->SendMessage(BCGM_PROPERTY_CHANGED, GetDlgCtrlID(), LPARAM(pProp));
	}
}



void CtlPropList::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if (m_pTracked != nullptr) {
		Prop::CommandButton* button = dynamic_cast<Prop::CommandButton*>(m_pTracked);
		if (button != nullptr) {
			button->Highlighted = true;
			button->Redraw();
		}
	}
}



void CtlPropList::SetProperty(CBCGPProp* pProp, Json::Object& design)
{
	SetPropId(pProp, design);
	SetPropName(pProp, design);
	SetPropEnable(pProp, design);
}



CBCGPProp* CtlPropList::CreateProp(Json::Object& design)
{
	CBCGPProp* pProp = nullptr;
	CString type = Ast::GetType(design);

	if (type == L"group") {
		pProp = CreateGroupProp(design);
	}
	else if (type == L"angle") {
		//:TODO
		pProp = CreateEditProp(design);
	}
	else if (type == L"button") {
		pProp = CreateButtonProp(design);
	}
	else if (type == L"check") {
		pProp = CreateCheckProp(design);
	}
	else if (type == L"color") {
		pProp = CreateColorProp(design);
	}
	else if (type == L"coord") {
		pProp = CreateCoordProp(design);
	}
	else if (type == "double") {
		//:TODO
		pProp = CreateEditProp(design);
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
	else if (type == L"folders") {
		pProp = CreateFoldersProp(design);
	}
	else if (type == L"font") {
		pProp = CreateFontProp(design);
	}
	else if (type == L"length") {
		//:TODO
		pProp = CreateEditProp(design);
	}
	else if (type == L"slider") {
		pProp = CreateSliderProp(design);
	}
	else if (type == L"root") {
		Json::Array* pItems = Ast::GetItems(design);
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

	return pProp;
}



CBCGPProp* CtlPropList::CreateGroupProp(Json::Object& design)
{
	BOOL hasCheck = design.GetBoolean("hasCheck", false) ? TRUE : FALSE;
	CBCGPProp* pProp = new CBCGPProp(Ast::GetTitle(design), NULL, FALSE, hasCheck);
	SetPropId(pProp, design);
	SetPropName(pProp, design);

	Json::Array* pItems = Ast::GetItems(design);
	if (pItems != nullptr) {
		for (Json::Value* pValue : pItems->GetBuffer()) {
			CBCGPProp* pSubItem = CreateProp(pValue->AsObject());
			if (pSubItem != nullptr) {
				pProp->AddSubItem(pSubItem);
			}
		}
	}

	SetPropEnable(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateButtonProp(Json::Object& design)
{
	CBCGPProp* pProp = new Prop::CommandButton(
		Ast::GetTitle(design),
		Ast::Local(design.GetString("command")),
		Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateCheckProp(Json::Object& design)
{
	CBCGPProp* pProp = new CBCGPProp(Ast::GetTitle(design), 0,
		design.GetBoolean("value"), Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateColorProp(Json::Object& design)
{
	COLORREF value = 0;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		ASSERT(pValue->IsString());
		value = Json::Helper::ToColor(pValue->AsString());
	}

	CBCGPProp* pProp = new Prop::Color(Ast::GetTitle(design),
		value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateCoordProp(Json::Object& design)
{
	int format = design.GetInteger("format", 3);
	int digit = design.GetInteger("digit", 4);
	float coord[3] = {};
	Json::Helper::GetCoordinate(design.FindValue("value"), coord);

	CString value = WStr::ToString(coord[0], digit) + L"," + WStr::ToString(coord[1], digit);
	if (format == 3) {
		value += L"," + WStr::ToString(coord[2], digit);
	}

	CBCGPProp* pProp = new Prop::Coordinate(Ast::GetTitle(design),
		value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateDropdownProp(Json::Object& design)
{
	// CHECK - int value?
	CBCGPProp* pProp = new CBCGPProp(Ast::GetTitle(design), 0,
		(LPCTSTR)L"", Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	std::vector<CString> items;
	Ast::GetItems(design, items);
	for (CString& item : items) {
		pProp->AddOption(item);
	}

	pProp->AllowEdit(FALSE);
	pProp->SelectOption(design.GetInteger("value"));

	return pProp;
}



CBCGPProp* CtlPropList::CreateEditProp(Json::Object& design)
{
	CString value;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		value = pValue->AsString();
	}

	CBCGPProp* pProp = new CBCGPProp(Ast::GetTitle(design), 0,
		(LPCTSTR)value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateFileProp(Json::Object& design)
{
	CString value;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		value = pValue->AsString();
	}

	CBCGPProp* pProp = new CBCGPProp(Ast::GetTitle(design), 0,
		(LPCTSTR)value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateFolderProp(Json::Object& design)
{
	CString value;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		value = pValue->AsString();
	}

	CBCGPFileProp* pProp = new CBCGPFileProp(Ast::GetTitle(design), 0,
		(LPCTSTR)value, NULL, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateFoldersProp(Json::Object& design)
{
	CString value;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		value = pValue->AsString();
	}

	Prop::FoldersDialog* pProp = new Prop::FoldersDialog(Ast::GetTitle(design),
		value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateFontProp(Json::Object& design)
{
	CString value;
	Json::Value* pValue = design.FindValue("value");
	if (pValue != nullptr) {
		value = pValue->AsString();
	}

	Prop::FontCombo* pProp = new Prop::FontCombo(Ast::GetTitle(design),
		value, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	return pProp;
}



CBCGPProp* CtlPropList::CreateSliderProp(Json::Object& design)
{
	Prop::Slider* pProp = new Prop::Slider(Ast::GetTitle(design),
		0, Ast::GetDescription(design, Dictionary));
	SetProperty(pProp, design);

	int min = design.GetInteger("min");
	int max = design.GetInteger("max");
	int step = design.GetInteger("step", 1);

	pProp->SetRange(min, max, step);

	return pProp;
}
