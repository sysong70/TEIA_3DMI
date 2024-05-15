#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"

#include "../Signal/Signal.h"

#include <HEventInfo.h>

namespace KERNEL
{
	namespace Command
	{
		class ModelPanel;

		class Select : public CommandBase
		{
		public:
			Select(const DocView * pcInDocView);

			int MouseMove(HEventInfo & cInEvent);

			int LButtonDown(HEventInfo & cInEvent);
			int LButtonUp(HEventInfo & cInEvent);

			//== Object Snap 관련 함수 ===============================================================
			void DrawSnapItems();
			void SetObjectSnapMode(OSnap::Type eInType);
			void ResetSnapItems(bool bUpdate = true);

			//== Select 관련 함수 ====================================================================
			bool SelectByResult(H3DF::SelectionResults & cInResults);
			bool DynamicSelectByResult(H3DF::SelectionResults & cInResults);

			void SetSelectionFilter(SelectionFilter::Type eInType);

			H3DF::SelectionResults & Results();

			//== Highlight 관련 함수 =================================================================
			H3DF::HighlightControl & DynHighlightControl();

			void Unhighlight(H3DF::SelectionResults const & cInItems);
			void UnhighlightEverything();

			void SetMouseDownTickCount(ULONGLONG nInCount);
			void SetMouseUpTickCount(ULONGLONG nInCount);
		};
	}
}