#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class HighlightObjectSnap : public OperatorBase
		{
		public:
			HighlightObjectSnap(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			int LButtonDownAndMove(int nFlags, int x, int y);
			
			int NoButtonDownAndMove(int nFlags, int x, int y);

			void DrawSnapItems();

			void SetObjectSnapMode(DWORD nInSnapMode);

			void SetSelectionFilter(DWORD nInSelFilter);
		};
	}
}