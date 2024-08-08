#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"

namespace H3DF
{
/*
	class API_3DF AttributeLockKit : public Kit
	{
	public:
		AttributeLockKit();
		AttributeLockKit(AttributeLockKit const & cInKit);

		void Set(AttributeLockKit const & cInThat);
		AttributeLockKit const & operator = (AttributeLockKit const & cInThat);

		void Set(AttributeLockKit const & cInKit);

		void Show(AttributeLockKit & cOutKit) const;

		AttributeLockKit & SetLock(AttributeLock::Type in_type, bool in_state = true);
		AttributeLockKit & SetLock(AttributeLockTypeArray const & in_types, BoolArray const & in_states);

		AttributeLockKit & SetSubsegmentLockOverride(AttributeLock::Type in_type, bool in_state = true);
		AttributeLockKit & SetSubsegmentLockOverride(AttributeLockTypeArray const & in_types, BoolArray const & in_states);

		AttributeLockKit & UnsetLock(AttributeLock::Type in_type = AttributeLock::Type::Everything);
		AttributeLockKit & UnsetLock(AttributeLockTypeArray const & in_types);

		AttributeLockKit & UnsetSubsegmentLockOverride(AttributeLock::Type in_type = AttributeLock::Type::Everything);
		AttributeLockKit & UnsetSubsegmentLockOverride(AttributeLockTypeArray const & in_types);

		AttributeLockKit & UnsetEverything();

		bool ShowLock(AttributeLock::Type in_type, bool & out_state) const;
		bool ShowLock(AttributeLockTypeArray & out_types, BoolArray & out_states) const;

		bool ShowSubsegmentLockOverride(AttributeLock::Type in_type, bool & out_state) const;
		bool ShowSubsegmentLockOverride(AttributeLockTypeArray & out_types, BoolArray & out_states) const;
	};
*/
	class API_3DF AttributeLockControl : public Control
	{
	public:
		explicit AttributeLockControl(SegmentKey & cInSegmentKey);
		AttributeLockControl(AttributeLockControl const & cInThat);

		static const H3DF::Type staticType = H3DF::Type::AttributeLockControl;
		H3DF::Type ObjectType() const { return staticType; };

		void Set(AttributeLockControl const & cInThat);
		AttributeLockControl & operator = (AttributeLockControl const & cInThat);

		AttributeLockControl & SetLock(AttributeLock::Type eInType, bool bInState = true);
		AttributeLockControl & SetLock(AttributeLockTypeArray const & eInTypes, BoolArray const & bInStates);

		bool ShowLock(AttributeLock::Type eInType, bool & bOutState) const;
	};
}