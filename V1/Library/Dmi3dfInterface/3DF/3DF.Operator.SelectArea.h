#pragma once

#include "3DF.h"

#include <HOpSelectArea.h>

OPEN_3DF_NAMESPACE

namespace Operator
{
	class SelectArea : public HOpSelectArea
	{
	public:
		SelectArea(HBaseView * view, int DoRepeat = 0, int DoCapture = 1);
		const char * GetName() override;
		HBaseOperator * Clone() override;

		int OnLButtonDown(HEventInfo & cEvent) override;
		//int OnRButtonDownAndMove(HEventInfo & cEvent) override;
		int OnLButtonUp(HEventInfo & cEvent) override;

	protected:
		int ButtonUp(HEventInfo & cEvent);
	};
}

CLOSE_3DF_NAMESPACE