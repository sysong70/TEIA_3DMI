#pragma once

#include <memory>

#include <Json.h>

class CMainFrame;
class DmiHpsComponentTreeItem;

class ModelTreeCtrl : public CBCGPTreeCtrl
{
public:
	ModelTreeCtrl(DWORD_PTR nId);

	void InsertJsonItem(HTREEITEM hParentItem, Json::Object & cInObject);
	void OnSelection(DmiHpsComponentTreeItem * pcComponentItem);
protected:
	afx_msg void OnRClick(NMHDR * pNMHDR, LRESULT * pResult);
	afx_msg void OnContextMenu(CWnd * pWnd, CPoint ptMousePos);
	afx_msg void OnTvnItemexpanded(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()

	CMainFrame * GetMainFrame() { return (CMainFrame *) AfxGetMainWnd(); }

private:
	DWORD_PTR m_nId = 0xffffff;
public:
	afx_msg void OnTvnSelchanged(NMHDR * pNMHDR, LRESULT * pResult);
};
