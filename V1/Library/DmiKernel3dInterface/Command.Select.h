#pragma once

#include <3DF/3DF.h>
#include <3DF/Math.h>

#include "Command.h"
#include "Command.Set.h"

#include "../Signal/Signal.h"

#include <HEventInfo.h>

namespace KERNEL
{
	namespace Command
	{
		class ModelPanel;

		class Select : public Set
		{
		public:
			Select(const Session * pcInSession);

			Command::Result::Type MouseMove(Event & cInEvent);
			Command::Result::Type LButtonDown(Event & cInEvent);
			Command::Result::Type LButtonUp(Event & cInEvent);

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

			H3DF::SelectionItem & DynamicHighlightSelectionItem();

			void Unhighlight(H3DF::SelectionResults const & cInItems);
			void UnhighlightEverything();

			void SetMouseDownTickCount(ULONGLONG nInCount);
			void SetMouseUpTickCount(ULONGLONG nInCount);
		};
	}
}