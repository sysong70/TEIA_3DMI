#pragma once

#include "Ctl.h"

//:TODO
//--------------------------------------------------------------------------------------------------

class CtlPopupMenu : public CBCGPListBox
{
public:

	CtlPopupMenu();

public:

	void AddItem(UINT id);

	void AddItems(const Ctl::ResourceIds& ids, CSize imageSize);

	void SetCheckItems(const Ctl::ResourceIds& ids) { DEBUG_STOP; }

public:

	void OnClickItem(int index) override { DEBUG_STOP; }

	DECLARE_MESSAGE_MAP()
};
