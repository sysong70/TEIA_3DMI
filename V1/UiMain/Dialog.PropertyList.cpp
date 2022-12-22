#include "stdafx.h"
#include "Dialog.PropertyList.h"
#include "Window.h"

OPEN_DIALOG_NAMESPACE

#pragma region Preset

struct
{
	const CSize PropListSize = globalUtils.ScaleByDPI(CSize(400, 650));
}
DlgPropListPreset;

#define PRESET DlgPropListPreset

#pragma endregion

#pragma region Message Map

BEGIN_MESSAGE_MAP(PropertyList, Standard)
END_MESSAGE_MAP()

#pragma endregion

PropertyList::PropertyList(CStringA dialogName, CWnd* pParent)
	: Standard(dialogName, pParent)
{
}



PropertyList::~PropertyList()
{
}



void PropertyList::DoDataExchange(CDataExchange* pDX)
{
	DEBUG_STOP;
	__super::DoDataExchange(pDX);
}



BOOL PropertyList::OnInitDialog()
{
	BOOL bResult = __super::OnInitDialog();

	SetSizeLimit(true, false);

	// Enables or disables layout manager for a window. (move and/or resize controls)
	EnableDynamicLayout();
	CMFCDynamicLayout* pLayout = GetDynamicLayout();
	pLayout->Create(this);

	pLayout->AddItem(m_wndPropList, CMFCDynamicLayout::MoveNone(), CMFCDynamicLayout::SizeHorizontalAndVertical(100, 100));
	pLayout->AddItem(m_wndOk, CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());
	pLayout->AddItem(m_wndCancel, CMFCDynamicLayout::MoveHorizontalAndVertical(100, 100), CMFCDynamicLayout::SizeNone());

	return bResult;
}

#define GetName(x)			x.GetString("name")
#define GetTitle(x)			Window::Local(x.GetString("title"))
#define GetDescription(x)	Window::Local(x.GetString("desc"))

CRect PropertyList::ConstructBody(CRect bound)
{
	CPoint bodyOffset = bound.TopLeft();
	CSize size = CreatePropList(m_wndPropList, GenControlId(), { bodyOffset, PRESET.PropListSize });

	Json::Array& properties = GetUiData().GetValue("properties").AsArray();
	for (Json::Value* pValue : properties.GetBuffer()) {
		CBCGPProp* pProp = CreateItem(pValue->AsObject());
		if (pProp) {
			m_wndPropList.AddProperty(pProp);
		}
	}

	//CStringList commandList;
	//int index = 0;
	//Json::Array& commands = GetUiData().GetAt("commends")->AsArray();

	//for (Json::Value* pValue : commands.GetBuffer()) {
	//	Json::Object& data = pValue->AsObject();

	//	CString name = GetName(data);
	//	CString title = GetTitle(data);
	//	commandList.AddTail(title);

	//	m_commands.push_back({ name, index++ });
	//}

	//if (commandList.GetCount() > 0) {
	//	m_wndPropList.SetCommandsVisible();
	//	m_wndPropList.SetCommands(commandList, 0);
	//}

	return { bodyOffset, size };
}



CRect PropertyList::ConstructFooter(CRect bound)
{
	CPoint point = bound.BottomRight();
	CSize size;
	size = CreateConfirmButton(m_wndCancel, IDCANCEL, point, Standard::Confirm::Cancel);
	size = CreateConfirmButton(m_wndOk, IDOK, { point.x - size.cx, point.y }, Standard::Confirm::Ok);

	bound.bottom += size.cy;
	
	return bound;
}



CBCGPProp* PropertyList::CreateItem(Json::Object& data)
{
	CBCGPProp* pProp = nullptr;
	CString type = data.GetString("type");

	if (type == L"group") {
		pProp = CreateGroup(data);
	}
	else if (type == L"check") {
		pProp = CreateCheck(data);
	}
	else if (type == L"combo") {
		pProp = CreateCombo(data);
	}
	else if (type == L"edit") {
		pProp = CreateEdit(data);
	}
	else {
		DEBUG_STOP;
	}

	return pProp;
}



