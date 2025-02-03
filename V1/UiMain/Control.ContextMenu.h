#pragma once

#include "Control.h"

// TEMP - Popup or ContextMenu?
//--------------------------------------------------------------------------------------------------

namespace Control
{
	class ContextMenu : public CBCGPPopupMenu
	{
	protected:

		DECLARE_MESSAGE_MAP()

		ResourceIds m_ojectSnap;
		ResourceIds m_selFilter;

	public:

		ContextMenu();

		bool Initailize(CWnd* pParentWnd, CPoint pos, int eDocType /* Window::EDocType */);

		void AddItem(UINT id);

		void AddItems(const ResourceIds& ids, CSize imageSize);

		void AddSeperator();
	};
}
