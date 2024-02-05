#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Operator.h"

#include "../Signal/Signal.h"

#include <HEventInfo.h>

namespace KERNEL
{
	namespace Operator
	{
		class ModelPanel;

		class Select : public OperatorBase
		{
		public:
			Select(const H3DF::View * pcInView, const Signal::Delivery * pcInDelivery);
			void SetModelPanel(ModelPanel * pcInModelPanel);

			int MouseMove(HEventInfo & cInEvent);

			int LButtonDown(HEventInfo & cInEvent);
			int LButtonUp(HEventInfo & cInEvent);

			//== Object Snap 관련 함수 ===============================================================
			void DrawSnapItems();
			void SetObjectSnapMode(OSnap::Type eInType);

			//== Select 관련 함수 ====================================================================
			bool SelectByResult(H3DF::SelectionResults & cInResults);
			bool DynamicSelectByResult(H3DF::SelectionResults & cInResults);

			void SetSelectionFilter(SelectionFilter::Type eInType);

			//== Highlight 관련 함수 =================================================================
			void UnhighlightEverything();
		};
	}
}