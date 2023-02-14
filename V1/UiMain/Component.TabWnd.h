#pragma once

#include "Component.h"



namespace Component
{
	class TabWnd : public CBCGPTabWnd
	{
	public:

		TabWnd();

		void SetImageList(std::vector<UINT> ids, CSize imageSize);
		//:WARNING - use image size, setting before Create()
		void SetTabHeight(int height);

	protected:

		DECLARE_MESSAGE_MAP()
	};
}
