#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Object.h"

namespace KERNEL
{
	namespace Operator
	{
		enum class Type
		{
			HighlightObjectSnap,
			VisualEffects,
			Count
		};

		class OperatorBase : public Object
		{
		public:
			OperatorBase();
		};
	}
}