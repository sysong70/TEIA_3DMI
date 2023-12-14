#pragma once

#include "3DF.h"

#include "Kit.h"
#include "Control.h"
#include "Math.h"

namespace H3DF
{
	class API_3DF ConditionControl : public Control
	{
	public:
		ConditionControl(SegmentKey & cInSegmentKey);
		ConditionControl(ConditionControl const & cInThat);

		void Set(ConditionControl const & cInThat);
		ConditionControl & operator = (ConditionControl const & cInThat);

		H3DF::Type ObjectType() const { return H3DF::Type::ConditionControl; };

		size_t GetCount() const;
	};
}