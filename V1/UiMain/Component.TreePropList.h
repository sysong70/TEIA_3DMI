#pragma once

#include "Component.h"



namespace Component
{
	class TreePropList : public CWnd
	{
	public:

		TreePropList();

		~TreePropList() override;

		bool Initialize(CWnd* pParentWnd);

	protected:

		void PostNcDestroy() override;

		afx_msg void OnSize(UINT nType, int cx, int cy);

		DECLARE_MESSAGE_MAP()

	private:

		CString m_sFilterMessage;
		CBCGPTreeCtrlEx m_wndTree;
		CBCGPPropList m_wndPropList;

		void CreateTreeCtrl();

		void CreatePropList();
	};
}
