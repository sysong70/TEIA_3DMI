#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"

namespace KERNEL
{
	namespace Command
	{
		class InputKit : public Object
		{
			enum class Type
			{
				Coordinate,
				String
			};

			InputKit();
			InputKit(InputKit const & cInThat);

			void Set(InputKit const & cInThat);
			InputKit & operator = (InputKit const & cInThat);

			size_t CoordinateCount() const;
			H3DF::Point Coordinate(size_t nIndex);
			
			size_t StringCount() const;
			CString String(size_t nIndex);
		};

		class InputManager : public CommandBase
		{
		public:
			InputManager() = default;
		};
	}
}