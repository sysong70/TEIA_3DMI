#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"
#include "Command.Set.h"

namespace KERNEL
{
	namespace Command
	{
		namespace PMI
		{
			class Distance : public KERNEL::Command::Set
			{
			public:
				Distance(const Session * pcInSession);

				KERNEL::Command::Type GetType() const override;
			};
		}
	}
}