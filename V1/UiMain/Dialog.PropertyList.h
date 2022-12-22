#pragma once

#include "Dialog.Standard.h"

OPEN_DIALOG_NAMESPACE

class PropertyList : public Standard
{
public:

	PropertyList(CStringA dialogName, CWnd* pParent = nullptr);

	~PropertyList() override;

protected:

	void DoDataExchange(CDataExchange* pDX) override;

	BOOL OnInitDialog() override;

	DECLARE_MESSAGE_MAP()

protected:

	CRect ConstructBody(CRect bound) override;

	CRect ConstructFooter(CRect bound) override;

protected:

	CBCGPProp* CreateItem(Json::Object& data);

	CBCGPProp* CreateGroup(Json::Object& data, UINT id = 0);

	CBCGPProp* CreateCheck(Json::Object& data, UINT id = 0);

	CBCGPProp* CreateCombo(Json::Object& data, UINT id = 0);

	CBCGPProp* CreateEdit(Json::Object& data, UINT id = 0);

	CBCGPProp* GetPropertyById(UINT id);

	CBCGPProp* GetPropertyByName(const CString name);

	CBCGPProp* GetPropertyByName(const CStringA name);

	CString	GetPropertyName(UINT id);

	void ShowProperty(const CString name, bool bShow = true);

	CString GetCommandName(int index);

	struct PropertyHolder
	{
		UINT Id;
		CString Name;
		CBCGPProp* pProperty;
	};

	struct CommandHolder
	{
		CString Name;
		int Index;
	};

	CBCGPPropList& GetPropList();

	std::vector<PropertyHolder>& GetProperties();

	std::vector<CommandHolder>& GetCommands();

private:

	CBCGPPropList m_wndPropList;

	CBCGPButton m_wndOk;

	CBCGPButton m_wndCancel;

	std::vector<PropertyHolder> m_properties;

	std::vector<CommandHolder> m_commands;
};

CLOSE_DIALOG_NAMESPACE