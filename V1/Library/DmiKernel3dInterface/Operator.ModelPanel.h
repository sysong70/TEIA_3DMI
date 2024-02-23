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
			void Initialize(CString strFilePathName);
			void SetSelect(Select * pcInSelect);

			void Signal(Json::Object & cInObject);

			void SetSelectItem(H3DF::SelectionItem & cSelItem);

		protected:
			void OnItemExpandedSignal(Json::Object & cInObject);

			void OnItemSelectedSignal(Json::Object& cInObject);
			void OnItemCheckedSignal(Json::Object & cInObject);
			void OnItemCheckedSignalSelect(Json::Object & cInObject);
		};
	}
}	