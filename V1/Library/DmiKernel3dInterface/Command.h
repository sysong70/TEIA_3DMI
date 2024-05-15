#pragma once

#include "Object.h"

namespace KERNEL
{
	class DocViewImpl;

	namespace Command
	{
		enum class Type
		{
			VisualEffects,
			Attribute,
			Camera,
			Select,
			ModelPanel,
			Count
		};

		class API_KERNEL CommandBase : public Object
		{
		public:
			CommandBase() = default;
		};
	}
}