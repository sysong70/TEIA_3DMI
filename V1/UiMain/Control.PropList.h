#pragma once

#include "Control.h"
#include <Json.h>
#include <unordered_map>



namespace Control
{
	class PropList : public CBCGPPropList
	{
	public:

		friend class TreeCtrlEx;
		friend class TreePropList;

		PropList();

		~PropList() override;

		bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

		void InitializeDesign(Json::Object& design);

		void InitializeData(Json::Object& data);

	public:

		void Enable(bool value);

		void Enable(const CString& name, bool value);
		// from root item
		CBCGPProp* FindPropByName(const CString& name);
		// sub items
		CBCGPProp* FindPropByName(CBCGPProp* pParent, const CString& name);

		void GetAncestor(CBCGPProp* pItem, std::vector<CBCGPProp*>& parent);

		Json::Object& GetData();
		// from pItem->GetData()
		CString GetItemNamePath(CBCGPProp* pItem);

		void SetPropData(CBCGPProp* pProp, Json::Value* pValue);
		//:WARNING - do not use Name, use XMLTagName
		void SetPropName(CBCGPProp* pProp, Json::Object& design);

		void SetPropValue(CBCGPProp* pProp, Json::Value* pValue);

	protected:

		BOOL PreTranslateMessage(MSG* pMsg) override;

		void OnPropertyChanged(CBCGPProp* pProp) const override;
		//:WARNING - for Property::CommandButton highlight
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		CBCGPProp* CreateProp(Json::Object& design);

		CBCGPProp* CreateGroupProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateButtonProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateCheckProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateColorProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateDropdownProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateEditProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateFileProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateFolderProp(Json::Object& design, UINT id = 0);

		CBCGPProp* CreateSliderProp(Json::Object& design, UINT id = 0);

	protected:

		bool m_bInitialized = false;
		Json::Object* m_pDesign = nullptr;
		Json::Object* m_pData = nullptr;
	};
}
