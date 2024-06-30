#pragma once

#include "Object.h"

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
	}
}