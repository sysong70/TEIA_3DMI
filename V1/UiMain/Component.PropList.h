#pragma once

#include "Component.h"
#include "Json.h"
#include <unordered_map>


namespace Component
{
	class PropList : public CBCGPPropList
	{
	public:

		friend class TreeCtrlEx;
		friend class TreePropList;

		PropList();

		~PropList() override;

		void InitializeDesign(Json::Object& design);

		void InitializeData(Json::Object& data);

		void GetAncestor(CBCGPProp* pItem, std::vector<CBCGPProp*>& parent);

		Json::Object& GetData();
		// from pItem->GetData()
		CString GetItemNamePath(CBCGPProp* pItem);

	protected:

		DECLARE_MESSAGE_MAP()

	protected:

		CBCGPProp* CreateProp(Json::Object& design);

		CBCGPProp* CreateGroupProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateCheckProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateColorProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateDropdownProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateEditProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateFileProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateFolderProp(Json::Object& design, UINT id = 0);

	protected:

		bool m_bInitialized = false;
		Json::Object* m_pDesign = nullptr;
		Json::Object* m_pData = nullptr;

		// root items
		CBCGPProp* FindPropByData(CString& name);
		// sub items
		CBCGPProp* FindPropByData(CBCGPProp* pParent, CString& name);
		// set value pointer to pProp->m_dwData
		void ReplacePropData(CBCGPProp* pProp, Json::Value* pValue);
		// set name to pProp->m_dwData
		void SetPropName(CBCGPProp* pProp, Json::Object& design);
	};
}
