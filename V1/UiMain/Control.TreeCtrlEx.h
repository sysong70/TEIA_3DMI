#pragma once

#include "Control.h"

//--------------------------------------------------------------------------------------------------

namespace Control
{
	class TreeCtrlEx : public CBCGPTreeCtrlEx
	{
	public:

		friend class PropList;
		friend class TreePropList;

		TreeCtrlEx();

		~TreeCtrlEx() override;

		bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

		bool InitializeDesign(Json::Array& design);

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
