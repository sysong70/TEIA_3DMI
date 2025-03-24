#pragma once

#include "Ctl.h"

//:TEMP - Popup or ContextMenu?
//--------------------------------------------------------------------------------------------------

class CtlContextMenu : public CBCGPPopupMenu
{
public:

	Ctl::ResourceIds OjectSnap;
	Ctl::ResourceIds SelectionFilter;

public:

	CtlContextMenu() {}

public:

	bool Initailize(CWnd* pParentWnd, CPoint pos, int eDocType /* Wnd::EDocType */) { RETURN_FALSE; }

	void AddItem(UINT id) { DEBUG_STOP; }

	void AddItems(const Ctl::ResourceIds& ids, CSize imageSize) { DEBUG_STOP; }

	void AddSeperator() { DEBUG_STOP; }

public:

	DECLARE_MESSAGE_MAP()
};
