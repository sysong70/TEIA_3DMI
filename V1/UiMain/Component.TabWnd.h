#pragma once

#include "Component.h"



namespace Component
{
	class TabWnd : public CBCGPTabWnd
	{
	public:

		TabWnd();

		void SetImageSize(CSize size);
		//:WARNING - use image size
		void SetTabHeight(int height);

		void AddImages(std::vector<UINT> ids);

	protected:

		afx_msg BOOL OnEraseBkgnd(CDC* pDC);

		DECLARE_MESSAGE_MAP()
	};
}
