#pragma once

#include "Component.h"
#include "Json.h"



namespace Component
{
	class TreeCtrlEx : public CBCGPTreeCtrlEx
	{
	public:

		friend class PropList;
		friend class TreePropList;

		TreeCtrlEx();

		~TreeCtrlEx() override;

		void InitializeDesign(Json::Array& design);

		void GetAncestor(HTREEITEM pItem, std::vector<HTREEITEM>& parent);
		// from GetItemData(pItem)
		CString GetItemNamePath(HTREEITEM pItem);

	protected:

		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

		DECLARE_MESSAGE_MAP()

	protected:

		bool m_bInitialized = false;

		HTREEITEM CreateItem(Json::Object& design, HTREEITEM pParent);

		void CreateItem(Json::Array& design, HTREEITEM pParent);
	};
}
