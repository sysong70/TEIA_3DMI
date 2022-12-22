#pragma once

#include "3DF.h"

#include <HOpCameraPan.h>

OPEN_3DF_NAMESPACE

class OpCameraPan : public HOpCameraPan
{
public:
	OpCameraPan(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);

	int OnRButtonDown(HEventInfo & hevent) override;
	int OnRButtonDownAndMove(HEventInfo & hevent) override;
	int OnRButtonUp(HEventInfo & hevent) override;
};

CLOSE_3DF_NAMESPACE