CBCGPProp* PropertyList::CreateGroup(Json::Object& data, UINT id)
{
	id = (id != 0 ? id : GenControlId());
	CString name = GetName(data);
	CString title = GetTitle(data);

	CBCGPProp* pProp = new CBCGPProp(title, id);
	m_properties.push_back({ id, name, pProp });

	Json::Array& list = data.GetValue("list").AsArray();
	for (Json::Value* pValue : list.GetBuffer()) {
		CBCGPProp* pSubItem = CreateItem(pValue->AsObject());
		if (pSubItem != nullptr) {
			pProp->AddSubItem(pSubItem);
		}
	}

	return pProp;
}



CBCGPProp* PropertyList::CreateCheck(Json::Object& data, UINT id)
{
	id = (id != 0 ? id : GenControlId());
	CString name = GetName(data);
	CString title = GetTitle(data);
	CString desc = GetDesciption(data);
	bool value = data.GetBoolean("value");

	CBCGPProp* pProp = new CBCGPProp(title, id, (_variant_t)value, desc);
	m_properties.push_back({ id, name, pProp });

	return pProp;
}



CBCGPProp* PropertyList::CreateCombo(Json::Object& data, UINT id)
{
	id = (id != 0 ? id : GenControlId());
	CString name = GetName(data);
	CString title = GetTitle(data);
	CString desc = GetDesciption(data);
	int value = data.GetInteger("value");

	CBCGPProp* pProp = new CBCGPProp(title, id, (_variant_t)L"", desc);
	m_properties.push_back({ id, name, pProp });

	std::vector<CString> list;
	GetListItems(data, list);
	for (CString& item : list) {
		pProp->AddOption(item);
	}

	pProp->AllowEdit(FALSE);
	pProp->SelectOption(value);

	return pProp;
}



CBCGPProp* PropertyList::CreateEdit(Json::Object& data, UINT id)
{
	id = (id != 0 ? id : GenControlId());
	CString name = GetName(data);
	CString title = GetTitle(data);
	CString desc = GetDesciption(data);

	CString sValue;
	Json::Value& value = data.GetValue("value");
	if (value.IsValid()) {
		sValue = value.ToString();
	}

	CBCGPProp* pProp = new CBCGPProp(title, id, (_variant_t)sValue, desc);
	m_properties.push_back({ id, name, pProp });

	return pProp;
}



CBCGPProp* PropertyList::GetPropertyById(UINT id)
{
	for (PropertyHolder holder : m_properties) {
		if (holder.Id == id) {
			return holder.pProperty;
		}
	}

	RETURN_NULL;
}



CBCGPProp* PropertyList::GetPropertyByName(const CString name)
{
	for (PropertyHolder holder : m_properties) {
		if (holder.Name == name) {
			return holder.pProperty;
		}
	}

	RETURN_NULL;
}



CBCGPProp* PropertyList::GetPropertyByName(const CStringA name)
{
	return GetPropertyByName(CString(name));
}



CString PropertyList::GetPropertyName(UINT id)
{
	for (PropertyHolder holder : m_properties) {
		if (holder.Id == id) {
			return holder.Name;
		}
	}

	return L"";
}



void PropertyList::ShowProperty(const CString name, bool bShow)
{
	CBCGPProp* pProp = GetPropertyByName(name);
	if (pProp != nullptr) {
		pProp->Show((BOOL)bShow);
	}
}



CString PropertyList::GetCommandName(int index)
{
	for (CommandHolder holder : m_commands) {
		if (holder.Index == index) {
			return holder.Name;
		}
	}

	return L"";
}



CBCGPPropList& PropertyList::GetPropList()
{
	return m_wndPropList;
}



std::vector<PropertyList::PropertyHolder>& PropertyList::GetProperties()
{
	return m_properties;
}



std::vector<PropertyList::CommandHolder>& PropertyList::GetCommands()
{
	return m_commands;
}

CLOSE_DIALOG_NAMESPACE