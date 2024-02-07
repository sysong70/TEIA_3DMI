#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Object.h"

namespace KERNEL
{
	class DocViewImpl;

	namespace Operator
	{
		enum class Type
		{
			VisualEffects,
			Visibility,
			Camera,
			Select,
			ModelPanel,
			Count
		};

		class OperatorBase : public Object
		{
		public:
			OperatorBase();

			void SetDocViewImpl(KERNEL::DocViewImpl * pcInDocView);
			KERNEL::DocViewImpl & DocViewImpl();
		};
	}
}