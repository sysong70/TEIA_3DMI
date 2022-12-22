#pragma once

#include <vector>
#include <unordered_map>

#include "ModelTree.h"

using TreeMap = std::unordered_map<int, CBCGPTreeCtrl *>;

class ModelBrowserBar : public CBCGPDockingControlBar
{
public:
	ModelBrowserBar();

	//== Model Tree Function =======================================================================
	ModelTreeCtrl * CreateModelTreeCtrl(DWORD_PTR nId);

	void Init(DWORD_PTR nDocId, ModelTreeCtrl * pcModelTree);

	int GetImageBaseIndex(int nType) const { return m_anImageBaseIndicesArray[nType]; }

protected:
	//ModelTreeCtrl modelTree;
	CTreeCtrl * m_pwndCurrentTreeCtrl = nullptr;
	
// Attributes
protected:
	CBCGPTreeCtrl * m_pwndTreeCtrl = nullptr;

	CBCGPTreeCtrl m_cEmptyTree;

	TreeMap m_mpcTreeMap;

	DWORD_PTR m_nDocId = 0xffffff;
	UINT m_nTreeId = 1;

	CImageList m_cImageList;
	std::vector<int> m_anImageBaseIndicesArray;

// Operations
public:

// Overrides

// Implementation
public:
	virtual ~ModelBrowserBar();

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
