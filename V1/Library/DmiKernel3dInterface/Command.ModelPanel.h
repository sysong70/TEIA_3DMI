#pragma once

#include <3DF/3DF.h>

#include "Command.h"
#include "Command.Set.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Command
	{
		class Select;

		class ModelPanel : public Set
		{
		public:
			ModelPanel(const Session * pcInSession);

			void Initialize(H3DF::CADModel & cInCadModel);
			void Initialize(CString strFilePathName);

			void Signal(Json::Object & cInObject);

			void SelectTreeItem(H3DF::SelectionItem & cSelItem);

			void Checked(H3DF::SelectionResults & cInResults, bool bInChecked);
			void CheckedAll(bool bChecked);
			void CheckedUpdate(H3DF::Component & cInComponent);

		protected:
			void OnItemExpandedSignal(Json::Object & cInObject);

			void OnItemSelectedSignal(Json::Object& cInObject);
			void ExchangeMkpViewSelectedSignal(H3DF::Component * pcInComponent);
			void ProductOccurrenceSelectedSignal(H3DF::Component * pcInComponent);

			void OnItemCheckedSignal(Json::Object & cInObject);

			void OnItemDblClickedSignal(Json::Object & cInObject);
		};
	}
}	