#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/Selection.h>

#include "Operator.h"

#include "../Signal/Signal.h"

#include <HEventInfo.h>

namespace KERNEL
{
	namespace Operator
	{
		class HighlightObjectSnap : public OperatorBase
		{
		public:
			HighlightObjectSnap(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);

			int LButtonDownAndMove(HEventInfo & cInEvent);
			
			int NoButtonDownAndMove(HEventInfo & cInEvent);

			void DrawSnapItems();

			void SetObjectSnapMode(DWORD nInSnapMode);

			void SetSelectionFilter(DWORD nInSelFilter);

			H3DF::SelectionItem & HighlightSelectionItem();

			void UnhighlightEverything();
		};
	}
}