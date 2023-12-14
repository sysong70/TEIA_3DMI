#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
	class API_3DF AttributeLockControl : public Control
	{
	public:
		explicit AttributeLockControl(SegmentKey & cInSegmentKey);
		AttributeLockControl(AttributeLockControl const & cInThat);

		void Set(AttributeLockControl const & cInThat);
		AttributeLockControl & operator = (AttributeLockControl const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::VisualEffectsControl; };

		AttributeLockControl & SetLock(AttributeLock::Type eInType, bool bInState = true);
	};
}