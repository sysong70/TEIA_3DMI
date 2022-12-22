#pragma once

#include "Component.h"



namespace Component
{
	class TabWnd : public CBCGPTabWnd
	{
	public:

		TabWnd();

		void SetImageSize(CSize size);

		void AddImages(std::vector<UINT> ids);
	};
}
