#pragma once


#include <3DF/3DF.h>
#include <3DF/Math.h>
#include <3DF/Selection.h>

#include "Command.h"
#include "Command.Set.h"

#include "../Signal/Signal.h"

#include "Command.Event.h"

namespace KERNEL
{
	namespace Command
	{
		class HighlightObjectSnap : public Set
		{
		public:
			HighlightObjectSnap(const Session * pcInSession);

			Command::Result::Type MouseMove(Event & cInEvent);

			void DrawSnapItems();

			void SetObjectSnapMode(DWORD nInSnapMode);

			void SetSelectionFilter(DWORD nInSelFilter);
						
			H3DF::SelectionItem & DynamicHighlightSelectionItem();

			void UnhighlightEverything();

			void Reset(bool bUpdate = true);
		};
	}
}