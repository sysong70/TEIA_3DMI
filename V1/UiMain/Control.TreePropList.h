#pragma once

#include "Control.h"
#include "Control.PropList.h"
#include "Control.TreeCtrlEx.h"

//--------------------------------------------------------------------------------------------------

using OnPropertyChangedFunc = void (CWnd::*)(Json::Value& target, CBCGPProp& source);

//--------------------------------------------------------------------------------------------------

namespace Control
{
	class TreePropList : public CWnd
	{
	public:

		TreePropList();

		~TreePropList() override;

		TreeCtrlEx& GetTree() {
			return m_tree;
		}

		PropList& GetPropList() {
			return m_propList;
		}

	public:

		bool Initialize(CWnd* pParentWnd, UINT id = WM_USER, const RECT& rect = {});

		void InitializeDesign(Json::Object& design);

		void InitializeData(Json::Object& data);

		void RefreshData();

		void OnPropertyChangedHandler(CWnd* pTarget) {
			m_onPropertyChangedHandler = pTarget;
		}

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		afx_msg void OnSize(UINT nType, int cx, int cy);

		afx_msg void OnTreeSelChanged(NMHDR* pNMHDR, LRESULT* pResult);

		afx_msg LRESULT OnPropertyChanged(WPARAM wp, LPARAM lp);

		DECLARE_MESSAGE_MAP()

	protected:

		TreeCtrlEx m_tree;
		PropList m_propList;

		CWnd* m_onPropertyChangedHandler = nullptr;

		bool m_bModified = false;
		Json::Object* m_pDesign = nullptr;
		Json::Object* m_pData = nullptr;

		void ChangePropList(HTREEITEM pItem);
	};
}
