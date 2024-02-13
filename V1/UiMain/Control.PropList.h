#pragma once

#include "Control.h"

//--------------------------------------------------------------------------------------------------

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

		void Expand(const CString& name, bool value);
		// from root item
		CBCGPProp* FindPropByName(const CString& name);
		// sub items
		CBCGPProp* FindPropByName(CBCGPProp* pParent, const CString& name);

		void GetAncestor(CBCGPProp* pItem, std::vector<CBCGPProp*>& parent);

		Json::Object& GetData();
		// from pItem->GetData()
		CString GetItemNamePath(CBCGPProp* pItem);

		void SetPropData(CBCGPProp* pProp, Json::Value* pValue);

		void SetPropEnable(CBCGPProp* pProp, Json::Object& design);

		void SetPropId(CBCGPProp* pProp, Json::Object& design);
		//:WARNING - do not use Name, use XMLTagName
		void SetPropName(CBCGPProp* pProp, Json::Object& design);

		void SetPropValue(CBCGPProp* pProp, Json::Value* pValue);

		void Show(const CString& name, bool value);

		void ShowAll(bool value);

	protected:

		BOOL PreTranslateMessage(MSG* pMsg) override;

		void OnPropertyChanged(CBCGPProp* pProp) const override;
		//:WARNING - for Property::CommandButton highlight
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		void SetProperty(CBCGPProp* pProp, Json::Object& design);

		CBCGPProp* CreateProp(Json::Object& design);

		CBCGPProp* CreateGroupProp(Json::Object& design);

		CBCGPProp* CreateButtonProp(Json::Object& design);

		CBCGPProp* CreateCheckProp(Json::Object& design);

		CBCGPProp* CreateColorProp(Json::Object& design);

		CBCGPProp* CreateCoordProp(Json::Object& design);

		CBCGPProp* CreateDropdownProp(Json::Object& design);

		CBCGPProp* CreateEditProp(Json::Object& design);

		CBCGPProp* CreateFileProp(Json::Object& design);

		CBCGPProp* CreateFolderProp(Json::Object& design);

		CBCGPProp* CreateFoldersProp(Json::Object& design);

		CBCGPProp* CreateFontProp(Json::Object& design);

		CBCGPProp* CreateSliderProp(Json::Object& design);

	protected:

		bool m_bInitialized = false;
		Json::Object* m_pDesign = nullptr;
		Json::Object* m_pData = nullptr;
	};
}
