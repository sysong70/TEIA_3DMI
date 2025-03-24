#pragma once

#include "Ctl.Panels.h"

class CtlModelTree;

//--------------------------------------------------------------------------------------------------

class CtlPanel3dModel : public CtlPanel3d
{
public:

	CtlModelTree* TreeCtl = nullptr;
	bool DisableNofify = false;

public:

	CtlPanel3dModel();

	~CtlPanel3dModel() override;

public:

	void AdjustLayout(int cx, int cy) override;

	void ConstructBody() override;
	// Create toolbar
	int ConstructHeader(int cx) override;

	void ReceiveSignal(Json::Object* pData) override;

public:

	afx_msg void OnCommandSort();

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

public: // Signal responser

	HTREEITEM AddItem(Json::Object* pData);

	void AddChildren(Json::Object* pData);

	void CheckItem(Json::Object* pData);

	void CheckItems(Json::Object* pData);

	void DeleteItem(Json::Object* pData);

	void ExpandItem(Json::Object* pData);

	void ExpandParent(Json::Object* pData);

	void SelectItem(Json::Object* pData);

	void SelectItems(Json::Object* pData);
};

