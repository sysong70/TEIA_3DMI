#pragma once

#include "Component.Panel.h"
#include "Json.h"
#include <unordered_map>



namespace Component
{
	class ModelTreePanel : public Panel
	{
	public:

		ModelTreePanel();

		~ModelTreePanel() override;

		void ReceiveSignal(Json::Object* pData);

	private:

		void AdjustLayout(int cx, int cy) override;

		void ConstructBody() override;
		// Create toolbar
		int ConstructHeader(int cx) override;

		void OnCommand(UINT id);

	private:

		afx_msg LRESULT OnCheckClickTree(WPARAM wp, LPARAM lp);

		afx_msg void OnBeginDragTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnBeginLabelEditTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnClickTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnDblClickTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnDeleteItemTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnEndLabelEditTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnItemExpandedTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnItemExpandingTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnRClickTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnRDbClickTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnSelChangedTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnSelChangingTree(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg void OnSetFocusTree(NMHDR* pNMHDR, LRESULT* pResult);

		//afx_msg LRESULT OnDPIChangedAfterParent(WPARAM wp, LPARAM lp);

		DECLARE_MESSAGE_MAP()

		void GetParent(HTREEITEM sel, std::vector<HTREEITEM>& parent);

	private:

		CBCGPTreeCtrlEx m_wndControl;

		CString m_sFilterMessage = L"";
		bool m_bAlternateRows = false;
		bool m_bCheckBoxes = false;
		bool m_bGridLines = false;

		void EnableAlternateRows();

		void EnableGridLines();
	};
}
