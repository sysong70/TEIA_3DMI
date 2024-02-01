#pragma once

#include "Component.h"
#include "Control.Panel.h"
#include <Json.h>
#include <unordered_map>



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

		void OnCommand(UINT id);

	private:

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

		void AddItem(HTREEITEM parent, DWORD_PTR key, CString title, bool hasChildren, int type);

		void AddItems(Json::Object* pData);

		void AddChildren(Json::Object* pData);

		void GetAncestorData(HTREEITEM pItem, std::list<DWORD_PTR>& list);

		HTREEITEM GetItem(DWORD_PTR key);

	private:

		std::unordered_map<DWORD_PTR, HTREEITEM> m_keyMap;
		CBCGPTreeCtrlEx m_wndControl;

		CString m_sFilterMessage = L"";
		bool m_bAlternateRows = false;
		bool m_bCheckBoxes = false;
		bool m_bGridLines = false;

		void EnableAlternateRows();

		void EnableGridLines();
	};
}
