#pragma once

#include <3DF/3DF.h>

#include "Operator.h"

#include "../Signal/Signal.h"

namespace KERNEL
{
	namespace Operator
	{
		class Select;

		class ModelPanel : public OperatorBase
		{
		public:
			ModelPanel(const DocView * pcInDocView);

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
			void OnItemCheckedSignal(Json::Object & cInObject);

			void OnItemDblClickedSignal(Json::Object & cInObject);
		};
	}
}	