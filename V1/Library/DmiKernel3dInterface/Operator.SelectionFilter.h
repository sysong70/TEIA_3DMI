#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Object.h"

namespace KERNEL
{
	namespace Operator
	{
		class SelectionFilter : public Object
		{
		public:
			SelectionFilter(H3DF::WindowKey * pcWindow);

			int NoButtonDownAndMove(int nFlags, int x, int y);

			void DrawSnapItems();

			void SetObjectSnapMode(DWORD nInSnapMode);
		};
	}
}