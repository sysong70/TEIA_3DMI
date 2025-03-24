#pragma once

#include "Ctl.h"

//--------------------------------------------------------------------------------------------------

class CtlTabWnd : public CBCGPTabWnd
{
public:

	CtlTabWnd();

	~CtlTabWnd() override { DestroyWindow(); }

public:

	void SetImageList(const Ctl::ResourceIds& ids, CSize imageSize);
	// WARNING - use image size, setting before Create()
	void SetTabHeight(int height);

public:

	DECLARE_MESSAGE_MAP()
};

