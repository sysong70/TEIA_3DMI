#pragma once

#include "Object.h"

namespace H3DF
{
	class WindowKey;

	namespace Operator
	{
		enum class Result : UINT;
	}
}

namespace KERNEL
{
	namespace Command
	{
		enum class Type
		{
			None				= 0x00000000,
			General				= 0x01000000,
			VisualEffects		= 0x01000001,
			Attribute			= 0x01000001,
			Camera				= 0x01000002,
			Select				= 0x01000003,
			ModelPanel			= 0x01000004,

			PMI					= 0x02000000,
			PMI_Distance		= 0x02000001,

			Count
		};

		class Result
		{
		public:
			enum class Type : UINT
			{
				None = 0,
				Pass = 1 << 1,
				Consume = 1 << 2,
				Complete = 1 << 3,
			};

			static Type Convert(H3DF::Operator::Result eInType);
		};
	}
}