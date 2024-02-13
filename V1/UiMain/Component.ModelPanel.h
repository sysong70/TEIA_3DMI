#pragma once

#include "Component.h"
#include "Control.Panel.h"

//--------------------------------------------------------------------------------------------------

// class in Component.ModelPanel.cpp
class ModelTree;

//--------------------------------------------------------------------------------------------------

namespace Component
{
	class ModelPanel : public Control::Panel
	{
	public:

		ModelPanel();

		~ModelPanel() override;

		void ReceiveSignal(Json::Object* pData);

	private:

		void AdjustLayout(int cx, int cy) override;

		void ConstructBody() override;
		// Create toolbar
		int ConstructHeader(int cx) override;

	private:

		afx_msg void OnCommand(UINT id);

		afx_msg LRESULT OnTreeCheckClick(WPARAM wp, LPARAM lp);

		afx_msg void OnTreeBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeClick(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeDblClick(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeItemExpanded(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeRClick(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeRDbClick(NMHDR* pNMHDR, LRESULT* pResult);
		 
		afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeSelChanging(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnTreeSetFocus(NMHDR* pNMHDR, LRESULT* pResult);

		//afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp);

		DECLARE_MESSAGE_MAP()

	private: // Signal responser

		// use in AddChildren()
		HTREEITEM AddItem(HTREEITEM parent, DWORD_PTR key, LPWSTR title, bool hasChildren, int type);

		HTREEITEM AddItem(Json::Object* pData);

		void AddChildren(Json::Object* pData);

		void CheckItem(Json::Object* pData);

		void CollapseItem(Json::Object* pData);

		void DeleteItem(Json::Object* pData);

		void ExpandItem(Json::Object* pData);

		void ExpandParent(Json::Object* pData);

		void SelectItem(Json::Object* pData);

	private:

		void GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& list);

		HTREEITEM GetItem(DWORD_PTR key);

		void RedrawTree(bool value);

	private:

		std::unordered_map<DWORD_PTR, HTREEITEM> m_keyMap;
		ModelTree* m_pControl = nullptr;
		bool m_bExpanding = false;
	};
}
