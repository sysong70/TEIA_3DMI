#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Object.h"

namespace KERNEL
{
	namespace Operator
	{
		class HighlightObjectSnap : public Object
		{
		public:
			HighlightObjectSnap();
			HighlightObjectSnap(H3DF::WindowKey * pcWindow);

			int LButtonDownAndMove(int nFlags, int x, int y);
			
			int NoButtonDownAndMove(int nFlags, int x, int y);

			void DrawSnapItems();

			void SetObjectSnapMode(DWORD nInSnapMode);
		};
	}
}