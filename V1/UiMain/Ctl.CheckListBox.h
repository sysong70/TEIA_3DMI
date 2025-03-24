#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlCheckListBox : public CBCGPCheckListBox
{
public:

	CtlCheckListBox();

public:

	// Get image, string, tooltip from resource
	void AddItem(UINT resourceId);

	void AddItems(const Ctl::ResourceIds& ids, CSize imageSize);

	void SetImageList(const Ctl::ResourceIds& ids, CSize imageSize);

public:

	void OnClickItem(int index) override;

	DECLARE_MESSAGE_MAP()
};
