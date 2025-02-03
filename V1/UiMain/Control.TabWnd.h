#pragma once

#include "Control.h"

//--------------------------------------------------------------------------------------------------

namespace Control
{
	class TabWnd : public CBCGPTabWnd
	{
	public:

		TabWnd();

		void SetImageList(const ResourceIds& ids, CSize imageSize);
		// WARNING - use image size, setting before Create()
		void SetTabHeight(int height);

	protected:

		DECLARE_MESSAGE_MAP()
	};
}
